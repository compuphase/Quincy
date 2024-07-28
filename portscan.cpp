/* List valid COM ports
 *
 * For the Microsoft Windows part: see http://www.codeproject.com/system/serial_portsenum_fifo.asp
 * For the Linux part: see the source code of setserial.
 *
 * Partial copyright 2006-2024 CompuPhase
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * $Id: portscan.cpp 7151 2024-03-23 16:08:18Z thiadmer $
 */

#if defined __WIN32__ || defined _WIN32 || defined WIN32
  #define _CRT_SECURE_NO_WARNINGS
  #define _WINSOCK2API_       /* avoid double-loading WinSock2 */
  #define _WINSOCKAPI_
  #include <windows.h>
  #include <tchar.h>
#endif
#include <assert.h>
#include "portscan.h"

//---------------------------------------------------------------------------

#define MAX_NAME_PORTS 20

#if defined __WIN32__ || defined _WIN32 || defined WIN32

#include <setupapi.h>
#include <dbt.h>

#define RegDisposition_OpenExisting (0x00000001)   // open key only if exists
#define CM_REGISTRY_HARDWARE        (0x00000000)


typedef DWORD (WINAPI * CM_Open_DevNode_Key)(DWORD, DWORD, DWORD, DWORD, PHKEY, DWORD);

static CM_Open_DevNode_Key OpenDevNodeKey=NULL;
static HINSTANCE CfgMan=NULL;
static int Enum_numDev=0;


HPORT BeginEnumeratePorts(void)
{
  BOOL guidTest=FALSE;
  DWORD RequiredSize=0;
  HDEVINFO DeviceInfoSet;
  char* buf;

  Enum_numDev = 0;
  guidTest=SetupDiClassGuidsFromNameA("Ports",0,0,&RequiredSize);
  if(RequiredSize < 1)
    return (HPORT)-1;

  buf=(char *)malloc(RequiredSize*sizeof(GUID));
  assert(buf!=NULL);
  guidTest=SetupDiClassGuidsFromNameA("Ports",(GUID *)buf,RequiredSize*sizeof(GUID),&RequiredSize);

  if(!guidTest)
    return (HPORT)-1;

  DeviceInfoSet=SetupDiGetClassDevs((GUID *)buf,NULL,NULL,DIGCF_PRESENT);
  if(DeviceInfoSet == INVALID_HANDLE_VALUE)
    return (HPORT)-1;

  free(buf);

  return DeviceInfoSet;
}

bool EnumeratePortsNext(HPORT DeviceInfoSet, wxString &PortName)
{
  TCHAR DevName[MAX_NAME_PORTS]={0};
  int numport;

  SP_DEVINFO_DATA DeviceInfoData={0};
  DeviceInfoData.cbSize=sizeof(SP_DEVINFO_DATA);

  if (!DeviceInfoSet)
    return false;
  if (!OpenDevNodeKey) {
    if ((CfgMan=LoadLibrary(_T("cfgmgr32"))) == NULL)
      return false;
    if ((OpenDevNodeKey=(CM_Open_DevNode_Key)GetProcAddress(CfgMan,"CM_Open_DevNode_Key")) == NULL) {
      FreeLibrary(CfgMan);
      return false;
    } /* if */
  } /* if */

  bool result=false;
  while (!result) {
    HKEY KeyDevice;
    DWORD len;
    int res1=SetupDiEnumDeviceInfo(DeviceInfoSet,Enum_numDev,&DeviceInfoData);

    if (!res1) {
      SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	  break;
    }

    res1=OpenDevNodeKey(DeviceInfoData.DevInst,KEY_QUERY_VALUE,0,
                        RegDisposition_OpenExisting,&KeyDevice,CM_REGISTRY_HARDWARE);
    if (res1 != ERROR_SUCCESS)
      break;
    len=MAX_NAME_PORTS;

    res1=RegQueryValueEx(
            KeyDevice,      // handle of key to query
            _T("portname"), // address of name of value to query
            NULL,           // reserved
            NULL,           // address of buffer for value type
            (BYTE*)DevName, // address of data buffer
            &len            // address of data buffer size
            );

    RegCloseKey(KeyDevice);
    if (res1 != ERROR_SUCCESS)
      break;
    Enum_numDev++;
    if (_tcsnicmp(DevName, _T("com"), 3) != 0)
      continue;
    numport=(int)_tcstol(DevName+3, NULL, 10);
    if (numport > 0) {
      PortName=DevName;
      result=true;
    } /* if */
  } /* while */

  FreeLibrary(CfgMan);
  OpenDevNodeKey=NULL;
  return result;
}

bool EndEnumeratePorts(HPORT DeviceInfoSet)
{
  Enum_numDev = 0;
  return SetupDiDestroyDeviceInfoList(DeviceInfoSet) != 0;
}

#else // WIN32

#include <sys/ioctl.h>
#include <linux/serial.h>

HPORT BeginEnumeratePorts(void)
{
  /* assume LINUX, FreeBSD, OpenBSD, or some other variant */
  DIR *dir = opendir("/dev/");
  if (dir == NULL)
    return (HPORT)-1;
  return dir;
}

bool EnumeratePortsNext(HPORT DeviceInfoSet, wxString &PortName)
{
  bool ok = false;
  struct dirent *entry;
  while ((entry=readdir(DeviceInfoSet)) != NULL && !ok) {
    if (strncmp(entry->d_name, "ttyS", 4) == 0 || strncmp(entry->d_name, "ttyUSB", 6) == 0) {
      /* verify whether this port really works */
      struct serial_struct serinfo;
      memset(&serinfo, 0, sizeof (serinfo));
      char path[64];
      sprintf(path, "/dev/%s", entry->d_name);
      int fd = open(path, O_RDWR | O_NONBLOCK | O_NOCTTY);
      if (fd >= 0) {
        if (ioctl(fd, TIOCGSERIAL, &serinfo) >= 0 && serinfo.type != 0 && serinfo.baud_base != 0) {
          PortName = wxString::FromUTF8(entry->d_name);
          ok = true;
        }
        close(fd);
      }
    }
  }
  return ok;
}

bool EndEnumeratePorts(HPORT DeviceInfoSet)
{
  closedir(DeviceInfoSet);
  return true;
}

#endif // WIN32


wxArrayString EnumeratePortsList(void)
{
  wxArrayString list;
  HPORT hp = BeginEnumeratePorts();
  if ((long)hp != -1) {
    wxString name;
    while (EnumeratePortsNext(hp, name))
      list.Add(name);
    EndEnumeratePorts(hp);
  } /* if */
  return list;
}

/* List valid COM ports
 *
 * Partial copyright ITB CompuPhase, 2006-2016
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
 * Version: $Id: portscan.h 5504 2016-05-15 13:42:30Z  $
 */
#ifndef _PORTSCAN_H
#define _PORTSCAN_H

#include <wx/wx.h>
#include <wx/arrstr.h>

#if defined __WIN32__ || defined _WIN32 || defined WIN32
  typedef HANDLE HPORT;
#else
  #include <dirent.h>
  typedef DIR* HPORT;
#endif

HPORT BeginEnumeratePorts(void);
bool  EnumeratePortsNext(HPORT DeviceInfoSet, wxString &PortName);
bool  EndEnumeratePorts(HPORT DeviceInfoSet);

wxArrayString EnumeratePortsList(void);

#endif /* _PORTSCAN_H */

/* rs232 - Multi-platform RS232 support
 *
 * Copyright 2006-2014 CompuPhase
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
 * $Id: rs232.c 5091 2014-06-26 09:58:55Z  $
 */
#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <errno.h>
#include <string.h>
#if defined __WIN32__ || defined _WIN32 || defined WIN32
    #include <windows.h>
    #include <tchar.h>
	#if !defined _WIN32
		#define _WIN32	1
	#endif
#else
    #include <stdio.h>
    #include <fcntl.h>
    #include <termios.h>
    #include <unistd.h>
#endif
#include "rs232.h"


#if !defined sizearray
    #define sizearray(a)    (sizeof(a) / sizeof((a)[0]))
#endif

#if defined _WIN32
    static HANDLE hCom = INVALID_HANDLE_VALUE;
#else /* _WIN32 */
    static int fdCom = -1;
    static struct termios oldtio;
#endif /* _WIN32 */


int rs232_open(const TCHAR *port, unsigned BaudRate)
{
    #if defined _WIN32
        DCB dcb;
        COMMTIMEOUTS commtimeouts;

        /* set up the connection */
        hCom=CreateFile(port,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if (hCom==INVALID_HANDLE_VALUE && _tcslen(port)<10) {
            /* try with prefix */
            TCHAR buffer[40]=_T("\\\\.\\");
            _tcscat(buffer,port);
            hCom=CreateFile(buffer,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        } /* if */
        if (hCom==INVALID_HANDLE_VALUE)
            return ENXIO;

        GetCommState(hCom,&dcb);
        /* first set the baud rate only, because this may fail for a non-standard
         * baud rate
         */
        dcb.BaudRate=BaudRate;
        if (!SetCommState(hCom,&dcb) || dcb.BaudRate!=BaudRate) {
            /* find the lowest standard baud rate */
            static const unsigned stdbaud[] = {1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200, 230400 };
            int i;
            for (i=0; (i+1)<(int)sizearray(stdbaud) && stdbaud[i]<BaudRate; i++)
                /* nothing */;
            dcb.BaudRate=stdbaud[i];
        } /* if */
        dcb.ByteSize=8;
        dcb.StopBits=ONESTOPBIT;
        dcb.Parity=NOPARITY;
        dcb.fDtrControl=DTR_CONTROL_DISABLE;
        dcb.fOutX=FALSE;
        dcb.fInX=FALSE;
        dcb.fNull=FALSE;
        dcb.fRtsControl=RTS_CONTROL_DISABLE;
        SetCommState(hCom,&dcb);
        SetCommMask(hCom,EV_RXCHAR|EV_TXEMPTY);

        commtimeouts.ReadIntervalTimeout        =0x7fffffff;
        commtimeouts.ReadTotalTimeoutMultiplier =0;
        commtimeouts.ReadTotalTimeoutConstant   =1;
        commtimeouts.WriteTotalTimeoutMultiplier=0;
        commtimeouts.WriteTotalTimeoutConstant  =0;
        SetCommTimeouts(hCom,&commtimeouts);
    #else /* _WIN32 */
        struct termios newtio;
        char portdev[60];

        /* open the serial port device file
         * O_NDELAY   - tells port to operate and ignore the DCD line
         * O_NONBLOCK - same as O_NDELAY under Linux
         * O_NOCTTY   - this process is not to become the controlling
         *              process for the port. The driver will not send
         *              this process signals due to keyboard aborts, etc.
         */
        fdCom = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
        if (fdCom < 0) {
          sprintf(portdev, "/dev/%s", port);
          fdCom = open(portdev, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
          if (fdCom < 0)
            return errno;
        } /* if */

        tcgetattr(fdCom, &oldtio); /* save current port settings */
        memset(&newtio, 0, sizeof newtio);

        /* B9600  - 9600 bps (may also be B57600)
         * CS8    - 8 data bits, 1 stop bit, no parity
         * CREAD  - receiver enabled
         * CLOCAL - ignore modem control lines
         */
        newtio.c_cflag = CS8 | CLOCAL | CREAD;
        #define NEWTERMIOS_SETBAUDARTE(bps) newtio.c_cflag |= bps;
        switch (BaudRate) {
        #ifdef B1152000
            case 1152000: NEWTERMIOS_SETBAUDARTE( B1152000 ); break;
        #endif // B1152000
        #ifdef B576000
            case  576000: NEWTERMIOS_SETBAUDARTE( B576000 ); break;
        #endif // B576000
        #ifdef B230400
            case  230400: NEWTERMIOS_SETBAUDARTE( B230400 ); break;
        #endif // B230400
        #ifdef B115200
            case  115200: NEWTERMIOS_SETBAUDARTE( B115200 ); break;
        #endif // B115200
        #ifdef B57600
            case   57600: NEWTERMIOS_SETBAUDARTE( B57600 ); break;
        #endif // B57600
        #ifdef B38400
            case   38400: NEWTERMIOS_SETBAUDARTE( B38400 ); break;
        #endif // B38400
        #ifdef B19200
            case   19200: NEWTERMIOS_SETBAUDARTE( B19200 ); break;
        #endif // B19200
        #ifdef B9600
            case    9600: NEWTERMIOS_SETBAUDARTE( B9600 ); break;
        #endif // B9600
        default:
            rs232_close();
            return EPROTONOSUPPORT;
        } /* switch */

        newtio.c_iflag = IGNPAR | IGNBRK; /* ignore parity and BREAK conditions */
        newtio.c_oflag = 0; /* set output mode (non-canonical, no processing,...) */
        newtio.c_lflag = 0; /* set input mode (non-canonical, no echo,...) */

        /* with VMIN==0 && VTIME==0, read() will always return immediately; if no
         * data is available it will return with no characters read
         */
        newtio.c_cc[VTIME]=0; /* inter-character timer used (increments of 0.1 second) */
        newtio.c_cc[VMIN] =0; /* blocking read until 0 chars received */

        tcflush(fdCom, TCIFLUSH);
        if (tcsetattr(fdCom, TCSANOW, &newtio))
            return EPROTONOSUPPORT;

        /* Set up for no delay, ie non-blocking reads will occur. When we read, we'll
         * get what's in the input buffer or nothing
         */
        fcntl(fdCom, F_SETFL,FNDELAY);
    #endif /* _WIN32 */

    return 0;
}

void rs232_close(void)
{
    #if defined _WIN32
        if (hCom != INVALID_HANDLE_VALUE) {
            FlushFileBuffers(hCom);
            CloseHandle(hCom);
            hCom = INVALID_HANDLE_VALUE;
        } /* if */
    #else /* _WIN32 */
        if (fdCom >= 0) {
            tcflush(fdCom, TCOFLUSH);
            tcflush(fdCom, TCIFLUSH);
            tcsetattr(fdCom, TCSANOW, &oldtio);
            close(fdCom);
            fdCom = -1;
        } /* if */
    #endif /* _WIN32 */
}

int rs232_isopen(void)
{
    #if defined _WIN32
        return hCom != INVALID_HANDLE_VALUE;
    #else /* _WIN32 */
        return fdCom >= 0;
    #endif /* _WIN32 */
}

int rs232_send(const unsigned char *buffer, int size)
{
    #if defined _WIN32
        DWORD written = 0;
        if (hCom != INVALID_HANDLE_VALUE) {
            if (!WriteFile(hCom, buffer, size, &written, NULL))
                written = 0;
        } /* if */
        return (int)written;
    #else /* _WIN32 */
        return (fdCom>=0) ? (int)write(fdCom, buffer, size) : 0;
    #endif /* _WIN32 */
}

int rs232_recv(unsigned char *buffer, int size)
{
    #if defined _WIN32
        DWORD read = 0;
        if (hCom != INVALID_HANDLE_VALUE) {
          if (!ReadFile(hCom, buffer, size, &read, NULL))
              read = -1;
        }
        return (int)read;
    #else /* _WIN32 */
        if (fdCom >= 0)
            return (int)read(fdCom, buffer, size);
        return 0;
    #endif /* _WIN32 */
}

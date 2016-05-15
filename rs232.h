/* rs232 - Multi-platform RS232 support
 *
 * Copyright 2006-2016 CompuPhase
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
 * $Id: rs232.h 5504 2016-05-15 13:42:30Z  $
 */
#ifndef _RS232_H
#define _RS232_H

#if defined __cplusplus
	extern "C" {
#endif

int  rs232_open(const TCHAR *port, unsigned BaudRate);
void rs232_close(void);
int  rs232_send(const unsigned char *buffer, int size);
int  rs232_recv(unsigned char *buffer, int size);
int  rs232_isopen(void);

#if defined __cplusplus
	}
#endif

#endif /* _RS232_H */

/*
 *    file:             socket_com_server.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _SOCKET_COM_SERVER_H_
#define _SOCKET_COM_SERVER_H_

#include <stdint.h>
#include "com_types.h"
Com * __new__SocketComServer( uint16_t port );
Com * __del__SocketComServer( Com *pSelf );

#endif /* _SOCKET_COM_SERVER_H_ */

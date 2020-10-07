/*
 *    file:             socket_com_client.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _SOCKET_COM_CLIENT_H_
#define _SOCKET_COM_CLIENT_H_

#include <stdint.h>
#include "com_types.h"
Com * __new__SocketComClient( const char *pAddress, uint16_t port );
Com * __del__SocketComClient( Com *pSelf );

#endif /* _SOCKET_COM_CLIENT_H_ */

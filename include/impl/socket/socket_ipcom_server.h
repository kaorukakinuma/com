/*
 *    file:             socket_ipcom_server.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _SOCKET_IPCOM_SERVER_H_
#define _SOCKET_IPCOM_SERVER_H_

#include <stdint.h>
#include "ipcom_types.h"
Ipcom * __new__SocketIpcomServer( uint16_t port );
Ipcom * __del__SocketIpcomServer( Ipcom *pSelf );

#endif /* _SOCKET_IPCOM_SERVER_H_ */

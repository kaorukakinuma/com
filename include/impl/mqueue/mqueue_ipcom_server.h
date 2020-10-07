/*
 *    file:             mqueue_ipcom_server.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _MQUEUE_IPCOM_SERVER_H_
#define _MQUEUE_IPCOM_SERVER_H_

#include "ipcom_types.h"
Ipcom * __new__MqueueIpcomServer( const char *pName );
Ipcom * __del__MqueueIpcomServer( Ipcom *pSelf );

#endif /* _MQUEUE_IPCOM_SERVER_H_ */

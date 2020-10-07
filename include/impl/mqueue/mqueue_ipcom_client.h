/*
 *    file:             mqueue_ipcom_client.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _MQUEUE_IPCOM_CLIENT_H_
#define _MQUEUE_IPCOM_CLIENT_H_

#include "ipcom_types.h"
Ipcom * __new__MqueueIpcomClient( const char *pName );
Ipcom * __del__MqueueIpcomClient( Ipcom *pSelf );

#endif /* _MQUEUE_IPCOM_CLIENT_H_ */

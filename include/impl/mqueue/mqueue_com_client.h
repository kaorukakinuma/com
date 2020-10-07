/*
 *    file:             mqueue_com_client.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _MQUEUE_COM_CLIENT_H_
#define _MQUEUE_COM_CLIENT_H_

#include "com_types.h"
Com * __new__MqueueComClient( const char *pName );
Com * __del__MqueueComClient( Com *pSelf );

#endif /* _MQUEUE_COM_CLIENT_H_ */

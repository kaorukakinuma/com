/*
 *    file:             mqueue_com_server.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _MQUEUE_COM_SERVER_H_
#define _MQUEUE_COM_SERVER_H_

#include "com_types.h"
Com * __new__MqueueComServer( const char *pName );
Com * __del__MqueueComServer( Com *pSelf );

#endif /* _MQUEUE_COM_SERVER_H_ */

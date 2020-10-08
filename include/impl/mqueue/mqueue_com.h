/*
 *    file:             mqueue_com.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _MQUEUE_COM_H_
#define _MQUEUE_COM_H_

#include "com_types.h"
Com * __new__MqueueCom( const char *pName );
Com * __del__MqueueCom( Com *pSelf );

#endif /* _MQUEUE_COM_H_ */

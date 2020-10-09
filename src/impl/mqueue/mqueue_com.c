/*
 * MIT License
 * 
 * Copyright (c) 2020 Kaoru Kakinuma
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#include "mqueue_com.h"
#include "com.h"

#define CHECK_NULL( p )\
    if ( p == NULL ) return COM_E_OBJ;

#define CHECK_MQD( pSelf )\
    if ( pSelf->mqueue == -1 ) return COM_E_OBJ;

typedef struct {
    Com         base;
    const char *pName;
    mqd_t       mqueue;
    size_t      msgsize;
    char       *pBuffer;
} MqueueCom;

/* ------------------------------------------------------------------------- */

static ComErcd Open( Com *pSuper )
{
    MqueueCom *pSelf = (MqueueCom *)pSuper;
    CHECK_NULL( pSelf );

    mqd_t mqueue = mq_open(
        pSelf->pName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL );
    if ( mqueue < 0 ) {
        perror( "mq_open" );
        return COM_E_SYS;
    }

    struct mq_attr attr;
    int ret = mq_getattr( mqueue, &attr );
    if ( ret < 0 ) {
        perror( "mq_getattr" );
        return COM_E_SYS;
    }

    char *pBuffer = (char *)malloc( attr.mq_msgsize );
    if ( pBuffer == NULL ) {
        perror( "malloc" );
        return COM_E_SYS;
    }

    pSelf->mqueue  = mqueue;
    pSelf->msgsize = attr.mq_msgsize;
    pSelf->pBuffer = pBuffer;

    return COM_E_OK;
}

static ComErcd Close( Com *pSuper )
{
    MqueueCom *pSelf = (MqueueCom *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_MQD( pSelf );

    int ret = mq_close( pSelf->mqueue );
    if ( ret < 0 ) {
        perror( "mq_close" );
        return COM_E_SYS;
    }

    pSelf->mqueue = -1;
    free( pSelf->pBuffer );

    return COM_E_OK;
}

static ComErcd Read( Com *pSuper, char *pBuffer, size_t length )
{
    MqueueCom *pSelf = (MqueueCom *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_MQD( pSelf );

    if ( length > pSelf->msgsize ) {
        printf( "Read length is too long.\n" );
        return COM_E_OBJ;
    }

    ssize_t recvlen = mq_receive(
        pSelf->mqueue, pSelf->pBuffer, pSelf->msgsize, NULL );
    if ( recvlen < 0 ) {
        perror( "mq_receive" );
        return COM_E_SYS;
    }
    memcpy( pBuffer, pSelf->pBuffer, length );

    return COM_E_OK;
}

static ComErcd Write( Com *pSuper, const char *pBuffer, size_t length )
{
    MqueueCom *pSelf = (MqueueCom *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_MQD( pSelf );

    if ( length > pSelf->msgsize ) {
        printf( "Write length is too long.\n" );
        return COM_E_OBJ;
    }

    int ret = mq_send( pSelf->mqueue, pBuffer, length, 0 );
    if ( ret < 0 ) {
        perror( "mq_send" );
        return COM_E_SYS;
    }

    return COM_E_OK;
}

static const Com sBase = {
    .Open  = Open,
    .Close = Close,
    .Read  = Read,
    .Write = Write,
};

/* ------------------------------------------------------------------------- */

Com * __new__MqueueCom( const char *pName )
{
    MqueueCom *pSelf = malloc( sizeof(MqueueCom) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base   = sBase;
    pSelf->pName  = pName;
    pSelf->mqueue = -1;

    return (Com *)pSelf;
}

Com * __del__MqueueCom( Com *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    MqueueCom *pSub = (MqueueCom *)pSelf;
    mq_unlink( pSub->pName );
    free( pSub );

    return NULL;
}

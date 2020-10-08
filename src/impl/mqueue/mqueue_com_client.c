/*
 *    file:             mqueue_com_client.c
 *    creation date:    2020-10-07
 *    last update:      2020-10-08
 *    author:           kaoru kakinuma
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#include "mqueue_com_client.h"
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
} MqueueComClient;

/* ------------------------------------------------------------------------- */

static ComErcd Open( Com *pSuper )
{
    MqueueComClient *pSelf = (MqueueComClient *)pSuper;
    CHECK_NULL( pSelf );

    mqd_t mqueue = mq_open( pSelf->pName, O_RDWR );
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
    MqueueComClient *pSelf = (MqueueComClient *)pSuper;
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
    MqueueComClient *pSelf = (MqueueComClient *)pSuper;
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
    MqueueComClient *pSelf = (MqueueComClient *)pSuper;
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

Com * __new__MqueueComClient( const char *pName )
{
    MqueueComClient *pSelf = malloc( sizeof(MqueueComClient) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base   = sBase;
    pSelf->pName  = pName;
    pSelf->mqueue = -1;

    return (Com *)pSelf;
}

Com * __del__MqueueComClient( Com *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    free( pSelf );

    return NULL;
}

/*
 *    file:             mqueue_com_server.c
 *    creation date:    2020-10-07
 *    last update:      2020-10-08
 *    author:           kaoru kakinuma
 */
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/stat.h>

#include "mqueue_com_server.h"
#include "com.h"

#define CHECK_NULL( p )\
    if ( p == NULL ) return COM_E_OBJ;

#define CHECK_MQD( pSelf )\
    if ( pSelf->mqueue == -1 ) return COM_E_OBJ;

typedef struct {
    Com         base;
    const char *pName;
    mqd_t       mqueue;
} MqueueComServer;

/* ------------------------------------------------------------------------- */

static ComErcd Open( Com *pSuper )
{
    MqueueComServer *pSelf = (MqueueComServer *)pSuper;
    CHECK_NULL( pSelf );

    mqd_t mqueue = mq_open(
        pSelf->pName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL );
    if ( mqueue < 0 ) {
        perror( "mq_open" );
        return COM_E_SYS;
    }
    pSelf->mqueue = mqueue;

    return COM_E_OK;
}

static ComErcd Close( Com *pSuper )
{
    MqueueComServer *pSelf = (MqueueComServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_MQD( pSelf );

    int ret = mq_close( pSelf->mqueue );
    if ( ret < 0 ) {
        perror( "mq_close" );
        return COM_E_SYS;
    }
    pSelf->mqueue = -1;

    return COM_E_OK;
}

static ComErcd Read( Com *pSuper, char *pBuffer, int length )
{
    MqueueComServer *pSelf = (MqueueComServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_MQD( pSelf );

    ssize_t recvlen = mq_receive( pSelf->mqueue, pBuffer, length, NULL );
    if ( recvlen < 0 ) {
        perror( "mq_receive" );
        return COM_E_SYS;
    }

    return COM_E_OK;
}

static ComErcd Write( Com *pSuper, const char *pBuffer, int length )
{
    MqueueComServer *pSelf = (MqueueComServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_MQD( pSelf );

    int ret = mq_send( pSelf->mqueue, pBuffer, length, 0 );
    if ( ret < 0 ) {
        perror( "send" );
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

Com * __new__MqueueComServer( const char *pName )
{
    MqueueComServer *pSelf = malloc( sizeof(MqueueComServer) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base   = sBase;
    pSelf->pName  = pName;
    pSelf->mqueue = -1;

    return (Com *)pSelf;
}

Com * __del__MqueueComServer( Com *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    MqueueComServer *pSub = (MqueueComServer *)pSelf;
    mq_unlink( pSub->pName );
    free( pSub );

    return NULL;
}

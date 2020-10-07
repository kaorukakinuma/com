/*
 *    file:             socket_ipcom_client.c
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "socket_ipcom_client.h"
#include "ipcom.h"

#define CHECK_NULL( p )\
    if ( p == NULL ) return IPCOM_E_OBJ;

#define CHECK_SOCKFD( pSelf )\
    if ( pSelf->serverSockfd < 0 ) return IPCOM_E_OBJ;

typedef struct {
    Ipcom         base;
    const char   *pAddress;
    uint16_t      port;
    int           serverSockfd;
} SocketIpcomClient;

/* ------------------------------------------------------------------------- */

static IpcomErcd Open( Ipcom *pSuper )
{
    SocketIpcomClient *pSelf = (SocketIpcomClient *)pSuper;
    CHECK_NULL( pSelf );
    int ret;

    /* set server address and port */
    struct sockaddr_in serverSockaddr = (struct sockaddr_in) { 0 };
    serverSockaddr.sin_family = AF_INET;
    ret = inet_aton( pSelf->pAddress, &serverSockaddr.sin_addr );
    if ( ret == 0 ) {
        perror( "inet_aton" );
        return IPCOM_E_SYS;
    }
    serverSockaddr.sin_port = htons( pSelf->port );

    /* create socket */
    int serverSockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( serverSockfd < 0 ) {
        perror( "socket" );
        return IPCOM_E_SYS;
    }

    /* connect to server */
    ret = connect( serverSockfd,
        (struct sockaddr *)&serverSockaddr, sizeof(serverSockaddr) );
    if ( ret < 0 ) {
        perror( "connect" );
        close( serverSockfd );
        return IPCOM_E_SYS;
    }

    pSelf->serverSockfd = serverSockfd;

    return IPCOM_E_OK;
}

static IpcomErcd Close( Ipcom *pSuper )
{
    SocketIpcomClient *pSelf = (SocketIpcomClient *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_SOCKFD( pSelf );

    close( pSelf->serverSockfd );
    pSelf->serverSockfd = -1;

    return IPCOM_E_OK;
}

static IpcomErcd Read( Ipcom *pSuper, char *pBuffer, int length )
{
    SocketIpcomClient *pSelf = (SocketIpcomClient *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    int totallen = 0;
    while ( totallen < length ) {

        ssize_t recvlen = recv(
            pSelf->serverSockfd, pBuffer+totallen, length-totallen, 0 );
        if ( recvlen <= 0 ) {
            perror( "recv" );
            return IPCOM_E_SYS;
        }
        totallen += (int)recvlen;
    }

    return IPCOM_E_OK;
}

static IpcomErcd Write( Ipcom *pSuper, const char *pBuffer, int length )
{
    SocketIpcomClient *pSelf = (SocketIpcomClient *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    int totallen = 0;
    while ( totallen < length ) {

        ssize_t sendlen = send(
            pSelf->serverSockfd, pBuffer+totallen, length-totallen, 0 );
        if ( sendlen <= 0 ) {
            perror( "send" );
            return IPCOM_E_SYS;
        }
        totallen += (int)sendlen;
    }

    return IPCOM_E_OK;
}

static const Ipcom sBase = {
    .Open  = Open,
    .Close = Close,
    .Read  = Read,
    .Write = Write,
};

/* ------------------------------------------------------------------------- */

Ipcom * __new__SocketIpcomClient( const char *pAddress, uint16_t port )
{
    SocketIpcomClient *pSelf = malloc( sizeof(SocketIpcomClient) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base         = sBase;
    pSelf->pAddress     = pAddress;
    pSelf->port         = port;
    pSelf->serverSockfd = -1;

    return (Ipcom *)pSelf;
}

Ipcom * __del__SocketIpcomClient( Ipcom *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    free( pSelf );

    return NULL;
}

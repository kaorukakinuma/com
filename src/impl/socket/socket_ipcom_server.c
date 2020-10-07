/*
 *    file:             socket_ipcom_server.c
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "socket_ipcom_server.h"
#include "ipcom.h"

#define CHECK_NULL( p )\
    if ( p == NULL ) return IPCOM_E_OBJ;

#define CHECK_SOCKFD( pSelf )\
    if ( pSelf->clientSockfd < 0 ) return IPCOM_E_OBJ;

static const int QUEUEMAX = 5;

typedef struct {
    Ipcom       base;
    uint16_t    port;
    int         serverSockfd;
    int         clientSockfd;
} SocketIpcomServer;

/* ------------------------------------------------------------------------- */

static IpcomErcd Open( Ipcom *pSuper )
{
    SocketIpcomServer *pSelf = (SocketIpcomServer *)pSuper;
    CHECK_NULL( pSelf );
    int ret;

    /* create socket */
    int serverSockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( serverSockfd < 0 ) {
        perror( "socket" );
        return IPCOM_E_SYS;
    }

    /* add an attribute of address-reuse */
    int opt = 1;
    ret = setsockopt( serverSockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if ( ret < 0 ) {
        perror( "setsockopt" );
        close( serverSockfd );
        return IPCOM_E_SYS;
    }

    /* bind address and port to the socket */
    struct sockaddr_in serverSockaddr = (struct sockaddr_in) { 0 };
    serverSockaddr.sin_family      = AF_INET;
    serverSockaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    serverSockaddr.sin_port        = htons( pSelf->port );
    ret = bind( serverSockfd,
        (struct sockaddr *)&serverSockaddr, sizeof(serverSockaddr) );
    if ( ret < 0 ) {
        perror( "bind" );
        close( serverSockfd );
        return IPCOM_E_SYS;
    }

    /* wait for connection from client */
    ret = listen( serverSockfd, QUEUEMAX );
    if ( ret < 0 ) {
        perror( "listen" );
        close( serverSockfd );
        return IPCOM_E_SYS;
    }

    /* get client socket descriptor */
    int clientSockfd = accept( serverSockfd, NULL, NULL );
    if ( clientSockfd < 0 ) {
        perror( "accept" );
        close( serverSockfd );
        return IPCOM_E_SYS;
    }

    pSelf->serverSockfd = serverSockfd;
    pSelf->clientSockfd = clientSockfd;

    return IPCOM_E_OK;
}

static IpcomErcd Close( Ipcom *pSuper )
{
    SocketIpcomServer *pSelf = (SocketIpcomServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_SOCKFD( pSelf );

    close( pSelf->clientSockfd );
    close( pSelf->serverSockfd );

    pSelf->serverSockfd = -1;
    pSelf->clientSockfd = -1;

    return IPCOM_E_OK;
}

static IpcomErcd Read( Ipcom *pSuper, char *pBuffer, int length )
{
    SocketIpcomServer *pSelf = (SocketIpcomServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    int totallen = 0;
    while ( totallen < length ) {

        ssize_t recvlen = recv(
            pSelf->clientSockfd, pBuffer+totallen, length-totallen, 0 );
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
    SocketIpcomServer *pSelf = (SocketIpcomServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    int totallen = 0;
    while ( totallen < length ) {

        ssize_t sendlen = send(
            pSelf->clientSockfd, pBuffer+totallen, length-totallen, 0 );
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

Ipcom * __new__SocketIpcomServer( uint16_t port )
{
    SocketIpcomServer *pSelf = malloc( sizeof(SocketIpcomServer) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base         = sBase;
    pSelf->port         = port;
    pSelf->serverSockfd = -1;
    pSelf->clientSockfd = -1;

    return (Ipcom *)pSelf;
}

Ipcom * __del__SocketIpcomServer( Ipcom *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    free( pSelf );

    return NULL;
}

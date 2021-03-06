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

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "socket_com_server.h"
#include "com.h"

#define CHECK_NULL( p )\
    if ( p == NULL ) return COM_E_OBJ;

#define CHECK_SOCKFD( pSelf )\
    if ( pSelf->clientSockfd < 0 ) return COM_E_OBJ;

static const int QUEUEMAX = 5;

typedef struct {
    Com         base;
    uint16_t    port;
    int         serverSockfd;
    int         clientSockfd;
} SocketComServer;

/* ------------------------------------------------------------------------- */

static ComErcd Open( Com *pSuper )
{
    SocketComServer *pSelf = (SocketComServer *)pSuper;
    CHECK_NULL( pSelf );
    int ret;

    /* create socket */
    int serverSockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( serverSockfd < 0 ) {
        perror( "socket" );
        return COM_E_SYS;
    }

    /* add an attribute of address-reuse */
    int opt = 1;
    ret = setsockopt( serverSockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if ( ret < 0 ) {
        perror( "setsockopt" );
        close( serverSockfd );
        return COM_E_SYS;
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
        return COM_E_SYS;
    }

    /* wait for connection from client */
    ret = listen( serverSockfd, QUEUEMAX );
    if ( ret < 0 ) {
        perror( "listen" );
        close( serverSockfd );
        return COM_E_SYS;
    }

    /* get client socket descriptor */
    int clientSockfd = accept( serverSockfd, NULL, NULL );
    if ( clientSockfd < 0 ) {
        perror( "accept" );
        close( serverSockfd );
        return COM_E_SYS;
    }

    pSelf->serverSockfd = serverSockfd;
    pSelf->clientSockfd = clientSockfd;

    return COM_E_OK;
}

static ComErcd Close( Com *pSuper )
{
    SocketComServer *pSelf = (SocketComServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_SOCKFD( pSelf );

    close( pSelf->clientSockfd );
    close( pSelf->serverSockfd );

    pSelf->serverSockfd = -1;
    pSelf->clientSockfd = -1;

    return COM_E_OK;
}

static ComErcd Read( Com *pSuper, char *pBuffer, size_t length )
{
    SocketComServer *pSelf = (SocketComServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    size_t totallen = 0;
    while ( totallen < length ) {

        ssize_t recvlen = recv(
            pSelf->clientSockfd, pBuffer+totallen, length-totallen, 0 );
        if ( recvlen <= 0 ) {
            perror( "recv" );
            return COM_E_SYS;
        }
        totallen += (size_t)recvlen;
    }

    return COM_E_OK;
}

static ComErcd Write( Com *pSuper, const char *pBuffer, size_t length )
{
    SocketComServer *pSelf = (SocketComServer *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    size_t totallen = 0;
    while ( totallen < length ) {

        ssize_t sendlen = send(
            pSelf->clientSockfd, pBuffer+totallen, length-totallen, 0 );
        if ( sendlen <= 0 ) {
            perror( "send" );
            return COM_E_SYS;
        }
        totallen += (size_t)sendlen;
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

Com * __new__SocketComServer( uint16_t port )
{
    SocketComServer *pSelf = malloc( sizeof(SocketComServer) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base         = sBase;
    pSelf->port         = port;
    pSelf->serverSockfd = -1;
    pSelf->clientSockfd = -1;

    return (Com *)pSelf;
}

Com * __del__SocketComServer( Com *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    free( pSelf );

    return NULL;
}

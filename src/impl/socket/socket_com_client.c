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
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "socket_com_client.h"
#include "com.h"

#define CHECK_NULL( p )\
    if ( p == NULL ) return COM_E_OBJ;

#define CHECK_SOCKFD( pSelf )\
    if ( pSelf->serverSockfd < 0 ) return COM_E_OBJ;

typedef struct {
    Com           base;
    const char   *pAddress;
    uint16_t      port;
    int           serverSockfd;
} SocketComClient;

/* ------------------------------------------------------------------------- */

static ComErcd Open( Com *pSuper )
{
    SocketComClient *pSelf = (SocketComClient *)pSuper;
    CHECK_NULL( pSelf );
    int ret;

    /* set server address and port */
    struct sockaddr_in serverSockaddr = (struct sockaddr_in) { 0 };
    serverSockaddr.sin_family = AF_INET;
    ret = inet_aton( pSelf->pAddress, &serverSockaddr.sin_addr );
    if ( ret == 0 ) {
        perror( "inet_aton" );
        return COM_E_SYS;
    }
    serverSockaddr.sin_port = htons( pSelf->port );

    /* create socket */
    int serverSockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( serverSockfd < 0 ) {
        perror( "socket" );
        return COM_E_SYS;
    }

    /* connect to server */
    ret = connect( serverSockfd,
        (struct sockaddr *)&serverSockaddr, sizeof(serverSockaddr) );
    if ( ret < 0 ) {
        perror( "connect" );
        close( serverSockfd );
        return COM_E_SYS;
    }

    pSelf->serverSockfd = serverSockfd;

    return COM_E_OK;
}

static ComErcd Close( Com *pSuper )
{
    SocketComClient *pSelf = (SocketComClient *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_SOCKFD( pSelf );

    close( pSelf->serverSockfd );
    pSelf->serverSockfd = -1;

    return COM_E_OK;
}

static ComErcd Read( Com *pSuper, char *pBuffer, size_t length )
{
    SocketComClient *pSelf = (SocketComClient *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    size_t totallen = 0;
    while ( totallen < length ) {

        ssize_t recvlen = recv(
            pSelf->serverSockfd, pBuffer+totallen, length-totallen, 0 );
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
    SocketComClient *pSelf = (SocketComClient *)pSuper;
    CHECK_NULL( pSelf );
    CHECK_NULL( pBuffer );
    CHECK_SOCKFD( pSelf );

    size_t totallen = 0;
    while ( totallen < length ) {

        ssize_t sendlen = send(
            pSelf->serverSockfd, pBuffer+totallen, length-totallen, 0 );
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

Com * __new__SocketComClient( const char *pAddress, uint16_t port )
{
    SocketComClient *pSelf = malloc( sizeof(SocketComClient) );
    if ( pSelf == NULL ) {
        perror( "malloc" );
        return NULL;
    }

    pSelf->base         = sBase;
    pSelf->pAddress     = pAddress;
    pSelf->port         = port;
    pSelf->serverSockfd = -1;

    return (Com *)pSelf;
}

Com * __del__SocketComClient( Com *pSelf )
{
    if ( pSelf == NULL ) {
        return NULL;
    }
    pSelf->Close( pSelf );

    free( pSelf );

    return NULL;
}

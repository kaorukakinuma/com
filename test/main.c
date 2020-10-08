#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "mqueue_com_server.h"
#include "mqueue_com_client.h"
#include "com.h"

static const char * const NAME = "/test";

static void * client( void *pArg )
{
    (void)pArg;
    Com *pCom = __new__MqueueComClient( NAME );
    pCom->Open( pCom );

    char buf = 1;
    pCom->Write( pCom, &buf, 1 );

    pCom->Close( pCom );
    pCom = __del__MqueueComClient( pCom );
    pthread_exit( NULL );
}

int main( void )
{
    Com *pCom = __new__MqueueComServer( NAME );
    pCom->Open( pCom );

    pthread_t thread;
    pthread_create( &thread, NULL, client, NULL );
    pthread_detach( thread );

    while ( 1 ) {
        char buf = 0;
        pCom->Read( pCom, &buf, 1 );
        if ( buf > 0 ) {
            printf( "Communication success.\n" );
            break;
        }
        sleep( 1 );
    }

    pCom->Close( pCom );
    pCom = __del__MqueueComServer( pCom );
    return 0;
}

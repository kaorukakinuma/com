#include "socket_com_server.h"
#include "com.h"

static const int PORT = 8888;

int main( void )
{
    Com *pCom = __new__SocketComServer( PORT );
    pCom->Open( pCom );
    pCom->Close( pCom );
    pCom = __del__SocketComServer( pCom );
    return 0;
}

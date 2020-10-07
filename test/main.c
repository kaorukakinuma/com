#include "socket_ipcom_server.h"
#include "ipcom.h"

static const int PORT = 8888;

int main( void )
{
    Ipcom *pCom;
    pCom = __new__SocketIpcomServer( PORT );
    pCom->Open( pCom );
    pCom->Close( pCom );
    pCom = __del__SocketIpcomServer( pCom );
    return 0;
}

#ifndef _DEBUGCLIENT_H_
#define _DEBUGCLIENT_H_

#define WAIT_KEY_COMMAND "**WAITKEY**" 

#include <xbsocket.h>
#include <xbsockaddr.h>
#include <XBNet.h>


class CDebugClient
{
public:
    CDebugClient();

	int  Init( char *ipaddr, unsigned int port, int blocking );
	int  Cleanup( );
	int  ChangeBlocking( int block );
	int  Recv( void *buffer, int bytes );
	int  Send( void *buffer, int bytes );
	char WaitKey( );

    CXBSocket    m_debugClientSock;  

};


#endif //_DEBUGCLIENT_H_
#include <XBApp.h>
#include <xbsocket.h>
#include <xbsockaddr.h>
#include <XBNet.h>

#include "debugclient.h" 


CDebugClient::CDebugClient()
{
}

int CDebugClient::Init( char *ipaddr, unsigned int port, int blocking )
{
	DWORD iResult ;
	DWORD dwNonBlocking ;

    HRESULT hr = XBNet_Init( XNET_STARTUP_BYPASS_SECURITY );
    if( FAILED(hr) )
    {
		return -1 ;
    }

	m_debugClientSock.Open( CXBSocket::Type_TCP ) ;

    CXBSockAddr cAddr( inet_addr( ipaddr ), port );

	if ( !m_debugClientSock.IsOpen() )
	{
		return -2 ;
	}

	if ( m_debugClientSock.Connect( cAddr.GetPtr() ) == SOCKET_ERROR )
	{
		return -3 ;
	}

	if ( blocking==0 )
	{
		dwNonBlocking = 1 ;
		iResult = m_debugClientSock.IoCtlSocket( FIONBIO, &dwNonBlocking );

		if ( iResult != 0 )
			return -4 ;
	}
	
	return 1 ;
}

int CDebugClient::Cleanup( )
{
	m_debugClientSock.Close() ;

	return ( XBNet_Cleanup() == S_OK );
}

int CDebugClient::ChangeBlocking( int block )
{
	DWORD iResult ;
	DWORD dwNonBlocking = ( block == 0 ? 1 : 0 ) ;

	iResult = m_debugClientSock.IoCtlSocket( FIONBIO, &dwNonBlocking );

	return ( iResult != 0 ) ? 0 : 1 ;
}

int CDebugClient::Recv( void *buffer, int bytes )
{
	return m_debugClientSock.Recv( buffer, bytes ) ;
}
int CDebugClient::Send( void *buffer, int bytes )
{
	return m_debugClientSock.Send( buffer, bytes ) ;
}

char CDebugClient::WaitKey( )
{
	char ch ;

	Send( WAIT_KEY_COMMAND, strlen( WAIT_KEY_COMMAND ) ) ;
	Recv( &ch, 1 ) ;
	return ch ;
}

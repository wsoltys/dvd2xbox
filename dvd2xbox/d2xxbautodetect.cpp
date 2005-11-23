#include "d2xxbautodetect.h"


int D2Xxbautodetect::udp_server_socket;
int D2Xxbautodetect::inited = 0;

D2Xxbautodetect::D2Xxbautodetect()
{
}

D2Xxbautodetect::~D2Xxbautodetect()
{
}


void D2Xxbautodetect::OnStartup()
{
	if(!g_d2xSettings.ftpd_enabled || !g_d2xSettings.network_enabled || inited==1)
	{
		StopThread(); 
		return;
	}

	iUDPPort = 4905;	
	life = 0;

	strSendMessage = "ping\0";
	strReceiveMessage = "ping";
	timeval timeout={0,500};

	cliLen = sizeof( cliAddr);
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);

	if( !inited ) 
    {
		int tUDPsocket  = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		char value      = 1;
		setsockopt( tUDPsocket, SOL_SOCKET, SO_BROADCAST, &value, value );
		struct sockaddr_in addr;
		memset(&(addr),0,sizeof(addr));
		addr.sin_family       = AF_INET; 
		addr.sin_addr.s_addr  = INADDR_ANY;
		addr.sin_port         = htons(iUDPPort);
		bind(tUDPsocket,(struct sockaddr *)(&addr),sizeof(addr));
		udp_server_socket = tUDPsocket;
		inited = 1;
    }
}

void D2Xxbautodetect::Process()
{

	while(!m_bStop)
	{
		FD_ZERO(&readfds);
		FD_SET(udp_server_socket, &readfds);
		life = select( 0,&readfds, NULL, NULL, &timeout );
		while( life )
		{
			recvfrom(udp_server_socket, sztmp, 512, 0,(struct sockaddr *) &cliAddr, &cliLen); 
			strWorkTemp = sztmp;
			if( strWorkTemp == strReceiveMessage )
			{
				strWorkTemp.Format("%s;%s;%s;%d;%d\r\n\0","dvd2xbox",g_d2xSettings.ftpduser,g_d2xSettings.ftpd_pwd,21,0 );
				sendto(udp_server_socket,(char *)strWorkTemp.c_str(),strlen((char *)strWorkTemp.c_str())+1,0,(struct sockaddr *)(&cliAddr),sizeof(cliAddr));
				strWorkTemp.Format("%d.%d.%d.%d",cliAddr.sin_addr.S_un.S_un_b.s_b1,cliAddr.sin_addr.S_un.S_un_b.s_b2,cliAddr.sin_addr.S_un.S_un_b.s_b3,cliAddr.sin_addr.S_un.S_un_b.s_b4 );
	 
			}
			timeout.tv_sec=0;
			timeout.tv_usec = 5000;
			FD_ZERO(&readfds);
			FD_SET(udp_server_socket, &readfds);
			life = select( 0,&readfds, NULL, NULL, &timeout );
		}
		Sleep(1000);
	}
}

void D2Xxbautodetect::OnExit()
{
	inited = 0;
}

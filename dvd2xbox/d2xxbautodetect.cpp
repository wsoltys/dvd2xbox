#include "d2xxbautodetect.h"

static CRITICAL_SECTION m_criticalSection;
int D2Xxbautodetect::udp_server_socket;
int D2Xxbautodetect::inited = 0;
DWORD D2Xxbautodetect::pingTimer = 0;

static map<CStdString,CStdString>	m_ftpservers;

D2Xxbautodetect::D2Xxbautodetect()
{
	InitializeCriticalSection(&m_criticalSection);
}

D2Xxbautodetect::~D2Xxbautodetect()
{
}

// Xbox Autodetection taken from XBMC

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
	m_ftpservers.clear();
}

void D2Xxbautodetect::Process()
{

	while(!m_bStop)
	{
		if((timeGetTime() - pingTimer >= (DWORD)30000) || pingTimer == 0)
		{
			// Send a ping every 30 seconds
			pingTimer = timeGetTime();
			FD_ZERO(&readfds);
			FD_SET(udp_server_socket, &readfds);
			life = select( 0,&readfds, NULL, NULL, &timeout ); 
			if (life != -1 )
			{
				memset(&(server),0,sizeof(server));
				server.sin_family           = AF_INET;
				server.sin_addr.S_un.S_addr = INADDR_BROADCAST;
				server.sin_port             = htons(iUDPPort);	
				DebugOut("Sending Ping\n");
				sendto(udp_server_socket,(char *)strSendMessage.c_str(),5,0,(struct sockaddr *)(&server),sizeof(server));
			}
		}
		FD_ZERO(&readfds);
		FD_SET(udp_server_socket, &readfds);
		life = select( 0,&readfds, NULL, NULL, &timeout );
		while( life )
		{
			recvfrom(udp_server_socket, sztmp, 512, 0,(struct sockaddr *) &cliAddr, &cliLen); 
			strWorkTemp = sztmp;
			if( strWorkTemp == strReceiveMessage )
			{
				if(g_d2xSettings.autodetect_send_pwd)
				{
					strUser = g_d2xSettings.ftpduser;
					strPWD  = g_d2xSettings.ftpd_pwd;
				}
				else
				{
					strUser = "anonymous";
					strPWD  = "anonymous";
				}
				//strNick = "dvd2xbox@"+CStdString(g_d2xSettings.localIP);
				strWorkTemp.Format("%s;%s;%s;%d;%d\r\n\0",g_d2xSettings.strAutoDetectNick,strUser,strPWD,21,0 );
				DebugOut("Ping received, sending %s",strWorkTemp.c_str());
				sendto(udp_server_socket,(char *)strWorkTemp.c_str(),strlen((char *)strWorkTemp.c_str())+1,0,(struct sockaddr *)(&cliAddr),sizeof(cliAddr));
			}
			else 
			{
				sprintf( szTemp, "%d.%d.%d.%d", cliAddr.sin_addr.S_un.S_un_b.s_b1,cliAddr.sin_addr.S_un.S_un_b.s_b2,cliAddr.sin_addr.S_un.S_un_b.s_b3,cliAddr.sin_addr.S_un.S_un_b.s_b4 );
				DebugOut("Receiving server info: %s %s",szTemp,strWorkTemp.c_str());

				CStdString strFTPPath, strNickName, strFtpUserName, strFtpPassword, strFtpPort, strBoosMode;
				CStdStringArray arSplit; 
				StringUtils::SplitString(strWorkTemp,";", arSplit);
				if ((int)arSplit.size() > 1)
				{
					strNickName     = arSplit[0].c_str();
					strFtpUserName  = arSplit[1].c_str();
					strFtpPassword  = arSplit[2].c_str();
					strFtpPort      = arSplit[3].c_str();
					strBoosMode     = arSplit[4].c_str();
					strFTPPath.Format("%s:%s:%s:%s",strFtpUserName.c_str(),strFtpPassword.c_str(),szTemp,strFtpPort.c_str());

					if(D2Xutils::getMapValue(m_ftpservers,strNickName) == NULL)
					{
						EnterCriticalSection(&m_criticalSection);
						m_ftpservers.insert(pair<CStdString,CStdString>(strNickName,strFTPPath));
						LeaveCriticalSection(&m_criticalSection);
						updateFTPstr();
					}
				}
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
	closesocket(udp_server_socket);
	inited = 0;
	DeleteCriticalSection(&m_criticalSection);
	m_ftpservers.clear();
}


void D2Xxbautodetect::updateFTPstr()
{
	EnterCriticalSection(&m_criticalSection);
	g_d2xSettings.autoFTPstr = m_ftpservers;
	LeaveCriticalSection(&m_criticalSection);
}

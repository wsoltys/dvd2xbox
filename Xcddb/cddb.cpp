//-----------------------------------------------------------------------------
// File: cddb.cpp
//
// Desc: Make a connection to the CDDB database
//
// Hist: 00.00.01
//
//-----------------------------------------------------------------------------

#include "cddb.h"

Xcddb::Xcddb() 
{
	debug_client=NULL;
	mpDebug=NULL;
	b_debug_enabled=false;
	initialize_network_ok=false;
	lastError=0;
	strcpy(cddb_ip_adress,"194.97.4.18");
}

Xcddb::Xcddb(CXBoxDebug *p_mpDebug, CDebugClient *p_debug_client) 
{
	p_mpDebug->Message(L"Xcddb Constructor start");
	debug_client=p_debug_client;
	mpDebug=p_mpDebug;
	b_debug_enabled=true;
	writeLog("Xcddb Constructor ende");
	initialize_network_ok=false;
	lastError=0;
}

HRESULT Xcddb::openSocket()
{
	writeLog("Xcddb::openSocket - Start");
	unsigned int port=CDDB_PORT;
	writeLog(cddb_ip_adress);

	sockaddr_in service;
	service.sin_family = AF_INET;
	
	// connect to site directly
	service.sin_addr.s_addr = inet_addr(cddb_ip_adress);
	service.sin_port = htons(port);
	m_cddb_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	// attempt to connection
	if (connect(m_cddb_socket,(sockaddr*) &service,sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		writeLog("Connect Failed");
		char strError[128];
		sprintf(strError,"error:%i %i", GetLastError(), WSAGetLastError());
		writeLog(strError);
		closesocket(m_cddb_socket);
		m_cddb_socket=NULL;
		return E_FAIL;
	}
	writeLog("Connect OK :-)");
	return S_OK;
}

HRESULT Xcddb::closeSocket()
{
	writeLog("Xcddb::closeSocket - Start");
	//closesocket(m_cddb_socket);
	m_cddb_socket=NULL;
	return S_OK;
}

HRESULT Xcddb::Send( void *buffer, int bytes )
{
	//writeLog("Xcddb::send - Start");
	char *tmp_buffer=new char[bytes+3];
	writeLog(tmp_buffer);
	strcpy(tmp_buffer,(const char*)buffer);
	tmp_buffer[bytes]='.';
	tmp_buffer[bytes+1]=0x0d;
	tmp_buffer[bytes+2]=0x0a;
	tmp_buffer[bytes+3]=0x00;
	writeLog("Sende:");
	writeLog(tmp_buffer);
	int iErr=send(m_cddb_socket,(const char*)tmp_buffer,bytes+3,0);
	if (iErr<=0)
	{
		return WSAGetLastError();
	}
	return S_OK;
}

HRESULT Xcddb::Send( char *buffer)
{
	int iErr=Send(buffer,strlen(buffer));
	if (iErr<=0)
	{
		return WSAGetLastError();
	}
	return S_OK;
}

string Xcddb::Recv(bool wait4point)
{
	//writeLog("Xcddb::recv - Start");	
	std::strstream buffer;
	WCHAR b;
	CHAR tmpbuffer[3];
	int counter=0;
	bool new_line=false;
	writeLog("-=00000 receive_s 00000=-");
	bool found_211=false;
	while (true)
	{
		long lenRead=recv(m_cddb_socket,(char*)&tmpbuffer,1,0);
		b=tmpbuffer[0];
		buffer.write(tmpbuffer,1);
		if(counter==0 && b=='2')
			found_211=true;
		else if(counter==0)
			found_211=false;

		if(counter==1 && b=='1')
			found_211=found_211&true;
		else if(counter==1)
			found_211=false;

		if(counter==2 && b=='1')
			found_211=found_211&true;
		else if(counter==2)
			found_211=false;

		if(counter==2 && found_211)
		{
			writeLog("Found 211: wait4point=true");
			wait4point=true;
		}
		counter++;
		if (new_line && b=='.')
			break;
		new_line=(b==0x0a);
		if (!wait4point && b==0x0a)
		{
			break;
		}
	}
	writeLog("-=00000 receive_e 00000=-");
	buffer.write(0x00,1);
	string str_buffer=std::string(buffer.str(),buffer.pcount());

	char str[4096];
	sprintf(str,"Empfangen %d bytes // Buffer= %d bytes",counter,str_buffer.size());
	writeLog(str);
	
	writeLog("Empfangen");
	writeLog((char *)str_buffer.c_str());
	return str_buffer;	
}

int Xcddb::queryCDinfo(int inexact_list_select)
{
	if (getInexactCommand(inexact_list_select)==NULL)
	{
		lastError=E_PARAMETER_WRONG;	
		return E_FAILED;
	}
	char read_buffer[1024];
	sprintf(read_buffer,"%s",getInexactCommand(inexact_list_select));
	
	//erstmal den Müll abholen
	Recv(false);

	if( FAILED( Send(read_buffer) ))
	{
		writeLog("Send Failed");
		writeLog(read_buffer);
		return E_FAILED;
	}


// cddb read Antwort
	string recv_buffer=Recv(true);
	writeLog("XXXXXXXXXXXXXXXX");
	/*
	210	OK, CDDB database entry follows (until terminating marker)
	401	Specified CDDB entry not found.
	402	Server error.
	403	Database entry is corrupt.
	409	No handshake.
	*/
	char *tmp_str2;
	tmp_str2=(char *)recv_buffer.c_str();
	writeLog("------------------------------");
	writeLog(tmp_str2);
	writeLog("------------------------------");
	switch (tmp_str2[0]-48)
	{	
		case 2:
			writeLog("2-- XXXXXXXXXXXXXXXX");
			// Cool, I got it ;-)
			parseData(tmp_str2);
		break;
		case 4:
			writeLog("4-- XXXXXXXXXXXXXXXX");
			switch (tmp_str2[2]-48)
			{
				case 1: //401
					writeLog("401 XXXXXXXXXXXXXXXX");
					lastError=401;
				break;
				case 2: //402
					writeLog("402 XXXXXXXXXXXXXXXX");
					lastError=402;
				break;
				case 3: //403
					writeLog("403 XXXXXXXXXXXXXXXX");
					lastError=403;
				break;
				case 9: //409
					writeLog("409 XXXXXXXXXXXXXXXX");
					lastError=409;
				break;
				default:
					lastError=-1;
					return E_FAILED;
			}
		break;
		default:
			lastError=-1;
			return E_FAILED;
	}

//##########################################################
// Abmelden 2x Senden kommt sonst zu fehler
	if( FAILED( Send("quit") ))
	{
		writeLog("Send Failed");
		return E_FAILED;
	}

// quit Antwort
	Recv(false);

// Socket schliessen
	if( FAILED( closeSocket() ))
	{
		writeLog("closeSocket Failed");
		return E_FAILED;
	}
	else
	{
		writeLog("closeSocket OK");
	}
	return OK;
}

int Xcddb::queryCDinfo(int real_track_count, toc cdtoc[])
{
	writeLog("Xcddb::queryCDinfo - Start");

/*	writeLog("getHostByName start");
	struct hostent* hp=gethostbyname("freedb.freedb.org");
	writeLog("hp->h_name=%s",hp->h_name);
	writeLog("hp->h_aliases=%s",hp->h_aliases);
	writeLog("hp->h_addr_list=%s",hp->h_addr_list);
	writeLog("getHostByName end");
*/
	int lead_out=real_track_count;
    unsigned long discid=calc_disc_id(real_track_count, cdtoc);
	unsigned long frames[100];

	for (int i=0;i<=lead_out;i++)
	{
		frames[i]=(cdtoc[i].min*75*60)+(cdtoc[i].sec*75)+cdtoc[i].frame;
		if (i>0 && frames[i]<frames[i-1])
		{
			lastError=E_TOC_INCORRECT;
			return E_FAILED;
		}
	}
	unsigned long complete_length=frames[lead_out]/75;

	// Debug ausgaben start
	{
		char buffer[100];
		sprintf(buffer,"real_track_count=%u", real_track_count);
		writeLog(buffer);
	}	
	{
		char buffer[100];
		sprintf(buffer,"complete_length=%lu sec", complete_length);
		writeLog(buffer);
	}	
	{
		char buffer[100];
		sprintf(buffer,"The discid is %08x", discid);
		writeLog(buffer);
	}
	{
		for (int i=0;i<=lead_out;i++)
		{
			char buffer[100];
			if (i!=lead_out)
            	sprintf(buffer,"Track %u = %lu", i+1,frames[i]);
			else
				sprintf(buffer,"Lead Out = %lu", frames[i]);
			writeLog(buffer);
		}
	}
	// Debug ausgaben ende

//#####################################################################
// Socket öffnen
	if ( FAILED(openSocket()) )
	{
		writeLog("openSocket Failed");
		lastError=E_NETWORK_ERROR_OPEN_SOCKET;
		return E_FAILED;
	}

//#####################################################################
// Erst mal was empfangen
	string recv_buffer=Recv(false);
	/*
	200	OK, read/write allowed
	201	OK, read only
	432	No connections allowed: permission denied
	433	No connections allowed: X users allowed, Y currently active
	434	No connections allowed: system load too high
	*/
	if (recv_buffer.c_str()[0]=='2')
	{
		//OK
		writeLog("Connection 2 cddb: OK");
		lastError=IN_PROGRESS;
	}
	else if (recv_buffer.c_str()[0]=='4')
	{
		//No connections allowed
		writeLog("Connection 2 cddb: No connections allowed");
		lastError=430+(recv_buffer.c_str()[3]-48);
		return E_FAILED; 
	}
//#####################################################################
// Jetzt hello Senden
	if( FAILED( Send("cddb hello xbox xbox xcddb 00.00.01") ))
	{
		writeLog("Send Failed");
		lastError=E_NETWORK_ERROR_SEND;
		return E_FAILED;
	}

// hello Antwort	
	recv_buffer=Recv(false);
	/*
	200	Handshake successful
	431	Handshake not successful, closing connection
	402	Already shook hands
	*/
	if (recv_buffer.c_str()[0]=='2')
	{
		//OK
		writeLog("Hello 2 cddb: OK");
		lastError=IN_PROGRESS;
	}
	else if (recv_buffer.c_str()[0]=='4' && recv_buffer.c_str()[1]=='3')
	{
		//No connections allowed
		writeLog("Hello 2 cddb: Handshake not successful, closing connection");
		lastError=E_CDDB_Handshake_not_successful;
		return E_FAILED; 
	}
	else if (recv_buffer.c_str()[0]=='4' && recv_buffer.c_str()[1]=='0')
	{
		writeLog("Hello 2 cddb: Already shook hands, but it's OK");
		lastError=W_CDDB_already_shook_hands;
	}


// Hier jetzt die CD abfragen
//##########################################################
	char query_buffer[1024];
	strcpy(query_buffer,"");
	strcat(query_buffer,"cddb query");
	{
		char tmp_buffer[256];
		sprintf(tmp_buffer," %08x", discid);
		strcat(query_buffer,tmp_buffer);
	}
	{
		char tmp_buffer[256];
		sprintf(tmp_buffer," %u", real_track_count);
		strcat(query_buffer,tmp_buffer);
	}
	for (int i=0;i<lead_out;i++)
	{
		char tmp_buffer[256];
		sprintf(tmp_buffer," %u", frames[i]);
		strcat(query_buffer,tmp_buffer);
	}
	{
		char tmp_buffer[256];
		sprintf(tmp_buffer," %u", complete_length);
		strcat(query_buffer,tmp_buffer);
	}

	//cddb query
	if( FAILED( Send(query_buffer)))
	{
		writeLog("Send Failed");
		lastError=E_NETWORK_ERROR_SEND;
		return E_FAILED;
	}

// Antwort
// 200 rock d012180e Soundtrack / Hackers
	char read_buffer[1024];
	recv_buffer=Recv(false);
	// Hier antwort auswerten
	/*
	200	Found exact match
	211	Found inexact matches, list follows (until terminating marker)
	202	No match found
	403	Database entry is corrupt
	409	No handshake
	*/
	char *tmp_str;
	tmp_str=(char *)recv_buffer.c_str();
	switch (tmp_str[0]-48)
	{	
		case 2:
			switch (tmp_str[1]-48)
			{
				case 0:
					switch (tmp_str[2]-48)
					{
						case 0: //200
							strtok(tmp_str," ");
							strcpy(read_buffer,"");
							strcat(read_buffer,"cddb read ");
							// categ
							strcat(read_buffer,strtok(0," "));
							{
								char tmp_buffer[256];
								sprintf(tmp_buffer," %08x", discid);
								strcat(read_buffer,tmp_buffer);
							}
							lastError=IN_PROGRESS;
						break;
						case 2: //202
							lastError=E_NO_MATCH_FOUND;
							return E_FAILED;
						break;
						default:
							lastError=E_FAILED;
							return E_FAILED;
					}
				break;
				case 1:
					switch (tmp_str[2]-48)
					{
						case 1: //211
							lastError=E_INEXACT_MATCH_FOUND;
							/*
							211 Found inexact matches, list follows (until terminating `.')
							soundtrack bf0cf90f Modern Talking / Victory - The 11th Album
							rock c90cf90f Modern Talking / Album: Victory (The 11th Album)
							misc de0d020f Modern Talking / Ready for the victory
							rock e00d080f Modern Talking / Album: Victory (The 11th Album)
							rock c10d150f Modern Talking / Victory (The 11th Album)
							.
							*/
							addInexactList(tmp_str);
							lastError=E_WAIT_FOR_INPUT;
							return E_FAILED;
						break;
						default:
							lastError=-1;
							return E_FAILED;
					}
				break;
				default:
					lastError=-1;
					return E_FAILED;
			}
		break;
		case 4:
			switch (tmp_str[2]-48)
			{
				case 3: //403
					lastError=403;
				break;
				case 9: //409
					lastError=409;
				break;
				default:
					lastError=-1;
					return E_FAILED;
			}
		break;
		default:
			lastError=-1;
			return E_FAILED;
	}


//##########################################################
	if( FAILED( Send(read_buffer) ))
	{
		writeLog("Send Failed");
		writeLog(read_buffer);
		return E_FAILED;
	}


// cddb read Antwort
	recv_buffer=Recv(true);
	/*
	210	OK, CDDB database entry follows (until terminating marker)
	401	Specified CDDB entry not found.
	402	Server error.
	403	Database entry is corrupt.
	409	No handshake.
	*/
	char *tmp_str2;
	tmp_str2=(char *)recv_buffer.c_str();
	switch (tmp_str2[0]-48)
	{	
		case 2:
			writeLog("2-- XXXXXXXXXXXXXXXX");
			// Cool, I got it ;-)
			parseData(tmp_str2);
		break;
		case 4:
			writeLog("4-- XXXXXXXXXXXXXXXX");
			switch (tmp_str2[2]-48)
			{
				case 1: //401
					writeLog("401 XXXXXXXXXXXXXXXX");
					lastError=401;
				break;
				case 2: //402
					writeLog("402 XXXXXXXXXXXXXXXX");
					lastError=402;
				break;
				case 3: //403
					writeLog("403 XXXXXXXXXXXXXXXX");
					lastError=403;
				break;
				case 9: //409
					writeLog("409 XXXXXXXXXXXXXXXX");
					lastError=409;
				break;
				default:
					lastError=-1;
					return E_FAILED;
			}
		break;
		default:
			lastError=-1;
			return E_FAILED;
	}

//##########################################################
// Abmelden 2x Senden kommt sonst zu fehler
	if( FAILED( Send("quit") ))
	{
		writeLog("Send Failed");
		return E_FAILED;
	}

// quit Antwort
	Recv(false);

// Socket schliessen
	if( FAILED( closeSocket() ))
	{
		writeLog("closeSocket Failed");
		return E_FAILED;
	}
	else
	{
		writeLog("closeSocket OK");
	}
	lastError=QUERRY_OK;
	return OK;
}

void Xcddb::writeLog(char *str)
{
	if (b_debug_enabled && (debug_client != NULL))
	{
		debug_client->Send(str,strlen (str));
		debug_client->Send("\n",1);
		//Sleep(250);
		//debug_client.WaitKey();
	}
	if (mpDebug!=NULL && false)
	{
		WCHAR buffer[4096];
		swprintf(buffer,L"%S", str);
		mpDebug->Message(buffer);
		mpDebug->Display();
	}
}

void Xcddb::writeLog(const char* pzFormat, ...)
{
	if (b_debug_enabled && (debug_client != NULL))
	{
		char buf[512];
		va_list arg;

		va_start( arg, pzFormat );

		vsprintf( buf, pzFormat, arg );
		strcat(buf,"\n");

		debug_client->Send(buf,strlen (buf));
		debug_client->Send("\n",1);
		va_end( arg );
	}
}

BOOL Xcddb::InitDebug(char *szRemoteAddress, int remotePort)
{
	debug_client = new CDebugClient();
	int ret = debug_client->Init( szRemoteAddress, remotePort, 0 );
	if (ret <= 0 )
	{
		b_debug_enabled=false;
	}
	else
	{
		b_debug_enabled=true;
	}
	return b_debug_enabled;
}

BOOL Xcddb::InitializeNetwork(char *szLocalAddress,	char *szLocalSubnet, char *szLocalGateway)
{
	if (!IsEthernetConnected())
		return FALSE;
	if (initialize_network_ok)
		return TRUE;

	// if local address is specified
	if ( (szLocalAddress[0]!=0) &&
		 (szLocalSubnet[0]!=0)  &&
		 (szLocalGateway[0]!=0)  )

	{
	// Thanks and credits to Team Evox for the description of the 
	// XNetConfigParams structure.

	TXNetConfigParams configParams;   

	writeLog("Loading network configuration...");
	XNetLoadConfigParams( (LPBYTE) &configParams );
	writeLog("Ready.");
	BOOL bXboxVersion2 = (configParams.V2_Tag == 0x58425632 );	// "XBV2"
	BOOL bDirty = FALSE;
	if (bXboxVersion2)
		writeLog("bXboxVersion2");
	else
		writeLog("no bXboxVersion2");

	writeLog("User local address: ");
	writeLog(szLocalAddress);
	if (bXboxVersion2)
	{
		if (configParams.V2_IP != inet_addr(szLocalAddress))
		{
			configParams.V2_IP = inet_addr(szLocalAddress);
			bDirty = TRUE;
		}
	}
	else
	{
		if (configParams.V1_IP != inet_addr(szLocalAddress))
		{
			configParams.V1_IP = inet_addr(szLocalAddress);
			bDirty = TRUE;
		}
	}

	writeLog("User subnet mask: ");
	writeLog(szLocalSubnet);

	if (bXboxVersion2)
	{
		if (configParams.V2_Subnetmask != inet_addr(szLocalSubnet))
		{
			configParams.V2_Subnetmask = inet_addr(szLocalSubnet);
			bDirty = TRUE;
		}
	}
	else
	{
		if (configParams.V1_Subnetmask != inet_addr(szLocalSubnet))
		{
			configParams.V1_Subnetmask = inet_addr(szLocalSubnet);
			bDirty = TRUE;
		}
	}

	writeLog("User gateway address: ");
	writeLog(szLocalGateway);

	if (bXboxVersion2)
	{
		if (configParams.V2_Defaultgateway != inet_addr(szLocalGateway))
		{
			configParams.V2_Defaultgateway = inet_addr(szLocalGateway);
			bDirty = TRUE;
		}
	}
	else
	{
		if (configParams.V1_Defaultgateway != inet_addr(szLocalGateway))
		{
			configParams.V1_Defaultgateway = inet_addr(szLocalGateway);
			bDirty = TRUE;
		}
	}

	if (configParams.Flag != (0x04|0x08) )
	{
		configParams.Flag = 0x04 | 0x08;
		bDirty = TRUE;
	}

	if (bDirty)
	{
		writeLog("Updating network configuration...");
		XNetSaveConfigParams( (LPBYTE) &configParams );
		writeLog("Ready.");
	}

	} //if local address is specified

	XNetStartupParams xnsp;
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);

	// Bypass security so that we may connect to 'untrusted' hosts
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	// create more memory for networking
	xnsp.cfgPrivatePoolSizeInPages = 64; // == 256kb, default = 12 (48kb)
	xnsp.cfgEnetReceiveQueueLength = 16; // == 32kb, default = 8 (16kb)
	xnsp.cfgIpFragMaxSimultaneous = 16; // default = 4
	xnsp.cfgIpFragMaxPacketDiv256 = 32; // == 8kb, default = 8 (2kb)
	xnsp.cfgSockMaxSockets = 64; // default = 64
	xnsp.cfgSockDefaultRecvBufsizeInK = 128; // default = 16
	xnsp.cfgSockDefaultSendBufsizeInK = 128; // default = 16
	INT err = XNetStartup(&xnsp);

	XNADDR xna;
	DWORD dwState;
	do
	{
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(500);
	} while (dwState==XNET_GET_XNADDR_PENDING);
	
	WSADATA WsaData;
	err = WSAStartup( MAKEWORD(2,2), &WsaData );
	initialize_network_ok=(err==NO_ERROR);
	writeLog("InitializeNetwork done");
	return ( err == NO_ERROR );
}

BOOL Xcddb::IsEthernetConnected()
{
	if (!(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE))
		return FALSE;

	return TRUE;
}

int Xcddb::getLastError()
{
	return lastError;
}


char *Xcddb::getLastErrorText()
{
	switch (getLastError())
	{
		case IN_PROGRESS:
			return "in Progress";
			break;
		case OK:
			return "OK";
			break;
		case E_FAILED:
			return "Failed";
			break;
		case E_TOC_INCORRECT:
			return "TOC Incorecct";
			break;
		case E_NETWORK_ERROR_OPEN_SOCKET:
			return "Error open Socket";
			break;
		case E_NETWORK_ERROR_SEND:
			return "Error send PDU";
			break;
		case E_WAIT_FOR_INPUT:
			return "Wait for Input";
			break;
		case E_PARAMETER_WRONG:
			return "Error Parameter Wrong";
			break;
		case E_NO_MATCH_FOUND:
			return "No Match found";
			break;
		case E_INEXACT_MATCH_FOUND:
			return "Inexact Match found";
			break;
		case W_CDDB_already_shook_hands:
			return "Warning already shook hands";
			break;
		case E_CDDB_Handshake_not_successful:
			return "Error Handshake not successful";
			break;
		case E_CDDB_permission_denied:
			return "Error permission denied";
			break;
		case E_CDDB_max_users_reached:
			return "Error max cddb users reached";
			break;
		case E_CDDB_system_load_too_high:
			return "Error cddb system load too high";
			break;
		case QUERRY_OK:
			return "Querry OK";
			break;
	}
	return "";
}


int Xcddb::cddb_sum(int n)
{
	int	ret;

	/* For backward compatibility this algorithm must not change */

	ret = 0;

	while (n > 0) {
		ret = ret + (n % 10);
		n = n / 10;
	}

	return (ret);
}

unsigned long Xcddb::calc_disc_id(int tot_trks, toc cdtoc[])
{
	int	i= 0,	t = 0,	n = 0;

	while (i < tot_trks) 
	{
		
		n = n + cddb_sum((cdtoc[i].min * 60) + cdtoc[i].sec);
		i++;
	}

	t = ((cdtoc[tot_trks].min * 60) + cdtoc[tot_trks].sec) -((cdtoc[0].min * 60) + cdtoc[0].sec);

	return ((n % 0xff) << 24 | t << 8 | tot_trks);
}

void Xcddb::addTitle(char *buffer)
{
	char value[2048];
	int trk_nr=0;
	//TTITLEN
	if (buffer[7]=='=')
	{ //Einstellig
		trk_nr=buffer[6]-47;
		strcpy(value,buffer+8);
	}
	else if (buffer[8]=='=')
	{ //Zweistellig
		trk_nr=((buffer[6]-48)*10)+buffer[7]-47;
		strcpy(value,buffer+9);
	}
	else if (buffer[9]=='=')
	{ //Dreistellig
		trk_nr=((buffer[6]-48)*100)+((buffer[7]-48)*10)+buffer[8]-47;
		strcpy(value,buffer+10);
	}
	else
	{
		return;
	}

	// track artist" / "track title 
	char artist[1024];
	char title[1024];
	unsigned int len=(unsigned int)strlen(value);
	bool found=false;
	for(unsigned int i=0;i<len;i++)
	{
		if ((i+2)<len && value[i]==' ' && value[i+1]=='/' && value[i+2]==' ') 
		{
			// Jep found
			found=true;
			break;
		}
	}
	if (found)
	{
		strncpy(artist,value,i);
		artist[i]='\0';
		strcpy(title,value+i+3);
	}
	else
	{
		artist[0]='\0';
		strcpy(title,value);
	}
	artists[trk_nr]=artist;
	titles[trk_nr]=title;

//	writeLog(artist);
//	writeLog(title);
}

const char *Xcddb::getInexactCommand(int select)
{
	typedef map<int,string>::const_iterator iter;
	iter i = inexact_cddb_command_list.find(select);
	if (i==inexact_cddb_command_list.end())
		return NULL;
	return i->second.c_str();
}

const char *Xcddb::getInexactArtist(int select)
{
	typedef map<int,string>::const_iterator iter;
	iter i = inexact_artist_list.find(select);
	if (i==inexact_artist_list.end())
		return NULL;
	return i->second.c_str();
}

const char *Xcddb::getInexactTitle(int select)
{
	typedef map<int,string>::const_iterator iter;
	iter i = inexact_title_list.find(select);
	if (i==inexact_title_list.end())
		return NULL;
	return i->second.c_str();
}

const char *Xcddb::getTrackArtist(int track)
{
	typedef map<int,string>::const_iterator iter;
	iter i = artists.find(track);
	if (i==artists.end())
		return NULL;
	return i->second.c_str();
}

const char* Xcddb::getTrackTitle(int track)
{
	typedef map<int,string>::const_iterator iter;
	iter i = titles.find(track);
	if (i==titles.end())
		return NULL;
	return i->second.c_str();
}

void Xcddb::getDiskTitle(char* t_disk_title)
{
	strcpy(t_disk_title,disk_title);
}

void Xcddb::getDiskArtist(char* t_disk_artist)
{
	strcpy(t_disk_artist,disk_artist);
}

void Xcddb::parseData(char *buffer)
{
	writeLog("parseData Start");

	char *line;
	const char trenner[2]={'\n',0x0};
	line=strtok(buffer,trenner);
	int line_cnt=0;
	while(line = strtok(0,trenner))
	{
		if (line[0]!='#')
		{
			if (0==strncmp(line,"DTITLE",6))
			{
				// DTITLE=Modern Talking / Album: Victory (The 11th Album)
				unsigned int len=(unsigned int)strlen(line)-6;
				bool found=false;
				unsigned int i=5;
				for(;i<len;i++)
				{
					if ((i+2)<len && line[i]==' ' && line[i+1]=='/' && line[i+2]==' ') 
					{
						// Jep found
						found=true;
						break;
					}
				}
				if (found)
				{
					strncpy(disk_artist,line+7,i-7);
					disk_artist[i-7]='\0';
					strcpy(disk_title,line+i+3);
				}
				else
				{
					disk_artist[0]='\0';
					strcpy(disk_title,line+7);
				}
			}
			else if (0==strncmp(line,"DYEAR",5))
			{
				strcpy(year,line+5);
			}
			else if (0==strncmp(line,"DGENRE",6))
			{
				strcpy(genre,line+6);
			}
			else if (0==strncmp(line,"TTITLE",6))
			{
				addTitle(line);
			}
			else if (0==strncmp(line,"EXTD",4))
			{
			}
			else if (0==strncmp(line,"EXTT",4))
			{
				addExtended(line);
			}
		}
		line_cnt++;
	}
	writeLog("parseData Ende");
}

void Xcddb::addExtended(char *buffer)
{
	char value[2048];
	int trk_nr=0;
	//TTITLEN
	if (buffer[5]=='=')
	{ //Einstellig
		trk_nr=buffer[4]-47;
		strcpy(value,buffer+6);
	}
	else if (buffer[6]=='=')
	{ //Zweistellig
		trk_nr=((buffer[4]-48)*10)+buffer[5]-47;
		strcpy(value,buffer+7);
	}
	else if (buffer[7]=='=')
	{ //Dreistellig
		trk_nr=((buffer[4]-48)*100)+((buffer[5]-48)*10)+buffer[6]-47;
		strcpy(value,buffer+8);
	}
	else
	{
		return;
	}
	extended_track[trk_nr]=value;
}

const char* Xcddb::getTrackExtended(int track)
{
	typedef map<int,string>::const_iterator iter;
	iter i = extended_track.find(track);
	if (i==extended_track.end())
		return NULL;
	return i->second.c_str();
}

void Xcddb::addInexactList(char *list)
{
	/*						
	211 Found inexact matches, list follows (until terminating `.')
	soundtrack bf0cf90f Modern Talking / Victory - The 11th Album
	rock c90cf90f Modern Talking / Album: Victory (The 11th Album)
	misc de0d020f Modern Talking / Ready for the victory
	rock e00d080f Modern Talking / Album: Victory (The 11th Album)
	rock c10d150f Modern Talking / Victory (The 11th Album)
	.
	*/
	
	/*
	inexact_cddb_command_list;
	inexact_artist_list;
	inexact_title_list;
	*/
	int start=0;
	int end=0;
	bool found=false;
	int line_counter=0;
	writeLog("addInexactList Start");
	for (unsigned int i=0;i<strlen(list);i++)
	{
		if (list[i]=='\n')
		{
			end=i;
			found=true;
		}
		if (found)
		{
			if (line_counter>0)
			{
				addInexactListLine(line_counter,list+start,end-start);
			}
			start=i+1;
			line_counter++;
			found=false;
		}	
	}
	writeLog("addInexactList End");
}

void Xcddb::addInexactListLine(int line_cnt, char *line, int len)
{
	// rock c90cf90f Modern Talking / Album: Victory (The 11th Album)
	int search4=    	   0;
	char genre[100];	// 0
	char discid[10];	// 1
	char artist[1024];	// 2
	char title[1024];
	char cddb_command[1024];
	int start=0;	
	writeLog("addInexactListLine Start");
	for (int i=0;i<len;i++)
	{
		switch(search4)
		{
			case 0:
				if (line[i]==' ')
				{
					strncpy(genre,line,i);
					genre[i]=0x00;
					search4=1;
					start=i+1;
				}
			break;
			case 1:
				if (line[i]==' ')
				{
					strncpy(discid,line+start,i-start);
					discid[i-start]=0x00;
					search4=2;
					start=i+1;
				}
			break;
			case 2:
				if (i+2<=len && line[i]==' ' && line[i+1]=='/' && line[i+2]==' ')
				{
					strncpy(artist,line+start,i-start);
					artist[i-start]=0x00;
					strncpy(title,line+(i+3),len-(i+3));
					title[len-(i+3)]=0x00;
				}
			break;
		}
	}
	sprintf(cddb_command,"cddb read %s %s",genre,discid);

	inexact_cddb_command_list[line_cnt]=cddb_command;
	inexact_artist_list[line_cnt]=artist;
	inexact_title_list[line_cnt]=title;
	char log_string[1024];
	sprintf(log_string,"%u: %s - %s",line_cnt,artist,title);
	writeLog(log_string);
	writeLog("addInexactListLine End");
}

void Xcddb::setCDDBIpAdress(char *ip_adress)
{
	strcpy(cddb_ip_adress,ip_adress);
}

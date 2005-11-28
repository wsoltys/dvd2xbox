#ifndef D2XXBAUTODETECT
#define D2XXBAUTODETECT

#include <xtl.h>
#include <thread.h>
#include "d2xsettings.h"
#include <StringUtils.h>
#include <stdstring.h>
#include "d2xutils.h"


class D2Xxbautodetect : public CThread
{
protected:
	struct sockaddr_in	server;
	struct sockaddr_in	cliAddr;
	struct timeval timeout;

	static int udp_server_socket;
	static int inited;
	static DWORD pingTimer;

	int			iUDPPort,life,cliLen;
	fd_set		readfds;

	CStdString	strSendMessage;
	CStdString	strReceiveMessage;
	CStdString	strWorkTemp;
	CStdString	strUser;
	CStdString	strPWD;
	CStdString	strNick;

	char  sztmp[512], szTemp[512];
	

	
public:
	D2Xxbautodetect();
	~D2Xxbautodetect();
							
	virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();

	static void updateFTPstr();

};

#endif
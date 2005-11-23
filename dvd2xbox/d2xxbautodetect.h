#ifndef D2XXBAUTODETECT
#define D2XXBAUTODETECT

#include <xtl.h>
#include <thread.h>
#include "d2xsettings.h"
//#include "d2xutils.h"
//#include <stdstring.h>


class D2Xxbautodetect : public CThread
{
protected:
	struct sockaddr_in	server;
	struct sockaddr_in	cliAddr;
	struct timeval timeout;

	static int udp_server_socket;
	static int inited;

	int			iUDPPort,life,cliLen;
	fd_set		readfds;

	CStdString	strSendMessage;
	CStdString	strReceiveMessage;
	CStdString	strWorkTemp;

	char  sztmp[512], szTemp[512];
	
public:
	D2Xxbautodetect();
	~D2Xxbautodetect();
							
	virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();

};

#endif
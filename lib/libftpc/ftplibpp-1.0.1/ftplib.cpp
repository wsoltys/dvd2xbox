/***************************************************************************
                          ftplib.cpp  -  description
                             -------------------
    begin                : Son Jul 27 2003
    copyright            : (C) 2003 by mkulke
    email                : sikor_sxe@radicalapproach.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* original unmodified copyright notes from Thomas Pfau */
 
/***************************************************************************/
/*									   */
/* ftplib.c - callable ftp access routines				   */
/* Copyright (C) 1996, 1997, 1998 Thomas Pfau, pfau@cnj.digex.net	   */
/*	73 Catherine Street, South Bound Brook, NJ, 08880		   */
/*									   */
/* This library is free software; you can redistribute it and/or	   */
/* modify it under the terms of the GNU Library General Public		   */
/* License as published by the Free Software Foundation; either		   */
/* version 2 of the License, or (at your option) any later version.	   */
/* 									   */
/* This library is distributed in the hope that it will be useful,	   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of	   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	   */
/* Library General Public License for more details.			   */
/* 									   */
/* You should have received a copy of the GNU Library General Public	   */
/* License along with this progam; if not, write to the			   */
/* Free Software Foundation, Inc., 59 Temple Place - Suite 330,		   */
/* Boston, MA 02111-1307, USA.						   */
/* 									   */
/***************************************************************************/

#include "ftplib.h"
//#include <helper.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <xbsocket.h>
//#include <netinet/in.h>
#include <XBNet.h>
//#include <netdb.h>
//#include <arpa/inet.h>
#include <string>


using namespace std;

/* io function names */
#define net_read(socket,buffer,len) recv(socket,(char*)buffer,len,0)
#define net_write(socket,buffer,len) send(socket,(const char*)buffer,len,0) 
#define net_close closesocket

/* socket values */
#define SETSOCKOPT_OPTVAL_TYPE (void *)
#define SETSOCKOPT_OPTVAL_TYPE (const char *)
#define FTPLIB_BUFSIZ 1024
#define ACCEPT_TIMEOUT 30

/* io types */
#define FTPLIB_CONTROL 0
#define FTPLIB_READ 1
#define FTPLIB_WRITE 2

ftplib::ftplib()
{
//    SSL_library_init();
    mp_netbuf = static_cast<netbuf *>(calloc(1,sizeof(netbuf)));
    if (mp_netbuf == NULL) perror("calloc");
    mp_netbuf->buf = static_cast<char *>(malloc(FTPLIB_BUFSIZ));
    if (mp_netbuf->buf == NULL)
    {
	perror("calloc");
	free(mp_netbuf);
    }
    mp_netbuf->dir = FTPLIB_CONTROL;
    mp_netbuf->ctrl = NULL;
    mp_netbuf->cmode = ftplib::defaultconnmode;
    mp_netbuf->idlecb = NULL;
    mp_netbuf->idletime.tv_sec = mp_netbuf->idletime.tv_usec = 0;
    mp_netbuf->idlearg = NULL;
    mp_netbuf->xfered = 0;
    mp_netbuf->xfered1 = 0;
    mp_netbuf->cbbytes = 0;
    mp_netbuf->tlsctrl = 0;
    mp_netbuf->tlsdata = 0;
    mp_netbuf->offset = 0;
    mp_netbuf->logcb = NULL;   
//    mp_netbuf->ctx = SSL_CTX_new(SSLv23_method());
//    SSL_CTX_set_verify(mp_netbuf->ctx, SSL_VERIFY_NONE, NULL);
//    mp_netbuf->ssl = SSL_new(mp_netbuf->ctx);
}

ftplib::~ftplib()
{
    //if (mp_netbuf->tlsctrl)
    //{
	//SSL_free(mp_netbuf->ssl);
	//SSL_CTX_free(mp_netbuf->ctx);
    //}
    free(mp_netbuf->buf);
    free(mp_netbuf);    
}

bool ftplib::isConnected()
{
	if(mp_netbuf->handle == 0)
		return false;
	else
		return true;
}

/*
 * socket_wait - wait for socket to receive or flush data
 *
 * return 1 if no user callback, otherwise, return value returned by
 * user callback
 */
int ftplib::socket_wait(netbuf *ctl)
{
    fd_set fd,*rfd = NULL,*wfd = NULL;
    struct timeval tv;
    int rv = 0;
    if ((ctl->dir == FTPLIB_CONTROL) || (ctl->idlecb == NULL) /*|| ((ctl->idletime.tv_sec == 0) && (ctl->idletime.tv_usec == 0))*/)
	return 1;
    if (ctl->dir == FTPLIB_WRITE)
	wfd = &fd;
    else
	rfd = &fd;
    FD_ZERO(&fd);
    do
    {
		FD_SET(ctl->handle,&fd);
		tv = ctl->idletime;
		rv = select(ctl->handle+1, rfd, wfd, NULL, &tv);
		if (rv == -1)
		{
	    	rv = 0;
	    	strncpy(ctl->ctrl->response, strerror(errno), sizeof(ctl->ctrl->response));
	    	break;
		}
		else if (rv > 0)
		{
	    	rv = 1;
	    	break;
		}
    } while ((rv = ctl->idlecb(ctl->xfered, ctl->idlearg)));
    return rv;
}

/*
 * read a line of text
 *
 * return -1 on error or bytecount
 */
int ftplib::readline(char *buf,int max,netbuf *ctl)
{
    int x,retval = 0;
    char *end,*bp=buf;
    int eof = 0;
    
    if ((ctl->dir != FTPLIB_CONTROL) && (ctl->dir != FTPLIB_READ))
	return -1;
    if (max == 0)
	return 0;
    do
    {
    	if (ctl->cavail > 0)
    	{
	    	x = (max >= ctl->cavail) ? ctl->cavail : max-1;
	    	end = static_cast<char*>(memccpy(bp,ctl->cget,'\n',x));
	    	if (end != NULL)
			x = end - bp;
	    	retval += x;
	    	bp += x;
	    	*bp = '\0';
	    	max -= x;
	    	ctl->cget += x;
	    	ctl->cavail -= x;
	    	if (end != NULL)
	    	{
				bp -= 2;
				if (strcmp(bp,"\r\n") == 0)
				{
		    		*bp++ = '\n';
		    		*bp++ = '\0';
		    		--retval;
				}
	    		break;
	    	}
    	}
    	if (max == 1)
    	{
	    	*buf = '\0';
	    	break;
    	}
    	if (ctl->cput == ctl->cget)
    	{
	    	ctl->cput = ctl->cget = ctl->buf;
	    	ctl->cavail = 0;
	    	ctl->cleft = FTPLIB_BUFSIZ;
    	}
		if (eof)
		{
	    	if (retval == 0)
			retval = -1;
	    	break;
		}

		if (!socket_wait(ctl)) return retval;

   		/*if (ctl->tlsdata) x = SSL_read(ctl->ssl, ctl->cput, ctl->cleft);
    	else
    	{
			if (ctl->tlsctrl) x = SSL_read(ctl->ssl, ctl->cput, ctl->cleft);
        	else x = net_read(ctl->handle,ctl->cput,ctl->cleft);
    	}*/
		x = net_read(ctl->handle,ctl->cput,ctl->cleft);
		if ( x == -1)
        {
	    	perror("read");
	    	retval = -1;
	    	break;
    	}

		// LOGGING FUNCTIONALITY!!!

		if ((ctl->dir == FTPLIB_CONTROL) && (mp_netbuf->logcb != NULL))
		{
            *((ctl->cput)+x) = '\0';
            mp_netbuf->logcb(ctl->cput, mp_netbuf->idlearg);
		}

		if (x == 0) eof = 1;
    	ctl->cleft -= x;
    	ctl->cavail += x;
    	ctl->cput += x;
    } while (1);
    return retval;
}

/*
 * write lines of text
 *
 * return -1 on error or bytecount
 */
int ftplib::writeline(char *buf, int len, netbuf *nData)
{
    int x, nb=0, w;
    char *ubp = buf, *nbp;
    char lc=0;

    if (nData->dir != FTPLIB_WRITE)
	return -1;
    nbp = nData->buf;
    for (x=0; x < len; x++)
    {
	if ((*ubp == '\n') && (lc != '\r'))
	{
	    if (nb == FTPLIB_BUFSIZ)
	    {

              if (!socket_wait(nData)) return x;

                /*if (nData->tlsctrl) w = SSL_write(nData->ssl, nbp, FTPLIB_BUFSIZ);
                else w = net_write(nData->handle, nbp, FTPLIB_BUFSIZ);*/
			  w = net_write(nData->handle, nbp, FTPLIB_BUFSIZ);

		if (w != FTPLIB_BUFSIZ)
		{
		    printf("net_write(1) returned %d, errno = %d\n", w, errno);
		    return(-1);
		}
		nb = 0;
	    }
	    nbp[nb++] = '\r';
	}
	if (nb == FTPLIB_BUFSIZ)
	{
	    if (!socket_wait(nData))
		return x;

	   /* if (nData->tlsctrl) w = SSL_write(nData->ssl, nbp, FTPLIB_BUFSIZ);
           else w = net_write(nData->handle, nbp, FTPLIB_BUFSIZ);*/
		net_write(nData->handle, nbp, FTPLIB_BUFSIZ);
	    if (w != FTPLIB_BUFSIZ)
	    {
		printf("net_write(2) returned %d, errno = %d\n", w, errno);
		return(-1);
	    }
	    nb = 0;
	}
	nbp[nb++] = lc = *ubp++;
    }
    if (nb)
    {
	if (!socket_wait(nData))
	    return x;
       /* if (nData->tlsctrl) w = SSL_write(nData->ssl, nbp, nb);
        else w = net_write(nData->handle, nbp, nb);*/
	w = net_write(nData->handle, nbp, nb);
	if (w != nb)
	{
	    printf("net_write(3) returned %d, errno = %d\n", w, errno);
	    return(-1);
	}
    }
    return len;
}

/*
 * read a response from the server
 *
 * return 0 if first char doesn't match
 * return 1 if first char matches
 */
int ftplib::readresp(char c, netbuf *nControl)
{
    char match[5];
	// wiso: dirty hack 
	//Sleep(1000);
    if (readline(nControl->response,256,nControl) == -1)
    {
	perror("Control socket read failed");
	//DPf_H("Response error: %s",nControl->response);
	return 0;
    }   
	//DPf_H("Response: %s",nControl->response);
    //if (ftplib_debug > 1)
	//fprintf(stderr,"%s",nControl->response);
    if (nControl->response[3] == '-')
    {
	    strncpy(match,nControl->response,3);
	    match[3] = ' ';
	    match[4] = '\0';
	    do
	    {
	        if (readline(nControl->response,256,nControl) == -1)
	        {
		        perror("Control socket read failed");
		        return 0;
	        }
			//DPf_H("Response2: %s",nControl->response);
	        //if (ftplib_debug > 1)
		    //fprintf(stderr,"%s",nControl->response);
	    } while (strncmp(nControl->response,match,4));
    }
    if (nControl->response[0] == c) return 1;
    return 0;
}

/*
 * FtpLastResponse - return a pointer to the last response received
 */
char* ftplib::LastResponse()
{
    if ((mp_netbuf) && (mp_netbuf->dir == FTPLIB_CONTROL))
    	return mp_netbuf->response;
    return NULL;
}

/*
 * ftplib::Connect - connect to remote server
 *
 * return 1 if connected, 0 if not
 */
int ftplib::Connect(const char *host,int port)
{
    int sControl;
    struct sockaddr_in sin;
    struct hostent *phe;
    struct servent *pse;
    int on=1;
    int ret; // new
    //netbuf *ctrl;
    char *lhost;
    char *pnum;

    memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(host);
	sin.sin_port = htons(port);
 //   lhost = strdup(host);
 //   pnum = strchr(lhost,':');
 //   if (pnum == NULL)
 //   {
 //   	/*if ((pse = getservbyname("ftp","tcp")) == NULL)
 //   	{
	//    perror("getservbyname");
	//    return 0;
 //   	}
 //   	sin.sin_port = pse->s_port;*/
	//	sin.sin_port = 21;
 //   }
 //   else
 //   {
	//*pnum++ = '\0';
	//if (isdigit(*pnum)) sin.sin_port = htons(atoi(pnum));
	//else
	//{
	//    /*pse = getservbyname(pnum,"tcp");
	//    sin.sin_port = pse->s_port;*/
	//	sin.sin_port = 21;
	//}
 //   }
	
    //ret = inet_aton(lhost, &sin.sin_addr); // new
    //if (ret == 0) // new
    ////if ((sin.sin_addr.s_addr = inet_addr(lhost)) == -1)
    //{
    //	if ((phe = gethostbyname(lhost)) == NULL)
    //	{
	   // perror("gethostbyname");
	   // return 0;
    //	}
    //	memcpy((char *)&sin.sin_addr, phe->h_addr, phe->h_length);
    //}
    //free(lhost);
    //sControl = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sControl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sControl == -1)
    {
	perror("socket");
	return 0;
    }
	//int t = 45;
	//setsockopt(sControl,SOL_SOCKET, SO_SNDTIMEO, (const char *)&t,sizeof(int) );
	//setsockopt(sControl,SOL_SOCKET, SO_RCVTIMEO, (const char *)&t,sizeof(int) );
    if (setsockopt(sControl,SOL_SOCKET,SO_REUSEADDR,
		   SETSOCKOPT_OPTVAL_TYPE &on, sizeof(on)) == -1)
    {
	//perror("setsockopt");
	//net_close(sControl);
	//return 0;
    }
    if (connect(sControl, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
	perror("connect");
	net_close(sControl);
	return 0;
    }
    /*ctrl = static_cast<netbuf *>(calloc(1,sizeof(netbuf)));
    if (ctrl == NULL)
    {
        perror("calloc");
        net_close(sControl);
        return 0;
    }
    ctrl->buf = static_cast<char *>(malloc(FTPLIB_BUFSIZ));
    if (ctrl->buf == NULL)
    {
	    perror("calloc");
	    net_close(sControl);
	    free(ctrl);
	    return 0;
    }
    */
    mp_netbuf->handle = sControl;
    /*ctrl->dir = FTPLIB_CONTROL;
    ctrl->ctrl = NULL;
	//if ((ctrl->cmode != FTPLIB_PASSIVE) || (ctrl->cmode != FTPLIB_PORT)) ctrl->cmode = FTPLIB_DEFMODE;
    ctrl->cmode = FTPLIB_DEFMODE;
	ctrl->idlecb = NULL;
    ctrl->idletime.tv_sec = ctrl->idletime.tv_usec = 0;
    ctrl->idlearg = NULL;
    ctrl->xfered = 0;
    ctrl->xfered1 = 0;
    ctrl->cbbytes = 0;
    */
    //if (readresp('2', ctrl) == 0)

	
    if (readresp('2', mp_netbuf) == 0)
    {
		net_close(sControl);
		////free(ctrl->buf);
		////free(ctrl);
		return 0;
    }
    //mp_netbuf = ctrl;
    
    return 1;
}

/*
 * FtpOptions - change connection options
 *
 * returns 1 if successful, 0 on error
 */
/*int ftplib::Options(int opt, long val)
{
    int v,rv=0;
    switch (opt)
    {
      case FTPLIB_CONNMODE:
	v = (int) val;
	if ((v == FTPLIB_PASSIVE) || (v == FTPLIB_PORT))
	{
	    mp_netbuf->cmode = v;
	    rv = 1;
	}
	break;
      case FTPLIB_CALLBACK:
	mp_netbuf->idlecb = (FtpCallback) val;
	rv = 1;
	break;
      case FTPLIB_IDLETIME:
	v = (int) val;
	rv = 1;
	mp_netbuf->idletime.tv_sec = v / 1000;
	mp_netbuf->idletime.tv_usec = (v % 1000) * 1000;
	break;
      case FTPLIB_CALLBACKARG:
	rv = 1;
	mp_netbuf->idlearg = (void *) val;
	break;
      case FTPLIB_CALLBACKBYTES:
        rv = 1;
        mp_netbuf->cbbytes = (int) val;
        break;
    }
    return rv;
}*/

/*
 * FtpSendCmd - send a command and wait for expected response
 *
 * return 1 if proper response received, 0 otherwise
 */
int ftplib::FtpSendCmd(const char *cmd, char expresp, netbuf *nControl)
{
    char buf[256];
    int x;

    if (nControl->dir != FTPLIB_CONTROL) return 0;
    //if (ftplib_debug > 2) fprintf(stderr,"%s\n",cmd);
    //if ((strlen(cmd) + 3) > sizeof(buf)) return 0;
    sprintf(buf,"%s\r\n",cmd);

    /*if (nControl->tlsctrl) x = SSL_write(nControl->ssl,buf,strlen(buf));
    else x = net_write(nControl->handle,buf,strlen(buf));*/
	x = net_write(nControl->handle,buf,strlen(buf));
	//DPf_H("Send: %s",buf);

    if (x <= 0)
    {
	    perror("write");
	    return 0;
    }
    return readresp(expresp, nControl);
}

/*
 * FtpLogin - log in to remote server
 *
 * return 1 if logged in, 0 otherwise
 */
int ftplib::Login(const char *user, const char *pass)
{
    char tempbuf[64];

    if (((strlen(user) + 7) > sizeof(tempbuf)) ||
        ((strlen(pass) + 7) > sizeof(tempbuf)))
        return 0;
    sprintf(tempbuf,"USER %s",user);
    if (!FtpSendCmd(tempbuf,'3',mp_netbuf))
    {
		if (mp_netbuf->response[0] != '2')
			return 0;
		else
			return 1;
    }
    sprintf(tempbuf,"PASS %s",pass);
    return FtpSendCmd(tempbuf,'2',mp_netbuf);
}

/*
 * FtpAcceptConnection - accept connection from server
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::FtpAcceptConnection(netbuf *nData, netbuf *nControl)
{
    int sData;
    struct sockaddr addr;
    int l;
    int i;
    struct timeval tv;
    fd_set mask;
    int rv = 0;

    FD_ZERO(&mask);
    FD_SET(nControl->handle, &mask);
    FD_SET(nData->handle, &mask);
    tv.tv_usec = 0;
    tv.tv_sec = ACCEPT_TIMEOUT;
    i = nControl->handle;
    if (i < nData->handle) i = nData->handle;
    i = select(i+1, &mask, NULL, NULL, &tv);

	if (i == -1)
    {
        strncpy(nControl->response, strerror(errno),
                sizeof(nControl->response));
        net_close(nData->handle);
        nData->handle = 0;
        rv = 0;
    }
    else if (i == 0)
    {
	    strcpy(nControl->response, "timed out waiting for connection");
	    net_close(nData->handle);
	    nData->handle = 0;
	    rv = 0;
    }
    else
    {
	    if (FD_ISSET(nData->handle, &mask))
	    {
	        l = sizeof(addr);
	        sData = accept(nData->handle, &addr, &l);
	        i = errno;
	        net_close(nData->handle);
	        if (sData > 0)
	        {
		        rv = 1;
		        nData->handle = sData;
                nData->ctrl = nControl;
	        }
	        else
	        {
		        strncpy(nControl->response, strerror(i), sizeof(nControl->response));
		        nData->handle = 0;
		        rv = 0;
	        }
	    }
	    else if (FD_ISSET(nControl->handle, &mask))
	    {
	        net_close(nData->handle);
	        nData->handle = 0;
	        readresp('2', nControl);
	        rv = 0;
	    }
    }
    return rv;
}

/*
 * FtpAccess - return a handle for a data stream
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::FtpAccess(const char *path, int typ, int mode, netbuf **nData)
{
	return FtpAccess(path, typ, mode, mp_netbuf, nData);
}

int ftplib::FtpAccess(const char *path, int typ, int mode, netbuf *nControl, netbuf **nData)
//int ftplib::FtpAccess(const char *path, int typ, int mode)
{
    char buf[256];
    int dir, ret;
    if ((path == NULL) && ((typ == FTPLIB_FILE_WRITE)
        || (typ == FTPLIB_FILE_READ)
        || (typ == FTPLIB_FILE_READ_APPEND)
        || (typ == FTPLIB_FILE_WRITE_APPEND)))	
    {
		sprintf(nControl->response,"Missing path argument for file transfer\n");
		return 0;
    }
    sprintf(buf, "TYPE %c", mode);
    if (!FtpSendCmd(buf, '2', nControl)) return 0;
    
    switch (typ)
    {
    case FTPLIB_DIR:
		strcpy(buf,"NLST");
		dir = FTPLIB_READ;
		break;
    case FTPLIB_DIR_VERBOSE:
		strcpy(buf,"LIST -aL");
		dir = FTPLIB_READ;
		break;
    case FTPLIB_FILE_READ_APPEND:
    case FTPLIB_FILE_READ:
		strcpy(buf,"RETR");
		dir = FTPLIB_READ;
		break;
    case FTPLIB_FILE_WRITE_APPEND:
    case FTPLIB_FILE_WRITE:
		strcpy(buf,"STOR");
		dir = FTPLIB_WRITE;
		break;
    default:
		sprintf(nControl->response, "Invalid open type %d\n", typ);
		return 0;
    }
    if (path != NULL)
    {
        int i = strlen(buf);
        buf[i++] = ' ';
        if ((strlen(path) + i) >= sizeof(buf)) return 0;
        strcpy(&buf[i],path);
    }
    if (nControl->cmode == ftplib::pasv)
    {
    	if (FtpOpenPasv(nControl, nData, mode, dir, buf) == -1) return 0;
    }
    if (nControl->cmode == ftplib::port)
    {
		if (FtpOpenPort(nControl, nData, mode, dir, buf) == -1) return 0;
		if (!FtpAcceptConnection(*nData,nControl))
	    {
	        FtpClose(*nData);
	        *nData = NULL;
	        return 0;
	    }
    }
    if (nControl->tlsdata)
    {
        /*(*nData)->ssl = SSL_new(nControl->ctx); */
	/*(*nData)->sbio = BIO_new_socket((*nData)->handle, BIO_NOCLOSE);*/
        /*SSL_set_bio((*nData)->ssl,(*nData)->sbio,(*nData)->sbio);
        ret = SSL_connect((*nData)->ssl);*/
        if (ret != 1) return 0;
        (*nData)->tlsdata = 1;    
    }

    return 1;
}

/*
 * FtpOpenPort - Establishes a PORT connection for data transfer
 *
 * return 1 if successful, -1 otherwise
 */
int ftplib::FtpOpenPort(netbuf *nControl, netbuf **nData, int mode, int dir, char *cmd)
{
    int sData;
    union {
	struct sockaddr sa;
	struct sockaddr_in in;
    } sin;
    struct linger lng = { 0, 0 };
    int l;
    int on=1;
    netbuf *ctrl;
    char buf[256];

    if (nControl->dir != FTPLIB_CONTROL)
	return -1;
    if ((dir != FTPLIB_READ) && (dir != FTPLIB_WRITE))
    {
		sprintf(nControl->response, "Invalid direction %d\n", dir);
		return -1;
    }
    if ((mode != ftplib::ascii) && (mode != ftplib::image))
    {
		sprintf(nControl->response, "Invalid mode %c\n", mode);
		return -1;
    }
    l = sizeof(sin);

	if (getsockname(nControl->handle, &sin.sa, &l) < 0)
	{
        perror("getsockname");
        return -1;
	}

    sData = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (sData == -1)
    {
        perror("socket");
        return -1;
    }
    if (setsockopt(sData,SOL_SOCKET,SO_REUSEADDR, SETSOCKOPT_OPTVAL_TYPE &on,sizeof(on)) == -1)
    {
        perror("setsockopt");
        net_close(sData);
        return -1;
    }
    if (setsockopt(sData,SOL_SOCKET,SO_LINGER, SETSOCKOPT_OPTVAL_TYPE &lng,sizeof(lng)) == -1)
    {
        perror("setsockopt");
        net_close(sData);
        return -1;
    }

	sin.in.sin_port = 0;
	if (bind(sData, &sin.sa, sizeof(sin)) == -1)
	{
	    perror("bind");
	    net_close(sData);
	    return -1;
    }
	if (listen(sData, 1) < 0)
    {
        perror("listen");
	    net_close(sData);
	    return -1;
	}
	if (getsockname(sData, &sin.sa, &l) < 0) return 0;
	sprintf(buf, "PORT %d,%d,%d,%d,%d,%d",
    	(unsigned char) sin.sa.sa_data[2],
		(unsigned char) sin.sa.sa_data[3],
		(unsigned char) sin.sa.sa_data[4],
		(unsigned char) sin.sa.sa_data[5],
		(unsigned char) sin.sa.sa_data[0],
		(unsigned char) sin.sa.sa_data[1]);
	if (!FtpSendCmd(buf,'2',nControl))
	{
		net_close(sData);
		return -1;
	}

    if (mp_netbuf->offset != 0)
    {
	char buf[256];
	sprintf(buf,"REST %d", mp_netbuf->offset);
	if (!FtpSendCmd(buf,'3',nControl))
	{
	    net_close(sData);
	    return 0;
	}
    }

    ctrl = static_cast<netbuf*>(calloc(1,sizeof(netbuf)));
    if (ctrl == NULL)
    {
        perror("calloc");
	    net_close(sData);
	    return -1;
    }
    if ((mode == 'A') && ((ctrl->buf = static_cast<char*>(malloc(FTPLIB_BUFSIZ))) == NULL))
    {
		perror("calloc");
		net_close(sData);
		free(ctrl);
		return -1;
    }

	if (!FtpSendCmd(cmd, '1', nControl))
    {
		FtpClose(*nData);
		*nData = NULL;
		return -1;
    }
    
    ctrl->handle = sData;
    ctrl->dir = dir;
    ctrl->ctrl = (nControl->cmode == ftplib::pasv) ? nControl : NULL;
    ctrl->idletime = nControl->idletime;
    ctrl->idlearg = nControl->idlearg;
    ctrl->xfered = 0;
    ctrl->xfered1 = 0;
    ctrl->cbbytes = nControl->cbbytes;
    if (ctrl->idletime.tv_sec || ctrl->idletime.tv_usec || ctrl->cbbytes) ctrl->idlecb = nControl->idlecb;
    else ctrl->idlecb = NULL;
    *nData = ctrl;

    return 1;
}

/*
 * FtpOpenPasv - Establishes a PASV connection for data transfer
 *
 * return 1 if successful, -1 otherwise
 */
int ftplib::FtpOpenPasv(netbuf *nControl, netbuf **nData, int mode, int dir, char *cmd)
{
	int sData;
    union {
		struct sockaddr sa;
		struct sockaddr_in in;
    } sin;
    struct linger lng = { 0, 0 };
    unsigned int l;
    int on=1;
    netbuf *ctrl;
    char *cp;
    unsigned int v[6];

    int ret;
	
    if (nControl->dir != FTPLIB_CONTROL) return -1;
    if ((dir != FTPLIB_READ) && (dir != FTPLIB_WRITE))
    {
		sprintf(nControl->response, "Invalid direction %d\n", dir);
		return -1;
    }
    if ((mode != ftplib::ascii) && (mode != ftplib::image))
    {
		sprintf(nControl->response, "Invalid mode %c\n", mode);
		return -1;
    }
	
    l = sizeof(sin);

    memset(&sin, 0, l);
    sin.in.sin_family = AF_INET;
    if (!FtpSendCmd("PASV",'2',nControl)) return -1;
    cp = strchr(nControl->response,'(');
    if (cp == NULL) return -1;
    cp++;
    sscanf(cp,"%u,%u,%u,%u,%u,%u",&v[2],&v[3],&v[4],&v[5],&v[0],&v[1]);
    sin.sa.sa_data[2] = v[2];
    sin.sa.sa_data[3] = v[3];
    sin.sa.sa_data[4] = v[4];
    sin.sa.sa_data[5] = v[5];
    sin.sa.sa_data[0] = v[0];
    sin.sa.sa_data[1] = v[1];

    if (mp_netbuf->offset != 0)
    {
        char buf[256];
        sprintf(buf,"REST %d",mp_netbuf->offset);
        if (!FtpSendCmd(buf,'3',nControl)) return 0;
    }
    
    sData = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (sData == -1)
    {
        perror("socket");
        return -1;
    }
    if (setsockopt(sData,SOL_SOCKET,SO_REUSEADDR, SETSOCKOPT_OPTVAL_TYPE &on,sizeof(on)) == -1)
    {
        perror("setsockopt");
        net_close(sData);
        return -1;
    }
    if (setsockopt(sData,SOL_SOCKET,SO_LINGER, SETSOCKOPT_OPTVAL_TYPE &lng,sizeof(lng)) == -1)
    {
        perror("setsockopt");
        net_close(sData);
        return -1;
    }
    
    if (nControl->dir != FTPLIB_CONTROL) return -1;
    sprintf(cmd,"%s\r\n",cmd);
    /*if (nControl->tlsctrl) ret = SSL_write(nControl->ssl,cmd,strlen(cmd));
    else ret = net_write(nControl->handle,cmd,strlen(cmd));*/
	ret = net_write(nControl->handle,cmd,strlen(cmd));
    if (ret <= 0)
    {
	   	perror("write");
	   	return -1;
    }
    
	if (connect(sData, &sin.sa, sizeof(sin.sa)) == -1)
    {
    	perror("connect");
        net_close(sData);
        return -1;
    }
	if (!readresp('1', nControl))
	{
        net_close(sData);
		return -1;
	}
    ctrl = static_cast<netbuf*>(calloc(1,sizeof(netbuf)));
    if (ctrl == NULL)
    {
        perror("calloc");
	    net_close(sData);
	    return -1;
    }
    if ((mode == 'A') && ((ctrl->buf = static_cast<char*>(malloc(FTPLIB_BUFSIZ))) == NULL))
    {
		perror("calloc");
		net_close(sData);
		free(ctrl);
		return -1;
    }
    ctrl->handle = sData;
    ctrl->dir = dir;
    ctrl->ctrl = (nControl->cmode == ftplib::pasv) ? nControl : NULL;
    ctrl->idletime = nControl->idletime;
    ctrl->idlearg = nControl->idlearg;
    ctrl->xfered = 0;
    ctrl->xfered1 = 0;
    ctrl->cbbytes = nControl->cbbytes;
    if (ctrl->idletime.tv_sec || ctrl->idletime.tv_usec || ctrl->cbbytes) ctrl->idlecb = nControl->idlecb;
	else ctrl->idlecb = NULL;
    *nData = ctrl;

    return 1;
}

/*
 * FtpClose - close a data connection
 */
int ftplib::FtpClose(netbuf *nData)
{
  netbuf *ctrl;
    if (nData->dir == FTPLIB_WRITE)
    {
	if (nData->buf != NULL)
	    writeline(NULL, 0, nData);
    }
    else if (nData->dir != FTPLIB_READ)
	return 0;
    if (nData->buf)
    	free(nData->buf);
    shutdown(nData->handle,2);
    net_close(nData->handle);

	ctrl = nData->ctrl;
    /*SSL_free(nData->ssl);*/
	free(nData);
    if (ctrl) return readresp('2', ctrl);
    return 1;
}

/*
 * FtpRead - read from a data connection
 */
int ftplib::FtpRead(void *buf, int max, netbuf *nData)
{
    int i;

    if (nData->dir != FTPLIB_READ)
	return 0;
    if (nData->buf)
        i = readline(static_cast<char*>(buf), max, nData);
    else
    {
        socket_wait(nData);
        i = socket_wait(nData); // 3.1-1
		if (i != 1) return 0; // 3.1-1
        /*if (nData->tlsdata) i = SSL_read(nData->ssl, buf, max);
        else i = net_read(nData->handle,buf,max);*/
        i = net_read(nData->handle, buf, max);
    }
	if (i == -1) return 0; // 3.1-1
    nData->xfered += i;
    if (nData->idlecb && nData->cbbytes)
    {
        nData->xfered1 += i;
        if (nData->xfered1 > nData->cbbytes)
        {
            //nData->idlecb(nData, nData->xfered, nData->idlearg);
            if (nData->idlecb(nData->xfered, nData->idlearg) == 0) return 0; // 3.1-1
			nData->xfered1 = 0;
        }
    }
    return i;
}

/*
 * FtpWrite - write to a data connection
 */
int ftplib::FtpWrite(void *buf, int len, netbuf *nData)
{
    int i;
    if (nData->dir != FTPLIB_WRITE)
	return 0;
    if (nData->buf)
    	i = writeline(static_cast<char*>(buf), len, nData);
    else
    {
        socket_wait(nData);
       /* if (nData->tlsdata) i = SSL_write(nData->ssl, buf, len);
        else i = net_write(nData->handle, buf, len);*/
		i = net_write(nData->handle, buf, len);
    }
	if (i == -1) return 0; // 3.1-1
    nData->xfered += i;
    if (nData->idlecb && nData->cbbytes)
    {
        nData->xfered1 += i;
        if (nData->xfered1 > nData->cbbytes)
        {
            if (nData->idlecb(nData->xfered, nData->idlearg) == 0) return 0;
            nData->xfered1 = 0;
        }
    }
    return i;
}

/*
 * FtpSite - send a SITE command
 *
 * return 1 if command successful, 0 otherwise
 */
int ftplib::Site(const char *cmd)
{
    char buf[256];

    if ((strlen(cmd) + 7) > sizeof(buf))
        return 0;
    sprintf(buf,"SITE %s",cmd);
    if (!FtpSendCmd(buf,'2',mp_netbuf))
	return 0;
    return 1;
}

/*
 * FtpRaw - send a raw string string
 *
 * return 1 if command successful, 0 otherwise
 */

int ftplib::Raw(const char *cmd)
{
    char buf[256];
    strncpy(buf, cmd, 256);
    if (!FtpSendCmd(buf,'2',mp_netbuf)) return 0;
    return 1;
}

/*
 * FtpSysType - send a SYST command
 *
 * Fills in the user buffer with the remote system type.  If more
 * information from the response is required, the user can parse
 * it out of the response buffer returned by FtpLastResponse().
 *
 * return 1 if command successful, 0 otherwise
 */
int ftplib::SysType(char *buf, int max)
{
    int l = max;
    char *b = buf;
    char *s;
    if (!FtpSendCmd("SYST",'2',mp_netbuf))
	return 0;
    s = &mp_netbuf->response[4];
    while ((--l) && (*s != ' '))
	*b++ = *s++;
    *b++ = '\0';
    return 1;
}

/*
 * FtpMkdir - create a directory at server
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Mkdir(const char *path)
{
    char buf[256];

    if ((strlen(path) + 6) > sizeof(buf))
        return 0;
    sprintf(buf,"MKD %s",path);
    if (!FtpSendCmd(buf,'2', mp_netbuf))
	return 0;
    return 1;
}

/*
 * FtpChdir - change path at remote
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Chdir(const char *path)
{
    char buf[256];

    if ((strlen(path) + 6) > sizeof(buf))
        return 0;
    sprintf(buf,"CWD %s",path);
    if (!FtpSendCmd(buf,'2',mp_netbuf))
	return 0;
    return 1;
}

/*
 * FtpCDUp - move to parent directory at remote
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Cdup()
{
    if (!FtpSendCmd("CDUP",'2',mp_netbuf))
	return 0;
    return 1;
}

/*
 * FtpRmdir - remove directory at remote
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Rmdir(const char *path)
{
    char buf[256];

    if ((strlen(path) + 6) > sizeof(buf))
        return 0;
    sprintf(buf,"RMD %s",path);
    if (!FtpSendCmd(buf,'2',mp_netbuf)) return 0;
    return 1;
}

/*
 * FtpPwd - get working directory at remote
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Pwd(char *path, int max)
{
    int l = max;
    char *b = path;
    char *s;
    if (!FtpSendCmd("PWD",'2',mp_netbuf))
	return 0;
    s = strchr(mp_netbuf->response, '"');
    if (s == NULL)
	return 0;
    s++;
    while ((--l) && (*s) && (*s != '"'))
	*b++ = *s++;
    *b++ = '\0';
    return 1;
}

/*
 * FtpXfer - issue a command and transfer data
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::FtpXfer(const char *localfile, const char *path, netbuf *nControl, int typ, int mode)
{
    int l,c;
    char *dbuf;
    FILE *local = NULL;
    netbuf *nData;
    int rv=1; // 3.1-1
    if (localfile != NULL)
    {
       // local = fopen(localfile, (typ == FTPLIB_FILE_WRITE) ? "r" : "w");
        char ac[4] = "w";
        if (typ == FTPLIB_FILE_WRITE_APPEND) ac[0] = 'r';    
        if (typ == FTPLIB_FILE_READ_APPEND) ac[0] = 'a';
		if (typ == FTPLIB_FILE_WRITE) ac[0] = 'r';
 		if (mode == ftplib::image) ac[1] = 'b';

        local = fopen(localfile, ac);
        if (typ == FTPLIB_FILE_WRITE_APPEND) fseek(local,mp_netbuf->offset,SEEK_SET);
        if (local == NULL)
		{
			strncpy(nControl->response, strerror(errno), sizeof(nControl->response));
			return 0;
		}
    }
    if (local == NULL) local = ((typ == FTPLIB_FILE_WRITE) || (typ == FTPLIB_FILE_WRITE_APPEND)) ? stdin : stdout;
    if (!FtpAccess(path, typ, mode, nControl, &nData)) return 0;
    dbuf = static_cast<char*>(malloc(FTPLIB_BUFSIZ));
    if ((typ == FTPLIB_FILE_WRITE) || (typ == FTPLIB_FILE_WRITE_APPEND))
    {
	while ((l = fread(dbuf, 1, FTPLIB_BUFSIZ, local)) > 0)
	    if ((c = FtpWrite(dbuf, l, nData)) < l)
		//printf("short write: passed %d, wrote %d\n", l, c);
	    {                   // 3.1-1
  		printf("short write: passed %d, wrote %d\n", l, c);
 		rv = 0;
 		break;
	    }
    }
    else
    {
    	while ((l = FtpRead(dbuf, FTPLIB_BUFSIZ, nData)) > 0)
	    if (fwrite(dbuf, 1, l, local) <= 0)
	    {
		perror("localfile write");
		break;
	    }
    }
    free(dbuf);
    fflush(local);
    if (localfile != NULL)
    fclose(local);
    //return FtpClose(nData);  3.1-1
    FtpClose(nData);
    return rv;
}

/*
 * FtpNlst - issue an NLST command and write response to output
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Nlst(const char *outputfile, const char *path)
{
    mp_netbuf->offset = 0;
    return FtpXfer(outputfile, path, mp_netbuf, FTPLIB_DIR, ftplib::ascii);
}

/*
 * FtpDir - issue a LIST command and write response to output
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Dir(const char *outputfile, const char *path)
{
    mp_netbuf->offset = 0;
	return FtpXfer(outputfile, path, mp_netbuf, FTPLIB_DIR_VERBOSE, ftplib::ascii);
}

//int ftplib::D2XDir(vector<ftp_dir> &dir_list, const char *path)
int ftplib::D2XDir(ftp_dir &dir_list, const char *path)
{
    mp_netbuf->offset = 0;
	//return FtpXfer(outputfile, path, mp_netbuf, FTPLIB_DIR_VERBOSE, ftplib::ascii);

	int l,c;
    char *dbuf;
    netbuf *nData;
    int rv=1; // 3.1-1
	int typ = FTPLIB_DIR_VERBOSE;
	int mode = ftplib::ascii;
	int ret;
	char filename[1024];
	char dir[10];
	//ftp_dir temp;
	//dir_list.clear();
  
	//DPf_H("LS for %s",path);
    if (!FtpAccess(path, typ, mode, mp_netbuf, &nData)) return 0;
    dbuf = static_cast<char*>(malloc(FTPLIB_BUFSIZ));
  
    while ((l = FtpRead(dbuf, FTPLIB_BUFSIZ, nData)) > 0)
	{
		memset(filename,0,strlen(filename));
		ret = sscanf(dbuf, "%[-drwxst] %*[0-9] %*[0-9.a-zA-Z_] %*[0-9.a-zA-Z_] %*[0-9] %*12[A-Za-z0-9: ]%*1[ ]%[^\n]",dir, filename);
		if(ret == 2)
		{
			if(!strcmp(filename,".") || !strncmp(filename,"..",2))
				continue;
			//DPf_H("Scanned %d entries, file (%s)",ret,filename);
			/*temp.filename = string(filename);*/
			dir_list.filename.push_back(string(filename));
			if(!strncmp(dir,"d",1))
			{
				//temp.directory = true;	
				dir_list.directory.push_back(true);
			}
			else
			{
				//temp.directory = false;	
				dir_list.directory.push_back(false);
			}
			//dir_list.push_back(temp);
			
		}
	 }
    free(dbuf);
    FtpClose(nData);
	//DPf_H("Leaving ftplib");
    return rv;
}

/*
 * FtpSize - determine the size of a remote file
 *
 * return 1 if successful, 0 otherwise
 */
//int ftplib::Size(const char *path, int *size, char mode)
int ftplib::Size(const char *path, int *size, ftplib::ftp mode)
{
    char cmd[256];
    int resp,sz,rv=1;

    if ((strlen(path) + 7) > sizeof(cmd))
        return 0;
    sprintf(cmd, "TYPE %c", mode);
    if (!FtpSendCmd(cmd, '2', mp_netbuf))
	return 0;
    sprintf(cmd,"SIZE %s",path);
    if (!FtpSendCmd(cmd,'2',mp_netbuf))
	rv = 0;
    else
    {
	if (sscanf(mp_netbuf->response, "%d %d", &resp, &sz) == 2)
	    *size = sz;
	else
	    rv = 0;
    }
    return rv;
}

/*
 * FtpModDate - determine the modification date of a remote file
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::ModDate(const char *path, char *dt, int max)
{
    char buf[256];
    int rv = 1;

    if ((strlen(path) + 7) > sizeof(buf))
        return 0;
    sprintf(buf,"MDTM %s",path);
    if (!FtpSendCmd(buf,'2',mp_netbuf))
	rv = 0;
    else
	strncpy(dt, &mp_netbuf->response[4], max);
    return rv;
}

/*
 * FtpGet - issue a GET command and write received data to output
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Get(const char *outputfile, const char *path, ftplib::ftp mode)
{
    mp_netbuf->offset = 0;
    return FtpXfer(outputfile, path, mp_netbuf, FTPLIB_FILE_READ, mode);
}

int ftplib::Get(const char *outputfile, const char *path, ftplib::ftp mode, int offset)
{
    //char buf[256];
    //sprintf(buf,"REST %d",offset);
    //if (!FtpSendCmd(buf,'3',mp_netbuf)) return 0;
    mp_netbuf->offset = offset;
    return FtpXfer(outputfile, path, mp_netbuf, 
    FTPLIB_FILE_READ_APPEND, mode);
}   

/*
 * FtpPut - issue a PUT command and send data from input
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Put(const char *inputfile, const char *path, ftplib::ftp mode)
{
    mp_netbuf->offset = 0;
    return FtpXfer(inputfile, path, mp_netbuf, FTPLIB_FILE_WRITE, mode);
}

int ftplib::Put(const char *inputfile, const char *path, ftplib::ftp mode, int offset)
{
    //char buf[256];
    //sprintf(buf,"REST %d",offset);
    //if (!FtpSendCmd(buf,'3',mp_netbuf)) return 0;
    mp_netbuf->offset = offset;
    return FtpXfer(inputfile, path, mp_netbuf, FTPLIB_FILE_WRITE_APPEND, mode);
}

/*
 * FtpRename - rename a file at remote
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Rename(const char *src, const char *dst)
{
    char cmd[256];

    if (((strlen(src) + 7) > sizeof(cmd)) ||
        ((strlen(dst) + 7) > sizeof(cmd)))
        return 0;
    sprintf(cmd,"RNFR %s",src);
    if (!FtpSendCmd(cmd,'3',mp_netbuf))
	return 0;
    sprintf(cmd,"RNTO %s",dst);
    if (!FtpSendCmd(cmd,'2',mp_netbuf))
	return 0;
    return 1;
}

/*
 * FtpDelete - delete a file at remote
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Delete(const char *fnm)
{
    char cmd[256];

    if ((strlen(fnm) + 7) > sizeof(cmd))
        return 0;
    sprintf(cmd,"DELE %s",fnm);
    if (!FtpSendCmd(cmd,'2', mp_netbuf))
	return 0;
    return 1;
}

/*
 * FtpQuit - disconnect from remote
 *
 * return 1 if successful, 0 otherwise
 */
void ftplib::Quit()
{ 
    if (mp_netbuf->dir != FTPLIB_CONTROL) return;
	if (mp_netbuf->handle == 0)
	{
		strcpy(mp_netbuf->response, "error: no anwser from server\n");
		return;
	}
    FtpSendCmd("QUIT",'2',mp_netbuf);
    net_close(mp_netbuf->handle);  
	mp_netbuf->handle = 0;
    /*if (mp_netbuf->tlsctrl)          
	{
		SSL_free(mp_netbuf->ssl);
		SSL_CTX_free(mp_netbuf->ctx);
		BIO_free(mp_netbuf->sbio);
	}*/
    /*free(mp_netbuf->buf);
    free(mp_netbuf);*/
}

/*
 * FtpFxp - transfer from server to server
 *
 * return 1 if successful, 0 otherwise
 */
int ftplib::Fxp(ftplib* src, ftplib* dst, const char *pathSrc, const char *pathDst, ftplib::ftp mode, ftplib::ftp method)
{
    char *cp;
    unsigned int v[6];
    char buf[256];
	int retval = 0;

	sprintf(buf, "TYPE %c", mode);
	if (!dst->FtpSendCmd(buf,'2',dst->mp_netbuf)) return -1;
    if (!src->FtpSendCmd(buf,'2',src->mp_netbuf)) return -1;

    if (method == ftplib::defaultfxp)
    {
        // PASV dst

        if (!dst->FtpSendCmd("PASV",'2',dst->mp_netbuf)) return -1;
        cp = strchr(dst->mp_netbuf->response,'(');
        if (cp == NULL) return -1;
        cp++;
        sscanf(cp,"%u,%u,%u,%u,%u,%u",&v[2],&v[3],&v[4],&v[5],&v[0],&v[1]);

        // PORT src

        sprintf(buf, "PORT %d,%d,%d,%d,%d,%d", v[2],v[3],v[4],v[5],v[0],v[1]);
        if (!src->FtpSendCmd(buf,'2',src->mp_netbuf)) return -1;

        // RETR src

        strcpy(buf,"RETR");
        if (pathSrc != NULL)
        {
            int i = strlen(buf);
            buf[i++] = ' ';
            if ((strlen(pathSrc) + i) >= sizeof(buf)) return 0;
            strcpy(&buf[i],pathSrc);
        }
        if (!src->FtpSendCmd(buf, '1', src->mp_netbuf)) return 0;

        // STOR dst

        strcpy(buf,"STOR");
        if (pathDst != NULL)
        {
            int i = strlen(buf);
            buf[i++] = ' ';
            if ((strlen(pathDst) + i) >= sizeof(buf)) return 0;
            strcpy(&buf[i],pathDst);
        }
        if (!dst->FtpSendCmd(buf, '1', dst->mp_netbuf))
        {
			if ((!src->FtpSendCmd("ABOR",'2',src->mp_netbuf)) || (!src->readresp('2', src->mp_netbuf))) return -1;
			return 0;
        }

        retval = (src->readresp('2', src->mp_netbuf)) & (dst->readresp('2', dst->mp_netbuf));

        //while(!readresp('2', nControlDst)) sleep(0);
        //while(!readresp('2', nControlSrc)) sleep(0);
    }
    else
    {
        // PASV src

        if (!src->FtpSendCmd("PASV",'2',src->mp_netbuf)) return -1;
        cp = strchr(src->mp_netbuf->response,'(');
        if (cp == NULL) return -1;
        cp++;
        sscanf(cp,"%u,%u,%u,%u,%u,%u",&v[2],&v[3],&v[4],&v[5],&v[0],&v[1]);

        // PORT dst

        sprintf(buf, "PORT %d,%d,%d,%d,%d,%d", v[2],v[3],v[4],v[5],v[0],v[1]);
        if (!dst->FtpSendCmd(buf,'2',dst->mp_netbuf)) return -1;

        // STOR dest

        strcpy(buf,"STOR");
        if (pathDst != NULL)
        {
            int i = strlen(buf);
            buf[i++] = ' ';
            if ((strlen(pathDst) + i) >= sizeof(buf)) return 0;
            strcpy(&buf[i],pathDst);
        }
        if (!dst->FtpSendCmd(buf, '1', dst->mp_netbuf)) return 0;

        // RETR src

        strcpy(buf,"RETR");
        if (pathSrc != NULL)
        {
            int i = strlen(buf);
            buf[i++] = ' ';
            if ((strlen(pathSrc) + i) >= sizeof(buf)) return 0;
            strcpy(&buf[i],pathSrc);
        }
        if (!src->FtpSendCmd(buf, '1', src->mp_netbuf))
        {
			if ((!dst->FtpSendCmd("ABOR",'2',dst->mp_netbuf)) || (!dst->readresp('2', dst->mp_netbuf))) return -1;
            return 0;
        }

		// wait til its finished!

		retval = (src->readresp('2', src->mp_netbuf)) & (dst->readresp('2', dst->mp_netbuf));

    }

    return retval;
}
int ftplib::SetDataEncryption(ftplib::ftp flag)
{
    if (!mp_netbuf->tlsctrl) return 0;
    if (!FtpSendCmd("PBSZ 0",'2',mp_netbuf)) return -1;
	switch(flag)
    {
    case 0:
        mp_netbuf->tlsdata = 0;
        if (!FtpSendCmd("PROT C",'2',mp_netbuf)) return -1;
        break;
    case 1:
        mp_netbuf->tlsdata = 1;
        if (!FtpSendCmd("PROT P",'2',mp_netbuf)) return -1;
        break;
    default:
        return 0;
    }
    return 1;
}

int ftplib::NegotiateEncryption()
{
    int ret;

    if (!FtpSendCmd("AUTH TLS",'2',mp_netbuf)) return -1;

    /*SSL_library_init();
    mp_netbuf->ctx = SSL_CTX_new(SSLv23_method());
    SSL_CTX_set_verify(mp_netbuf->ctx, SSL_VERIFY_NONE, NULL);
    mp_netbuf->ssl = SSL_new(mp_netbuf->ctx);*/
    /*mp_netbuf->sbio = BIO_new_socket(mp_netbuf->handle, BIO_NOCLOSE);
    SSL_set_bio(mp_netbuf->ssl,mp_netbuf->sbio,mp_netbuf->sbio);*/

    /*ret = SSL_connect(mp_netbuf->ssl);
    if (ret == 1) mp_netbuf->tlsctrl = 1;*/

    /*if (ret == -1) {
    switch (SSL_get_error(mp_netbuf->ssl, ret))
    {
        case SSL_ERROR_ZERO_RETURN:
            printf("SSL_ERROR_ZERO_RETURN\n");
            break;
        case SSL_ERROR_WANT_READ:
            printf("SSL_ERROR_WANT_READ\n");
            break;
        case SSL_ERROR_WANT_WRITE:
            printf("SSL_ERROR_WANT_WRITE\n");
            break;
         case SSL_ERROR_WANT_CONNECT:
            printf("SSL_ERROR_WANT_CONNECT\n");
            break;
         case SSL_ERROR_WANT_X509_LOOKUP:
            printf("SSL_ERROR_WANT_X509_LOOKUP\n");
            break;
        case SSL_ERROR_SYSCALL:
            printf("SSL_ERROR_SYSCALL\n");
            perror("error");
            break;
        case SSL_ERROR_SSL:
            printf("SSL_ERROR_SSL\n");
            break;
	default:
	    printf("other crappy error!\n");
    }
    }*/
    
    return ret;
}

/*
void ftplib::SetLog(string *logstring)
{
    //mp_logstring = logstring;
	mp_netbuf->log = logstring;
}
*/

void ftplib::SetCallbackIdleFunction(FtpCallbackIdle pointer)
{
    mp_netbuf->idlecb = pointer;
}

void ftplib::SetCallbackLogFunction(FtpCallbackLog pointer)
{
    mp_netbuf->logcb = pointer;
}

void ftplib::SetCallbackArg(void *arg)
{
    mp_netbuf->idlearg = arg;
}

void ftplib::SetCallbackBytes(long bytes)
{
    mp_netbuf->cbbytes = bytes;
}

void ftplib::SetCallbackIdletime(int time)
{
    mp_netbuf->idletime.tv_sec = time / 1000;
    mp_netbuf->idletime.tv_usec = (time % 1000) * 1000;
}

void ftplib::SetConnmode(ftplib::ftp mode)
{
    if ((mode != ftplib::pasv) && (mode != ftplib::port)) mode = ftplib::defaultconnmode; 
    mp_netbuf->cmode = mode;
}

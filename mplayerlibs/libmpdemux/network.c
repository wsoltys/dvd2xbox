/*
 * Network layer for MPlayer
 * by Bertrand BAUDET <bertrand_baudet@yahoo.com>
 * (C) 2001, MPlayer team.
 */

//#define DUMP2FILE

#ifdef _XBOX
#include <xtl.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

#include "cygwin_inttypes.h"
#include "mp_msg.h"
#include "url.h"
#include "network.h"
#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "url.h"
#include "mp3_hdr.h"
#include "http.h"
#include "../help.orig\help_mp-en.h"

#include "asf.h"
#ifdef STREAMING

#include "../version.h"
#pragma warning (disable:4018)
#pragma warning (disable:4244)
#pragma warning (disable:4305)
#pragma warning (disable:4133)
#pragma warning (disable:4013)
#pragma warning (disable:4047)
#pragma warning (disable:4700)
extern int verbose;
extern int stream_cache_size;

extern int mp_input_check_interrupt(int time);


/* Variables for the command line option -user, -passwd & -bandwidth */
char *network_username=NULL;
char *network_password=NULL;
int   network_bandwidth=0;


static SOCKET sockets[10] ={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

static struct
{
    char *mime_type;
    int demuxer_type;
}
mime_type_table[] = {
                        // MP3 streaming, some MP3 streaming server answer with audio/mpeg
                        { "audio/mpeg", DEMUXER_TYPE_AUDIO },
                        // MPEG streaming
                        { "video/mpeg", DEMUXER_TYPE_MPEG_PS },
                        { "video/x-mpeg", DEMUXER_TYPE_MPEG_PS },
                        { "video/x-mpeg2", DEMUXER_TYPE_MPEG_PS },
                        // AVI ??? => video/x-msvideo
                        { "video/x-msvideo", DEMUXER_TYPE_AVI },
                        // MOV => video/quicktime
                        { "video/quicktime", DEMUXER_TYPE_MOV },
                        // ASF
                        { "audio/x-ms-wax", DEMUXER_TYPE_ASF },
                        { "audio/x-ms-wma", DEMUXER_TYPE_ASF },
                        { "video/x-ms-asf", DEMUXER_TYPE_ASF },
                        { "video/x-ms-afs", DEMUXER_TYPE_ASF },
                        { "video/x-ms-wvx", DEMUXER_TYPE_ASF },
                        { "video/x-ms-wmv", DEMUXER_TYPE_ASF },
                        { "video/x-ms-wma", DEMUXER_TYPE_ASF },
                        // Playlists
                        { "video/x-ms-wmx", DEMUXER_TYPE_PLAYLIST },
                        { "audio/x-scpls", DEMUXER_TYPE_PLAYLIST },
                        { "audio/x-mpegurl", DEMUXER_TYPE_PLAYLIST },
                        { "audio/x-pls", DEMUXER_TYPE_PLAYLIST },
                        // Real Media
                        { "audio/x-pn-realaudio", DEMUXER_TYPE_REAL },
                        // OGG Streaming
                        { "application/x-ogg", DEMUXER_TYPE_OGG }

                    };

static struct
{
    char *extension;
    int demuxer_type;
}
extensions_table[] = {
                         { "mpeg", DEMUXER_TYPE_MPEG_PS },
												 { "vob", DEMUXER_TYPE_MPEG_PS },
                         { "bin", DEMUXER_TYPE_MPEG_PS },
                         { "dat", DEMUXER_TYPE_MPEG_PS },
                         { "mpg", DEMUXER_TYPE_MPEG_PS },
												 { "m2v", DEMUXER_TYPE_MPEG_PS },
												 { "mpa", DEMUXER_TYPE_MPEG_PS },
                         { "mpe", DEMUXER_TYPE_MPEG_ES },
                         { "avi", DEMUXER_TYPE_AVI },
                         { "divx", DEMUXER_TYPE_AVI },
                         { "ogm", DEMUXER_TYPE_OGG },
                         { "ogg", DEMUXER_TYPE_OGG },
                         { "mov", DEMUXER_TYPE_MOV },
                         { "qt", DEMUXER_TYPE_MOV },
                         { "asx", DEMUXER_TYPE_ASF },
                         { "asf", DEMUXER_TYPE_ASF },
                         { "wmv", DEMUXER_TYPE_ASF },
                         { "wma", DEMUXER_TYPE_ASF },
                         { "viv", DEMUXER_TYPE_VIVO },
												 { "pva", DEMUXER_TYPE_PVA },
                         { "rm", DEMUXER_TYPE_REAL },
                         { "ra", DEMUXER_TYPE_REAL },
                         { "y4m", DEMUXER_TYPE_Y4M },
                         { "mp3", DEMUXER_TYPE_AUDIO },
                         { "sc", DEMUXER_TYPE_AUDIO },
                         { "wav", DEMUXER_TYPE_AUDIO },
                         { "pls", DEMUXER_TYPE_PLAYLIST },
                         { "m3u", DEMUXER_TYPE_PLAYLIST }
                     };

int socket_add(SOCKET socket)
{
	int i;
	for (i=0; i < 10; i++)
	{
		if (sockets[i]==NULL)
		{
			sockets[i]=socket;
			return i;
		}
	}
	return -1;
}

SOCKET socket_get(int fd)
{
	return sockets[fd];
}

void socket_close(int fd)
{
	if (sockets[fd]!=NULL)
	{
		closesocket(sockets[fd]);
		sockets[fd]=NULL;
	}
}

int socket_read(int fd, unsigned char* buffer, int iSize)
{
	SOCKET socket=socket_get(fd);
	if (socket==NULL) return -1;
	return recv(socket,buffer,iSize,0);
}

int socket_write(int fd, unsigned char* buffer, int iSize)
{
	SOCKET socket=socket_get(fd);
	if (socket==NULL) return -1;
	return send(socket,buffer,iSize,0);
}



streaming_ctrl_t *streaming_ctrl_new( )
{
    streaming_ctrl_t *streaming_ctrl;
    streaming_ctrl = (streaming_ctrl_t*)malloc(sizeof(streaming_ctrl_t));
    if( streaming_ctrl==NULL )
    {
        mp_msg(MSGT_NETWORK,MSGL_FATAL,"Failed to allocate memory\n");
        return NULL;
    }
    memset( streaming_ctrl, 0, sizeof(streaming_ctrl_t) );
    return streaming_ctrl;
}

void streaming_ctrl_free( streaming_ctrl_t *streaming_ctrl )
{
    if( streaming_ctrl==NULL )
        return;
    if( streaming_ctrl->url )
        url_free( streaming_ctrl->url );
    if( streaming_ctrl->buffer )
        free( streaming_ctrl->buffer );
    if( streaming_ctrl->data )
        free( streaming_ctrl->data );
    free( streaming_ctrl );
}


int http_send_request( URL_t *url )
{
    HTTP_header_t *http_hdr;
    URL_t *server_url;
    char str[80];
    int fd;
    int ret;
    int proxy = 0;		// Boolean

    http_hdr = http_new_header();

    if( !strcmp(url->protocol, "http_proxy") )
    {
        proxy = 1;
        server_url = url_new( (url->file)+1 );
        http_set_uri( http_hdr, server_url->url );
    }
    else
    {
        server_url = url;
        http_set_uri( http_hdr, server_url->file );
    }
    _snprintf(str, 80, "Host: %s", server_url->hostname );
    http_set_field( http_hdr, str);
    http_set_field( http_hdr, "User-Agent: MPlayer/"VERSION);
    http_set_field( http_hdr, "Connection: closed");
    http_add_basic_authentication( http_hdr, url->username, url->password );
    if( http_build_request( http_hdr )==NULL )
    {
        return -1;
    }

    if( proxy )
    {
        if( url->port==0 )
            url->port = 8080;			// Default port for the proxy server
        fd = connect2Server( url->hostname, url->port );
        url_free( server_url );
    }
    else
    {
        if( server_url->port==0 )
            server_url->port = 80;	// Default port for the web server
        fd = connect2Server( server_url->hostname, server_url->port );
    }
    if( fd<0 )
    {
        return -1;
    }
    mp_msg(MSGT_NETWORK,MSGL_DBG2,"Request: [%s]\n", http_hdr->buffer );

    ret = socket_write( fd, http_hdr->buffer, http_hdr->buffer_size );
    if( ret!=(int)http_hdr->buffer_size )
    {
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Error while sending HTTP request: didn't sent all the request\n");
        return -1;
    }

    http_free( http_hdr );

    return fd;
}

HTTP_header_t *http_read_response( int fd )
{
    HTTP_header_t *http_hdr;
    char response[BUFFER_SIZE];
    int i;

    http_hdr = http_new_header();
    if( http_hdr==NULL )
    {
        return NULL;
    }

    do
    {
        i = socket_read( fd, response, BUFFER_SIZE );
        if( i<0 )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"Read failed\n");
            http_free( http_hdr );
            return NULL;
        }
        if( i==0 )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"http_read_response read 0 -ie- EOF\n");
            http_free( http_hdr );
            return NULL;
        }
        http_response_append( http_hdr, response, i );
    }
    while( !http_is_header_entire( http_hdr ) );
    http_response_parse( http_hdr );
    return http_hdr;
}

int http_authenticate(HTTP_header_t *http_hdr, URL_t *url, int *auth_retry)
{
    char *aut;

    if( *auth_retry
            ==1 )
    {
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Authentication failed\n");
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Please use the option -user and -passwd to provide your username/password for a list of URLs,\n");
        mp_msg(MSGT_NETWORK,MSGL_ERR,"or form an URL like: http://username:password@hostname/file\n");
        return -1;
    }
    if( *auth_retry
            >0 )
    {
        if( url->username )
        {
            free( url->username );
            url->username = NULL;
        }
        if( url->password )
        {
            free( url->password );
            url->password = NULL;
        }
    }

    aut = http_get_field(http_hdr, "WWW-Authenticate");
    if( aut!=NULL )
    {
        char *aut_space;
        aut_space = strstr(aut, "realm=");
        if( aut_space!=NULL )
            aut_space += 6;
        mp_msg(MSGT_NETWORK,MSGL_INFO,"Authentication required for %s\n", aut_space);
    }
    else
    {
        mp_msg(MSGT_NETWORK,MSGL_INFO,"Authentication required\n");
    }
    if( network_username )
    {
        url->username = strdup(network_username);
        if( url->username==NULL )
        {
            mp_msg(MSGT_NETWORK,MSGL_FATAL,"Memory allocation failed\n");
            return -1;
        }
    }
    else
    {
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Unable to read the username\n");
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Please use the option -user and -passwd to provide your username/password for a list of URLs,\n");
        mp_msg(MSGT_NETWORK,MSGL_ERR,"or form an URL like: http://username:password@hostname/file\n");
        return -1;
    }
    if( network_password )
    {
        url->password = strdup(network_password);
        if( url->password==NULL )
        {
            mp_msg(MSGT_NETWORK,MSGL_FATAL,"Memory allocation failed\n");
            return -1;
        }
    }
    else
    {
        mp_msg(MSGT_NETWORK,MSGL_INFO,"No password provided, trying blank password\n");
    }
    (*auth_retry
    )++;
    return 0;
}

// By using the protocol, the extension of the file or the content-type
// we might be able to guess the streaming type.
int autodetectProtocol(streaming_ctrl_t *streaming_ctrl, int *fd_out, int *file_format)
{
    HTTP_header_t *http_hdr;
    unsigned int i;
    int fd=-1;
    int redirect;
    int auth_retry
    =0;
    char *extension;
    char *content_type;
    char *next_url;

    URL_t *url = streaming_ctrl->url;
    *file_format = DEMUXER_TYPE_UNKNOWN;

    if( strncmp(url->protocol, "http", 4) && 
				strncmp(url->protocol, "mms" , 3) && 
				strncmp(url->protocol, "mmst", 4) )
    {
				return 0;    
		}

    do
    {
        *fd_out = -1;
        next_url = NULL;
        extension = NULL;
        content_type = NULL;
        redirect = 0;

        if( url==NULL )
        {
            return -1;
        }

        // Checking for ASF
        if( !strncmp(url->protocol, "mms", 3) )
        {
            *file_format = DEMUXER_TYPE_ASF;
            return 0;
        }

        // HTTP based protocol
        if( !strcmp(url->protocol, "http") || !strcmp(url->protocol, "http_proxy") )
        {
            fd = http_send_request( url );
            if( fd<0 )
            {
                return -1;
            }

            http_hdr = http_read_response( fd );
            if( http_hdr==NULL )
            {
                socket_close( fd );
                http_free( http_hdr );
                return -1;
            }

            *fd_out=fd;
            if( verbose>0 )
            {
                http_debug_hdr( http_hdr );
            }

            streaming_ctrl->data = (void*)http_hdr;

            // Check if the response is an ICY status_code reason_phrase
            if( !strcmp(http_hdr->protocol, "ICY") )
            {
                switch( http_hdr->status_code )
                {
                case 200:
                    { // OK
                        char *field_data = NULL;
                        // note: I skip icy-notice1 and 2, as they contain html <BR>
                        // and are IMHO useless info ::atmos
                        if( (field_data = http_get_field(http_hdr, "icy-name")) != NULL )
                            mp_msg(MSGT_NETWORK,MSGL_INFO,"Name   : %s\n", field_data);
                        field_data = NULL;
                        if( (field_data = http_get_field(http_hdr, "icy-genre")) != NULL )
                            mp_msg(MSGT_NETWORK,MSGL_INFO,"Genre  : %s\n", field_data);
                        field_data = NULL;
                        if( (field_data = http_get_field(http_hdr, "icy-url")) != NULL )
                            mp_msg(MSGT_NETWORK,MSGL_INFO,"Website: %s\n", field_data);
                        field_data = NULL;
                        // XXX: does this really mean public server? ::atmos
                        if( (field_data = http_get_field(http_hdr, "icy-pub")) != NULL )
                            mp_msg(MSGT_NETWORK,MSGL_INFO,"Public : %s\n", atoi(field_data)?"yes":"no");
                        field_data = NULL;
                        if( (field_data = http_get_field(http_hdr, "icy-br")) != NULL )
                            mp_msg(MSGT_NETWORK,MSGL_INFO,"Bitrate: %skbit/s\n", field_data);
                        field_data = NULL;
                        // Ok, we have detected an mp3 stream
                        *file_format = DEMUXER_TYPE_AUDIO;
                        return 0;
                    }
                case 400: // Server Full
                    mp_msg(MSGT_NETWORK,MSGL_ERR,"Error: ICY-Server is full, skipping!\n");
                    return -1;
                case 401: // Service Unavailable
                    mp_msg(MSGT_NETWORK,MSGL_ERR,"Error: ICY-Server return service unavailable, skipping!\n");
                    return -1;
                case 403: // Service Forbidden
                    mp_msg(MSGT_NETWORK,MSGL_ERR,"Error: ICY-Server return 'Service Forbidden'\n");
                    return -1;
                case 404: // Resource Not Found
                    mp_msg(MSGT_NETWORK,MSGL_ERR,"Error: ICY-Server couldn't find requested stream, skipping!\n");
                    return -1;
                default:
                    mp_msg(MSGT_NETWORK,MSGL_ERR,"Error: unhandled ICY-Errorcode, contact MPlayer developers!\n");
                    return -1;
                }
            }

            // Assume standard http if not ICY
            switch( http_hdr->status_code )
            {
							case 200: // OK
                // Look if we can use the Content-Type
                content_type = http_get_field( http_hdr, "Content-Type" );
                if( content_type!=NULL )
                {
                    char *content_length = NULL;
                    mp_msg(MSGT_NETWORK,MSGL_V,"Content-Type: [%s]\n", content_type );
                    if( (content_length = http_get_field(http_hdr, "Content-Length")) != NULL)
                        mp_msg(MSGT_NETWORK,MSGL_V,"Content-Length: [%s]\n", http_get_field(http_hdr, "Content-Length"));
                    // Check in the mime type table for a demuxer type
                    for( i=0 ; i<(sizeof(mime_type_table)/sizeof(mime_type_table[0])) ; i++ )
                    {
                        if( !strcmp( content_type, mime_type_table[i].mime_type ) )
                        {
                            *file_format = mime_type_table[i].demuxer_type;
                            return 0;
                        }
                    }
                }
                // Not found in the mime type table, don't fail,
                // we should try raw HTTP
                return 0;
                // Redirect
            case 301: // Permanently
            case 302: // Temporarily
                // TODO: RFC 2616, recommand to detect infinite redirection loops
                next_url = http_get_field( http_hdr, "Location" );
                if( next_url!=NULL )
                {
                    socket_close( fd );
                    url_free( url );
                    streaming_ctrl->url = url = url_new( next_url );
                    http_free( http_hdr );
                    redirect = 1;
                }
                break;
            case 401: // Authentication required
                if( http_authenticate(http_hdr, url, &auth_retry)<0 ) return -1;
                redirect = 1;
                break;
            default:
                mp_msg(MSGT_NETWORK,MSGL_ERR,"Server returned %d: %s\n", http_hdr->status_code, http_hdr->reason_phrase );
                return -1;
            }
        }
        else
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"Unknown protocol '%s'\n", url->protocol );
            return -1;
        }
    }
    while( redirect );

    return -1;
}

int connect2Server(char *host, int port)
{
    int socket_server_fd;
    int count = 0;
    struct sockaddr_in server_address;
    struct hostent *hp=NULL;

		SOCKET newsocket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
		if (newsocket==NULL)
		{
			return -2;
		}

    socket_server_fd = socket_add(newsocket);
    if( socket_server_fd==-1 )
    {
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Failed to create socket\n");
				socket_close(newsocket);
        return -2;
    }

    if( isalpha(host[0]) )
    {
				/*
        mp_msg(MSGT_NETWORK,MSGL_STATUS,"Resolving %s ...\n", host );
        hp=(struct hostent*)gethostbyname( host );
        if( hp==NULL )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"Counldn't resolve name: %s\n", host);
            return -2;
        }
        memcpy( (void*)&server_address.sin_addr.s_addr, (void*)hp->h_addr, hp->h_length );*/
    }
    else
    {
        server_address.sin_addr.s_addr=inet_addr(host);
    }
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);

    if( connect( newsocket, (struct sockaddr*)&server_address, sizeof(server_address) )==SOCKET_ERROR )
    {
			socket_close(socket_server_fd);
      return -1;
    }

    return socket_server_fd;
}


int streaming_bufferize( streaming_ctrl_t *streaming_ctrl, char *buffer, int size)
{
    //mp_msg(0,0,"streaming_bufferize\n");
    streaming_ctrl->buffer = (char*)malloc(size);
    streaming_ctrl->buffer_size =NULL;
    if( streaming_ctrl->buffer==NULL )
    {
        mp_msg(MSGT_NETWORK,MSGL_FATAL,"Memory allocation failed\n");
        return -1;
    }
    if (buffer)
    {
        memcpy( streaming_ctrl->buffer, buffer, size );
        streaming_ctrl->buffer_size = size;
    }
    return size;
}


int streaming_start(stream_t *stream, int *demuxer_type, URL_t *url)
{
    int ret;
    if( stream==NULL )
        return -1;

    stream->streaming_ctrl = streaming_ctrl_new();
    if( stream->streaming_ctrl==NULL )
    {
        return -1;
    }
    stream->streaming_ctrl->url = url ;
    ret = autodetectProtocol( stream->streaming_ctrl, &stream->fd, demuxer_type );
    if( ret<0 )
    {
        return -1;
    }
    ret = -1;

    // Get the bandwidth available
    stream->streaming_ctrl->bandwidth = network_bandwidth;
    // For connection-oriented streams, we can usually determine the streaming type.
    switch( *demuxer_type )
    {
			case DEMUXER_TYPE_ASF:
        // Send the appropriate HTTP request
        // Need to filter the network stream.
        // ASF raw stream is encapsulated.
			
				if (strcmp(stream->streaming_ctrl->url->protocol,"http")==0 )
				{
					ret=asf_streaming_start(stream);
					if( ret<0 )
					{
							mp_msg(MSGT_NETWORK,MSGL_ERR,"asf_streaming_start failed\n");
					}
					return ret;
				}
        ret = xbmp_streaming_start( stream );
        if( ret<0 )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"asf_streaming_start failed\n");
        }
        break;
    case DEMUXER_TYPE_MPEG_ES:
    case DEMUXER_TYPE_PVA:
    case DEMUXER_TYPE_MPEG_PS:
    case DEMUXER_TYPE_AVI:
    case DEMUXER_TYPE_MOV:
    case DEMUXER_TYPE_VIVO:
    case DEMUXER_TYPE_FLI:
    case DEMUXER_TYPE_REAL:
    case DEMUXER_TYPE_Y4M:
    case DEMUXER_TYPE_FILM:
    case DEMUXER_TYPE_ROQ:
    case DEMUXER_TYPE_AUDIO:
    case DEMUXER_TYPE_OGG:
    case DEMUXER_TYPE_PLAYLIST:
    case DEMUXER_TYPE_UNKNOWN:
        // Generic start, doesn't need to filter
        // the network stream, it's a raw stream
        ret = xbmp_streaming_start( stream );
        if( ret<0 )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"xbmp_streaming_start failed\n");
        }
        if((*demuxer_type) == DEMUXER_TYPE_PLAYLIST)
            stream->type = STREAMTYPE_PLAYLIST;
        break;
    default:
        mp_msg(MSGT_NETWORK,MSGL_ERR,"Unable to detect the streaming type\n");
        ret = -1;
    }

    if( ret<0 )
    {
				stream->streaming_ctrl->url=NULL;
        streaming_ctrl_free( stream->streaming_ctrl );
        stream->streaming_ctrl = NULL;
    }
    else if( stream->streaming_ctrl->buffering )
    {
        if(stream_cache_size<0)
        {
            // cache option not set, will use our computed value.
            // buffer in KBytes, *5 because the prefill is 20% of the buffer.
            stream_cache_size = (stream->streaming_ctrl->prebuffer_size/1024)*5;
            if( stream_cache_size<16 )
                stream_cache_size = 16;	// 16KBytes min buffer
        }
        mp_msg(MSGT_NETWORK,MSGL_INFO,"Cache size set to %d KBytes\n", stream_cache_size);
    }

    return ret;
}

int streaming_stop( stream_t *stream )
{
    stream->streaming_ctrl->status = streaming_stopped_e;
    return 0;
}


int xbmp_streaming_read( int fd, char *buffer, int size, streaming_ctrl_t *stream_ctrl )
{
    int len=0;
    if( stream_ctrl->buffer_size!=0 )
    {
        int buffer_len = stream_ctrl->buffer_size-stream_ctrl->buffer_pos;
        len = (size<buffer_len)?size:buffer_len;
        memcpy( buffer, (stream_ctrl->buffer)+(stream_ctrl->buffer_pos), len );
        stream_ctrl->buffer_pos += len;
        if( stream_ctrl->buffer_pos>=stream_ctrl->buffer_size )
        {
            free( stream_ctrl->buffer );
            stream_ctrl->buffer = NULL;
            stream_ctrl->buffer_size = 0;
            stream_ctrl->buffer_pos = 0;
        }
    }

    if( len<size )
    {
        int ret;
        ret = streaming_read( fd, buffer+len, size-len );
        if( ret<0 )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"xbmp_streaming_read error : %s\n",strerror(errno));
        }
        len += ret;
    }

    return len;
}

int xbmp_streaming_seek( int fd, off_t pos, streaming_ctrl_t *stream_ctrl )
{
    if( stream_ctrl->buffer_size!=0 )
    {
        free( stream_ctrl->buffer );
    }
    stream_ctrl->buffer = NULL;
    stream_ctrl->buffer_size = 0;
    stream_ctrl->buffer_pos = 0;

    return streaming_seek(fd,pos);
}

int xbmp_streaming_close( int fd, streaming_ctrl_t *stream_ctrl )
{
    return streaming_close(fd);
}

int nop_streaming_close( int fd, streaming_ctrl_t *stream_ctrl )
{
	socket_close(fd);
	return 1;
}

extern unsigned long streaming_getfilesize(int fd);

int xbmp_streaming_start( stream_t *stream )
{
    int fd;
    if( stream==NULL )
        return -1;
    fd = stream->fd;
    if( fd<0 )
    {
        fd = streaming_open( stream->streaming_ctrl->url );
        if( fd<0 )
            return -1;
        stream->fd = fd;
				stream->end_pos=streaming_getfilesize(fd);
    }
    else
    {}

    stream->streaming_ctrl->streaming_close = xbmp_streaming_close;
    stream->streaming_ctrl->streaming_read = xbmp_streaming_read;
    stream->streaming_ctrl->streaming_seek = xbmp_streaming_seek;
    stream->streaming_ctrl->prebuffer_size = 8192;	// KBytes
    stream->streaming_ctrl->buffering = 1;
    stream->streaming_ctrl->status = streaming_playing_e;
    return 0;
}

int nop_streaming_read( int fd, char *buffer, int size, streaming_ctrl_t *stream_ctrl )
{
    int len=0;
    //printf("nop_streaming_read\n");
    if( stream_ctrl->buffer_size!=0 )
    {
        int buffer_len = stream_ctrl->buffer_size-stream_ctrl->buffer_pos;
        //printf("%d bytes in buffer\n", stream_ctrl->buffer_size);
        len = (size<buffer_len)?size:buffer_len;
        memcpy( buffer, (stream_ctrl->buffer)+(stream_ctrl->buffer_pos), len );
        stream_ctrl->buffer_pos += len;
        //printf("buffer_pos = %d\n", stream_ctrl->buffer_pos );
        if( stream_ctrl->buffer_pos>=stream_ctrl->buffer_size )
        {
            free( stream_ctrl->buffer );
            stream_ctrl->buffer = NULL;
            stream_ctrl->buffer_size = 0;
            stream_ctrl->buffer_pos = 0;
            //printf("buffer cleaned\n");
        }
        //printf("read %d bytes from buffer\n", len );
    }

    if( len<size )
    {
        int ret;
        ret = socket_read( fd, buffer+len, size-len );
        if( ret<0 )
        {
            mp_msg(MSGT_NETWORK,MSGL_ERR,"nop_streaming_read error : %s\n",strerror(errno));
        }
        len += ret;
        //printf("read %d bytes from network\n", len );
    }

    return len;
}

int nop_streaming_seek( int fd, off_t pos, streaming_ctrl_t *stream_ctrl )
{
    return -1;
    // To shut up gcc warning
    fd++;
    pos++;
    stream_ctrl=NULL;
}

int nop_streaming_start( stream_t *stream )
{
    HTTP_header_t *http_hdr = NULL;
    int fd;
    if( stream==NULL )
        return -1;

    fd = stream->fd;
    if( fd<0 )
    {
        fd = http_send_request( stream->streaming_ctrl->url );
        if( fd<0 )
            return -1;
        http_hdr = http_read_response( fd );
        if( http_hdr==NULL )
            return -1;

        switch( http_hdr->status_code )
        {
        case 200: // OK
            mp_msg(MSGT_NETWORK,MSGL_V,"Content-Type: [%s]\n", http_get_field(http_hdr, "Content-Type") );
            mp_msg(MSGT_NETWORK,MSGL_V,"Content-Length: [%s]\n", http_get_field(http_hdr, "Content-Length") );
            if( http_hdr->body_size>0 )
            {
                if( streaming_bufferize( stream->streaming_ctrl, http_hdr->body, http_hdr->body_size )<0 )
                {
                    http_free( http_hdr );
                    return -1;
                }
            }
            break;
        default:
            mp_msg(MSGT_NETWORK,MSGL_ERR,"Server return %d: %s\n", http_hdr->status_code, http_hdr->reason_phrase );
            socket_close( fd );
            fd = -1;
        }
        stream->fd = fd;
    }
    else
    {
        http_hdr = (HTTP_header_t*)stream->streaming_ctrl->data;
        if( http_hdr->body_size>0 )
        {
            if( streaming_bufferize( stream->streaming_ctrl, http_hdr->body, http_hdr->body_size )<0 )
            {
                http_free( http_hdr );
                stream->streaming_ctrl->data = NULL;
                return -1;
            }
        }
    }

    if( http_hdr )
    {
        http_free( http_hdr );
        stream->streaming_ctrl->data = NULL;
    }

		stream->streaming_ctrl->streaming_close = nop_streaming_close;
    stream->streaming_ctrl->streaming_read = nop_streaming_read;
    stream->streaming_ctrl->streaming_seek = nop_streaming_seek;
    stream->streaming_ctrl->prebuffer_size = 64*1024; // 64 KBytes
    stream->streaming_ctrl->buffering = 1;
    stream->streaming_ctrl->status = streaming_playing_e;
    return 0;
}

#endif

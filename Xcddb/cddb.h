#ifndef XCDDB
#define XCDDB

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBNet.h>
#include <xgraphics.h>
#include <assert.h>
#include <d3d8perf.h>
#include <string>
#include "debugclient.h"
#include <xbsocket.h>
#include <xbsockaddr.h>
#include <sstream>
#include <iostream>
#include <strstream>
#include "debug.h"
#include <map>


#define	IN_PROGRESS						 -1
#define	OK							 	  0
#define	E_FAILED					 	  1
#define	E_TOC_INCORRECT				 	  2
#define	E_NETWORK_ERROR_OPEN_SOCKET	 	  3
#define	E_NETWORK_ERROR_SEND		 	  4
#define	E_WAIT_FOR_INPUT				  5
#define	E_PARAMETER_WRONG				  6
#define	QUERRY_OK					 	  7

#define	E_NO_MATCH_FOUND		        202
#define	E_INEXACT_MATCH_FOUND	        211
		
#define	W_CDDB_already_shook_hands      402
#define	E_CDDB_Handshake_not_successful	431
#define	E_CDDB_permission_denied	    432
#define	E_CDDB_max_users_reached	    433
#define	E_CDDB_system_load_too_high	    434

#define CDDB_PORT 8880

using namespace std;

extern "C"
{
	// Thanks and credit go to Team Evox
	extern VOID	 WINAPI HalReturnToFirmware(DWORD);
	extern INT WINAPI XNetLoadConfigParams(LPBYTE);   
	extern INT WINAPI XNetSaveConfigParams(LPBYTE);     
	extern INT WINAPI XWriteTitleInfoNoReboot(LPVOID,LPVOID,DWORD,DWORD,LPVOID);
	extern DWORD* LaunchDataPage;  
}

struct toc {
	int	min;
	int	sec;
	int	frame;
};



//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class Xcddb
{
	SOCKET m_cddb_socket;  
	const static int recv_buffer=4096;
	int lastError;
	map<int,string> titles;
	map<int,string> artists;
	map<int,string> extended_track;
	
	map<int,string> inexact_cddb_command_list;
	map<int,string> inexact_artist_list;
	map<int,string> inexact_title_list;

	char disk_artist[1024];
	char disk_title[1024];
	char year[4];
	char genre[1024];
	char extended[1024];
	bool initialize_network_ok;
	void Xcddb::addTitle(char *buffer);
	void Xcddb::addExtended(char *buffer);
	void parseData(char *buffer);
	HRESULT Xcddb::Send( void *buffer, int bytes );
	string Xcddb::Recv(bool wait4point);
	HRESULT Xcddb::Send( char *buffer);
	HRESULT	openSocket();
	HRESULT Xcddb::closeSocket();
	struct toc cdtoc[100];
	int cddb_sum(int n);
	void addInexactList(char *list);
	void Xcddb::addInexactListLine(int line_cnt, char *line, int len);
	const char *Xcddb::getInexactCommand(int select);
	char cddb_ip_adress[20];

	// Debug Parameter Start
	CDebugClient		*debug_client;
	CXBoxDebug			*mpDebug;
	BOOL				b_debug_enabled;
	// Debug Parameter Ende
	
	public:
		void writeLog(char *str);
		void writeLog(const char* pzFormat, ...);
		BOOL Xcddb::InitializeNetwork(char *szLocalAddress,	char *szLocalSubnet, char *szLocalGateway);
		BOOL Xcddb::InitDebug(char *szRemoteAddress, int remotePort);
		BOOL Xcddb::IsEthernetConnected();
		void Xcddb::setCDDBIpAdress(char *ip_adress);
		int Xcddb::queryCDinfo(int real_track_count, toc cdtoc[]);
		Xcddb::Xcddb();
		Xcddb::Xcddb(CXBoxDebug *mpDebug, CDebugClient *p_debug_client);
		int Xcddb::queryCDinfo(int inexact_list_select);
		int getLastError();
		char *Xcddb::getLastErrorText();
		const char *Xcddb::getTrackArtist(int track);
		const char* Xcddb::getTrackTitle(int track);
		void Xcddb::getDiskArtist(char* t_disk_artist);
		void Xcddb::getDiskTitle(char* t_disk_title);
		const char* getTrackExtended(int track);
		unsigned long Xcddb::calc_disc_id(int nr_of_tracks, toc cdtoc[]);
		const char *Xcddb::getInexactArtist(int select);
		const char *Xcddb::getInexactTitle(int select);

};

// Thanks and credit go to Team Evox
typedef struct TXNetConfigParams
{   
	DWORD	Data_00;            // Check Block Start   
	DWORD	Data_04;   
	DWORD	Data_08;   
	DWORD	Data_0c;   
	DWORD	Data_10;            // Check Block End   
  
	DWORD	V1_IP;              // 0x14   
	DWORD	V1_Subnetmask;      // 0x18   
	DWORD	V1_Defaultgateway;  // 0x1c   
	DWORD	V1_DNS1;            // 0x20   
	DWORD	V1_DNS2;            // 0x24   

	DWORD	Data_28;            // Check Block Start   
	DWORD	Data_2c;   
	DWORD	Data_30;   
	DWORD	Data_34;   
	DWORD	Data_38;            // Check Block End   

	DWORD	V2_Tag;             // V2 Tag "XBV2"   
 
	DWORD	Flag;				// 0x40   
	DWORD	Data_44;   

	DWORD	V2_IP;              // 0x48   
	DWORD	V2_Subnetmask;      // 0x4c   
	DWORD	V2_Defaultgateway;  // 0x50   
	DWORD	V2_DNS1;            // 0x54   
	DWORD	V2_DNS2;            // 0x58   

	DWORD   Data_xx[0x200-0x5c];

} TXNetConfigParams,*PTXNetConfigParams;   
#endif

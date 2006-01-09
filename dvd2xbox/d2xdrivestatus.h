#ifndef D2XDRIVESTATUS
#define D2XDRIVESTATUS


#include <helper.h>
#include <undocumented.h>
#include <xtl.h>
#include <io.h>
#include <IOSupport.h>
#include <cdiosupport.h>
#include "d2xsettings.h"
#include "d2xfilefactory.h"
#include "d2xmedialib.h"
#include <thread.h>

#include "unlock\d2xunlock.h"

using namespace MEDIA_DETECT;


class D2Xdstatus : public CThread
{
protected:
	CIoSupport			m_IO;
	CCdInfo*			m_pCdInfo;
	static LONGLONG		dvdsize;
	LONGLONG			CountDVDsize(char *path);
	DWORD				GetTrayState();
	DWORD				m_dwTrayState;
	DWORD				m_dwTrayCount;
	DWORD				m_dwLastTrayState;
	D2Xfile*			p_file;
	static WCHAR		m_scdstat[128];
	static int			type;
	static DWORD		mediaReady;
	D2Xmedialib			p_ml;
	D2Xunlock			p_u;

public:
	D2Xdstatus();
	~D2Xdstatus();

	virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();


	static void GetDriveState(WCHAR *w_scdstat,int& itype);
	void GetDriveState();
	//void DetectMedia(WCHAR *m_scdstat,int& type);
	void DetectMedia();
	int	 countMB(char* drive);
	static DWORD getMediaStatus();


};

#endif
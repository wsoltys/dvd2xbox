#include "d2xtitle.h"

int D2Xtitle::i_network = 0;
char D2Xtitle::c_cddbip[20];
char D2Xtitle::disk_artist[1024];
char D2Xtitle::disk_title[1024];
char* D2Xtitle::track_artist[100];
char* D2Xtitle::track_title[100];

D2Xtitle::D2Xtitle()
{
	p_help = new HelperX();
	p_cdripx = new CCDRipX();
	tocentries = 0;
	cddbquery = 0;
}

D2Xtitle::~D2Xtitle()
{
	delete[] p_help;
	delete[] p_cdripx;
}

bool D2Xtitle::getCDDADiskTitle(char* title)
{
	strcpy(disk_artist,"\0");
	strcpy(disk_title,"\0");
	int i = 0;
	while(disk_artist[i] != NULL)
	{
		delete[] track_artist[i];
		delete[] track_title[i];
		track_artist[i] = NULL;
		track_title[i] = NULL;
		i++;
	}
	if(!D2Xtitle::i_network)
		return false;
	DPf_H("In getCDDADiskTitle");
	io.Remount("D:","Cdrom0");
	DPf_H("Try cdripx init");
	if(p_cdripx->Init()==E_FAIL)
	{
		cddbquery = 0;
		return false;
	}
	tocentries = p_cdripx->GetNumTocEntries();
	DPf_H("found %d tracks,try to connect %s",tocentries,D2Xtitle::c_cddbip);
	m_cddb.setCDDBIpAdress(D2Xtitle::c_cddbip);
	toc cdtoc[100];
	for (int i=0;i<=tocentries;i++)
	{
		// stupid but it works
		cdtoc[i].min=p_cdripx->oCDCon[i].min;
		cdtoc[i].sec=p_cdripx->oCDCon[i].sec;
		cdtoc[i].frame=p_cdripx->oCDCon[i].frame;
	}
	
	p_cdripx->DeInit();
	DPf_H("try to query db");
	if ( m_cddb.queryCDinfo(tocentries, cdtoc) == E_FAILED)
	{

		DPf_H("Query failed");
		int lasterror=m_cddb.getLastError();
		
		if (lasterror == E_WAIT_FOR_INPUT)
		{
		

			int i=1;
			
			if((m_cddb.getInexactArtist(i)!=NULL) && (m_cddb.getInexactTitle(i)!=NULL))
			{
		
				char buffer[2048];
				char t_disk_artist[1024];

				if ( m_cddb.queryCDinfo(i) == E_FAILED)
				{
					cddbquery = 0;
					return false;
				}

				strcpy(buffer,"\0");
				strcpy(t_disk_artist,m_cddb.getInexactArtist(i));
				char t_disk_title[1024];
				strcpy(t_disk_title,m_cddb.getInexactTitle(i));
				if(t_disk_artist!=NULL)
				{
					strcat(buffer,t_disk_artist);
					strcpy(disk_artist,t_disk_artist);
				}
				if(t_disk_artist!=NULL && t_disk_title!=NULL )
					strcat(buffer," - ");
				if(t_disk_title!=NULL)
				{
					strcat(buffer,t_disk_title);
					strcpy(disk_title,t_disk_title);
				}
				p_help->getFatxName(buffer);
				strcpy(title,buffer);
				cddbquery = 1;
				return true;
			} else {
				cddbquery = 0;
				return false;
			}
		} else {
			cddbquery = 0;
			return false;
		}
		
	}
	else
	{
		DPf_H("Got discname");
		// Jep, tracks are received
		char buffer[2048];
		char t_disk_artist[1024];
		strcpy(buffer,"\0");
		m_cddb.getDiskArtist(t_disk_artist);
		char t_disk_title[1024];
		m_cddb.getDiskTitle(t_disk_title);
		if(t_disk_artist!=NULL)
		{
			strcat(buffer,t_disk_artist);
			strcpy(disk_artist,t_disk_artist);
		}
		if(t_disk_artist!=NULL && t_disk_title!=NULL )
			strcat(buffer," - ");
		if(t_disk_title!=NULL)
		{
			strcat(buffer,t_disk_title);
			strcpy(disk_title,t_disk_title);
		}
		p_help->getFatxName(buffer);
		p_help->getFatxName(disk_artist);
		p_help->getFatxName(disk_title);
		strcpy(title,buffer);
		cddbquery = 1;
		DPf_H("Discname %s",title);
	}

	return true;
}

bool D2Xtitle::getCDDATrackTitle(char* file,int track)
{
	char buffer[1024];
	char dartist[1024];
	char artist[1024];
	char titel[1024];

	DPf_H("In getCDDATrackTitle");
	if(cddbquery && D2Xtitle::i_network)
	{
		strcpy(buffer,"\0");
			
		m_cddb.getDiskArtist(dartist);
		if((m_cddb.getTrackArtist(track) != NULL) && (strlen(m_cddb.getTrackArtist(track)) > 1))
		{
			strcpy(artist,m_cddb.getTrackArtist(track));
			p_help->getFatxName(artist);
			strcat(buffer,artist);
			track_artist[track-1] = new char[strlen(artist)+1];
			strcpy(track_artist[track-1],artist);
		}
		else if(dartist != NULL)
		{
			p_help->getFatxName(dartist);
			strcat(buffer,dartist);
			track_artist[track-1] = new char[strlen(dartist)+1];
			strcpy(track_artist[track-1],dartist);
		}
		if(strlen(buffer) > 1)
			strcat(buffer," - ");
		if(m_cddb.getTrackTitle(track) != NULL)
		{
			strcpy(titel,m_cddb.getTrackTitle(track));
			p_help->getFatxName(titel);
			strcat(buffer,titel);
			track_title[track-1] = new char[strlen(titel)+1];
			strcpy(track_title[track-1],titel);
		}
				
		//p_help->getFatxName(buffer);
		DPf_H("Buffer %s",buffer);
		sprintf(file,"%s%s",file,buffer);
		DPf_H("Track %d %s",track,file);
		track_title[tocentries] = NULL;
		track_artist[tocentries] = NULL;

	} else {
        sprintf(file,"%strack%02d",file,track);
		DPf_H("Track %d %s",track,file);
	}
	return true;	
}

int D2Xtitle::getXBETitle(char* file,WCHAR* m_GameTitle)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	wcscpy(m_GameTitle,L"");
	char *reverse = new char[strlen(file)+1];
	strcpy(reverse,file);
	if(_strnicmp(_strrev(reverse),"ebx.",4))
		return 0;
	delete[] reverse;
	stream  = fopen( file, "rb" );
	if(stream == NULL) {
	} else {
	
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,1,sizeof(HC),stream);
		fclose(stream);
		wcscpy(m_GameTitle, HC.TitleName);
		return 1;
	}	
    return 0;
}

bool D2Xtitle::getDVDTitle(char* title)
{
	char titleid[32];
	unsigned char setid[128];
	int ret = 0;
	dvd_reader_t*	dvd;

	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		DPf_H("Could not authenticate DVD");
		return false;
	}

	ret = DVDUDFVolumeInfo(dvd,titleid,sizeof(titleid),setid,sizeof(setid));
	if((ret != -1) && (ret != 0))
	{
		sprintf(title,"%hs",titleid);
	} else if(ret == 0)
	{
		if(DVDISOVolumeInfo(dvd,titleid,sizeof(titleid),setid,sizeof(setid)) != 1)
			sprintf(title,"%hs",titleid);
	} 
	
    DVDClose(dvd);
	return true;
}

char* D2Xtitle::GetNextPath(char *drive,int type)
{
	static char testname[1024];
	static char title[50];
	char drivepath[1024];
	int count=1;
	int ret = 0;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	boolean GoodOne=true;

	strcpy(drivepath,drive);
	
	strcat(drivepath,"*");

	strcpy(title,"dvd2xbox");

	if(type == GAME)
	{
		WCHAR m_GameTitle[43];
		if( getXBETitle("d:\\default.xbe",m_GameTitle) ) 
		{
			wsprintf(title,"%S",m_GameTitle);	
			p_help->getFatxName(title);
			count = 0;
		}
	} else if(type == DVD)
	{	
		if(getDVDTitle(title))
		{
			p_help->getFatxName(title);
			count = 0;
		}
	} 
	else if((type == CDDA) && i_network)
	{	
		if(getCDDADiskTitle(title))
		{
			//mhelp->getFatxName(title);
			count = 0;
		}
	} 
	while(count<=999)
	{
		if(count)
		{
			if(strlen(title) > 38)
			{
				strncpy(testname,title,38);
				testname[39] = '\0';
				sprintf(testname,"%hs%03d",testname,count);
			} else {
                sprintf(testname,"%hs%03d",title,count);
			}
		} else 
		{
			sprintf(testname,"%hs",title);
		}

		GoodOne=true;

		// Start the find and check for failure.
		hFind = FindFirstFile(drivepath, &wfd );

		// See if this exists in the directory
	    do
	    {
			// Only do directories
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY && strcmp(wfd.cFileName,testname)==0)
			{
				GoodOne=false;
				break;
			}
	    }
	    while(FindNextFile( hFind, &wfd ));
	    // Close the find handle.
	    FindClose( hFind );

		if(GoodOne) break;
		count++;
	}

	strcat(testname,"\\");

	return testname;

}

void D2Xtitle::getvalidFilename(char* path,char *name,char* ext)
{
	static char testname[1024];
	static char title[50];
	char drivepath[1024];
	int count=0;
	int ret = 0;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	boolean GoodOne=true;

	strcpy(drivepath,path);
	p_help->addSlash(drivepath);
	strcat(drivepath,"*");

	while(count<=999)
	{
		if(count)
		{
			if(strlen(name) > 34)
			{
				strncpy(testname,name,34);
				testname[35] = '\0';
				sprintf(testname,"%hs%03d%hs",testname,count,ext);
			} else {
                sprintf(testname,"%hs%03d%hs",name,count,ext);
			}
		} else 
		{
			sprintf(testname,"%hs%hs",name,ext);
		}
		p_help->getFatxName(testname);
		GoodOne=true;

		// Start the find and check for failure.
		hFind = FindFirstFile(drivepath, &wfd );

		// See if this exists in the directory
	    do
	    {
			// Only do files
			if(wfd.dwFileAttributes|FILE_ATTRIBUTE_DIRECTORY && strcmp(wfd.cFileName,testname)==0)
			{
				GoodOne=false;
				break;
			}
	    }
	    while(FindNextFile( hFind, &wfd ));
	    // Close the find handle.
	    FindClose( hFind );

		if(GoodOne) break;
		count++;
	}
	strcpy(name,testname);
}
#include "D2Xgamemanager.h"


D2XGM::D2XGM()
{
}

D2XGM::~D2XGM()
{
}

void D2XGM::DeleteStats()
{
	DeleteFile(g_d2xSettings.disk_statsPath);
}

int D2XGM::deleteItem(int ID)
{
	FILE*			stream1;
	FILE*			stream2;
	GMheader		sheader;
	GMitem			sitem;
	unsigned short	items_to_go;
	int				i;

	stream1 = fopen(g_d2xSettings.disk_statsPath,"rb");
	if(stream1 == NULL)
		return 0;
	
	stream2 = fopen(g_d2xSettings.disk_statsPath_new,"w+b");
	if(stream2 == NULL)
	{
		fclose(stream1);
		return 0;
	}

	fread(&sheader,sizeof(GMheader),1,stream1);

	items_to_go = sheader.total_items;

	if(fseek(stream1,sizeof(GMheader)+(ID-1)*sizeof(GMitem), SEEK_SET) != 0)
	{
		fclose(stream1);
		fclose(stream2);
		return 0;
	}

	fread(&sitem,sizeof(GMitem),1,stream1);

	sheader.total_dirs -= sitem.dirs;
	sheader.total_files -= sitem.files;
	sheader.total_items--;
	sheader.total_MB -= sitem.sizeMB;

	fwrite(&sheader, sizeof(GMheader), 1, stream2);

	if(fseek(stream1,sizeof(GMheader), SEEK_SET) != 0)
	{
		fclose(stream1);
		fclose(stream2);
		return 0;
	}

	for(i=1;i<=items_to_go;i++)
	{
		fread(&sitem,sizeof(GMitem),1,stream1);
		if(i != ID)
		{
			fwrite(&sitem,sizeof(GMitem),1,stream2);
		}
	}
	fclose(stream1);
	fclose(stream2);

	if(MoveFileEx(g_d2xSettings.disk_statsPath_new,g_d2xSettings.disk_statsPath,MOVEFILE_REPLACE_EXISTING)==0)
		return 0;

	return 1;
}

int D2XGM::addItem(GMitem item)
{
	FILE*		stream;
	GMheader	sheader;

	stream = fopen(g_d2xSettings.disk_statsPath,"r+b");
	if(stream == NULL)
	{
		stream = fopen(g_d2xSettings.disk_statsPath,"w+b");
		if(stream == NULL)
			return 0;

		strncpy(sheader.token,"D2XGM",5);
		sheader.total_dirs = 0;
		sheader.total_files = 0;
		sheader.total_items = 0;
		sheader.total_MB = 0;
	}
	else
	{
		fread(&sheader,sizeof(sheader),1,stream);
	}

	sheader.total_dirs += item.dirs;
	sheader.total_files += item.files;
	sheader.total_items ++;
	sheader.total_MB += item.sizeMB;

	fseek( stream, 0, SEEK_SET);
	fwrite(&sheader, sizeof(GMheader), 1, stream);
	fseek( stream, sizeof(GMheader)+(sheader.total_items-1)*sizeof(GMitem), SEEK_SET);
	fwrite(&item, sizeof(GMitem), 1, stream);
	fclose(stream);
	return 1;
}

int	D2XGM::readItem(int ID, GMitem* item)
{
	FILE*		stream;
	stream = fopen(g_d2xSettings.disk_statsPath,"r+b");
	if(stream == NULL)
		return 0;
	
	if(fseek(stream,sizeof(GMheader)+(ID-1)*sizeof(GMitem), SEEK_SET) != 0)
		return 0;

	if(fread(item,sizeof(GMitem),1,stream) <= 0)
	{
		fclose(stream);
		return 0;
	}
	fclose(stream);
	return 1;
}

int	D2XGM::readHeader(GMheader* header)
{
	FILE*		stream;
	stream = fopen(g_d2xSettings.disk_statsPath,"r+b");
	if(stream == NULL)
		return 0;

	if(fread(header,sizeof(GMheader),1,stream) <= 0)
	{
		fclose(stream);
		return 0;
	}
	fclose(stream);
	return 1;
}
void D2XGM::ScanDisk()
{
	for(unsigned int i=0;i<g_d2xSettings.xmlGameDirs.size();i++)
	{
		ScanHardDrive(g_d2xSettings.xmlGameDirs[i].c_str());
	}
}

int D2XGM::ScanHardDrive(const char* path)
{
	char c_path[128]="";
	char sourcefile[128]="";
	HANDLE hFind;
	WIN32_FIND_DATA wfd;

	strcpy(c_path,path);
	p_utils.addSlash(c_path);
	strcat(c_path,"*");

	hFind = FindFirstFile( c_path, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return 0;
	}
	else
	{
	    
	    do
	    {
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				memset(global_item.title,0,sizeof(global_item.title));
				memset(global_item.full_path,0,sizeof(global_item.full_path));
				global_item.dirs = 0;
				global_item.files = 0;
				global_item.TitleId = 0;


				strcpy(sourcefile,path);
				p_utils.addSlash(sourcefile);
				strcat(sourcefile,wfd.cFileName);
				strcat(sourcefile,"\\default.xbe");

				if(p_utils.getXBECert(sourcefile))
				{
					strcpy(sourcefile,path);
					p_utils.addSlash(sourcefile);
					strcat(sourcefile,wfd.cFileName);
					strcat(sourcefile,"\\");

					wcscpy(global_item.title,p_utils.xbecert.TitleName);
					strcpy(global_item.full_path,sourcefile);
					global_item.TitleId = p_utils.xbecert.TitleId;
					global_item.dirs++;
					global_item.sizeMB = (unsigned short) (CountItemSize(sourcefile)/1048576);

					addItem(global_item);
				}
			}

		}
		while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}

	

	return 1;
}

LONGLONG D2XGM::CountItemSize(char *path)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	LONGLONG llValue = 0;
	LONGLONG llResult = 0;
	LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return false;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				global_item.dirs++;
				strcat(sourcefile,"\\");
				// Recursion
				llValue += CountItemSize( sourcefile );
			}
			else
			{
				global_item.files++;
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					llValue += liSize.QuadPart;
				}
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}

	return llValue;
}

int D2XGM::PrepareList()
{

	FILE*		stream;
	GMitem		item;

	stream = fopen(g_d2xSettings.disk_statsPath,"r+b");
	if(stream == NULL)
		return 0;
	
	global_list.header.total_items = 0;
	fread(&global_list.header,sizeof(GMheader),1,stream);
	
	while(fread(&item,sizeof(GMitem),1,stream) > 0)
	{
		global_list.item.push_back(item);
	}
	fclose(stream);

	// Window start values
	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;

	return 1;
}

GMitem D2XGM::ProcessGameManager(XBGAMEPAD pad)
{

	p_input.update(pad);

	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) 
	{
		if(cbrowse > 1)
            cbrowse--;
		if(crelbrowse>1)
		{
            crelbrowse--;
		} else {
			if(coffset > 0)
				coffset--;
		}
	}
	if((pad.fY1 > 0.5)) {
		Sleep(100);
		if(cbrowse > 1)
            cbrowse--;
		if(crelbrowse>1)
		{
            crelbrowse--;
		} else {
			if(coffset > 0)
				coffset--;
		}
	}
	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
	{
		if(cbrowse < global_list.header.total_items)
            cbrowse++;
		if(crelbrowse<SHOWGAMES)
		{
            crelbrowse++;
		} else {
			if(coffset < (global_list.header.total_items-SHOWGAMES))
				coffset++;
		}
	}
	if(pad.fY1 < -0.5) {
		Sleep(100);
		if(cbrowse < global_list.header.total_items)
            cbrowse++;
		if(crelbrowse<SHOWGAMES)
		{
            crelbrowse++;
		} else {
			if(coffset < (global_list.header.total_items-SHOWGAMES))
				coffset++;
		}
	}

	if(p_input.pressed(GP_A))
	{
		p_utils.LaunchXbe(global_list.item[cbrowse-1].full_path,"d:\\default.xbe");
	}

	return global_list.item[cbrowse-1];
}

void D2XGM::ShowGameManager(CXBFont &font)
{
	float tmpy=0;
	int c=0;

	for(int i=0;i<SHOWGAMES;i++)
	{
		c = i+coffset;
		tmpy = i*font.m_fFontHeight;
		if(c >= global_list.header.total_items)
			break;
		 
		if((i+coffset) == (cbrowse-1))
		{
            font.DrawText( START_X, START_Y+tmpy, HIGHLITE_COLOR, global_list.item[c].title );
		} else {
			font.DrawText( START_X, START_Y+tmpy, TEXT_COLOR, global_list.item[c].title  );
		}

	} 
}

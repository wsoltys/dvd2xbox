#include "D2Xgamemanager.h"


D2XGM::D2XGM()
{
	/*D2Xff factory;
	p_file = factory.Create(UDF);*/
}

D2XGM::~D2XGM()
{
	/*delete p_file;
	p_file = NULL;*/
}

void D2XGM::DeleteStats()
{
	DeleteFile(g_d2xSettings.disk_statsPath);
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
	fwrite(&sheader, sizeof(sheader), 1, stream);
	fseek( stream, sizeof(sheader)+(sheader.total_items-1)*sizeof(item), SEEK_SET);
	fwrite(&item, sizeof(item), 1, stream);
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

int D2XGM::ScanHardDrive(char* path)
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
#include "D2Xacl.h"
#include <stdstring.h>
#include <xtl.h>
#include <helper.h>



D2Xacl::D2Xacl()
{
	reset();
	p_log = new D2Xlogger();
	p_util = new D2Xutils();
}

D2Xacl::~D2Xacl()
{
	reset();
	delete p_log;
	delete p_util;
}

void D2Xacl::reset()
{
	if(m_destination != NULL)
	{
		delete m_destination;
		m_destination = NULL;
	}
	m_acltype = ACL_UNKNOWN;
}

void D2Xacl::resetPattern()
{
	for(int i=0;i<ACL_PATTERNS;i++)
	{
		//delete m_pattern[i];
		//m_pattern[i] = NULL;
		memset(m_pattern[i],'0',ACL_PATTERN_LENGTH);
	}
}




bool D2Xacl::processACL(char* dest)
{
	char buffer[1024];
	char path[1024];
	char gameID[20];
	bool m_default = true;
	bool m_section = true;

	reset();
	resetPattern();
	m_destination = new char[strlen(dest)+1];
	strcpy(m_destination,dest);
	strcpy(path,dest);
	p_util->addSlash(path);
	strcat(path,"default.xbe");
	DPf_H("default: %s",path);
	m_titleID = p_util->getTitleID(path);
	if(m_titleID == 0)
	{
		p_log->WLog(L"");
		p_log->WLog(L"Couldn't obtain titleID from %hs. using default section.",path);
		m_section = false;
	}
	DPf_H("title id %X",m_titleID);
	p_IO.GetXbePath(path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;
	sprintf(path,"%s\\dvd2xbox.acl",path);

	FILE* stream;
	stream = fopen(path,"r");
	if(stream == NULL)
	{
		p_log->WLog(L"Couldn't open acl file: %hs",path);
		return false;
	}
	if(m_section)
        sprintf(gameID,"[%X]",m_titleID);
	while((fgets(buffer,1024,stream) != NULL) && m_section)
	{
		if(strstr(buffer,gameID))
		{
			m_default = false;
			p_log->WLog(L"Matched %hs section.",gameID);
			while((fgets(buffer,1024,stream) != NULL) && strncmp(buffer,"[",1))
			{
				processSection(buffer);
			}
		} 
	}

	fseek(stream,0,SEEK_SET);
	while((fgets(buffer,1024,stream) != NULL) && m_default)
	{
		if(stricmp(buffer,"[default]"))
		{
			p_log->WLog(L"Matched [default] section.");
			while((fgets(buffer,1024,stream) != NULL) && strncmp(buffer,"[",1))
			{
				processSection(buffer);
			}
		} 
	}
	fclose(stream);
	return true;
}


bool D2Xacl::processSection(char* pattern)
{
	if(!_strnicmp(pattern,"HR|",3))
	{
		sscanf(pattern,"HR|%[^|]|%[^|]|%[^|]|%[^|]|",m_currentmask,m_pattern[0],m_pattern[1],m_pattern[2]);
		DPf_H("HR: %X; Pattern: %s,%s,%s,%s",m_titleID,m_currentmask,m_pattern[0],m_pattern[1],m_pattern[2]);
		m_acltype = ACL_HEXREPLACE;
		FillVars(m_currentmask);
		processFiles(m_destination);
	} else if(!_strnicmp(pattern,"CP|",3))
	{
		sscanf(pattern,"CP|%[^|]|%[^|]|",m_pattern[0],m_pattern[1]);
		DPf_H("HR: %X; Pattern: %s,%s",m_titleID,m_pattern[0],m_pattern[1]);
		m_acltype = ACL_COPYFILES;
		FillVars(m_pattern[0]);
		FillVars(m_pattern[1]);
		if(CopyFile(m_pattern[0],m_pattern[1],false)!=0)
			p_log->WLog(L"Ok: Copied %hs to %hs.",m_pattern[0],m_pattern[1]);
		else
			p_log->WLog(L"Error: Failed to copy %hs to %hs.",m_pattern[0],m_pattern[1]);
	}
	resetPattern();
	return true;
}


bool D2Xacl::processFiles(char *path)
{
	char* sourcesearch = new char[1024];
	char* sourcefile = new char[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	p_util->addSlash(sourcesearch);
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
			p_util->addSlash(sourcefile);
			strcat(sourcefile,wfd.cFileName);
			if(!_stricmp(wfd.cFileName,"dvd2xbox.log"))
				continue;

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				// Recursion
				processFiles( sourcefile );
			}
			else
			{
				if(strrchr(m_currentmask,'*'))
				{
					// found * assuming extension
					if(strcmp(m_currentmask,"*"))
					{
						// extension
                        char* p_ext = strrchr(m_currentmask,'.');
						char* p_file = strrchr(sourcefile,'.');
						p_ext++;
						p_file++;
						if(stricmp(p_ext,p_file))
						{
							// not the same extension. skipping file
							continue;
						}
					}
				}
				else
				{
					// no * assuming file
					if(stricmp(m_currentmask,sourcefile))
						continue;
				}
				
				switch(m_acltype)
				{
                    case ACL_HEXREPLACE:
						HexReplace(sourcefile);
						break;
					default:
						break;
				}
			}
	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	delete sourcesearch;
	sourcesearch = NULL;
	delete sourcefile;
	sourcefile = NULL;
	return true;
}

void D2Xacl::FillVars(char* pattern)
{
	char* pat;
	char temp[1024];
	if(pat = strstr(pattern,"${DEST}"))
	{
		strcpy(temp,pat+7);
		strcpy(pattern,m_destination);
		strcat(pattern,temp);
	}
}

void D2Xacl::HexReplace(char* file)
{
	
	int mc_pos=0;
	int pos = 0;
	char patch_pos[30];
	char cur_pos[10];
	sprintf(patch_pos,",%s,",m_pattern[0]);
	p_log->WLog(L"Checking %hs for %hs",file,m_pattern[1]);
	while((mc_pos = p_util->findHex(file,m_pattern[1],mc_pos))>=0)
	{
		pos++;
		sprintf(cur_pos,",%d,",pos);
		if((mc_pos >= 0) && (strstr(patch_pos,cur_pos)))
		{
			if(!p_util->writeHex(file,m_pattern[2],mc_pos))
			{
				p_log->WLog(L"Ok: Replaced %hs by %hs at position %d",m_pattern[1],m_pattern[2],mc_pos);
			} else {
				p_log->WLog(L"Error: Found %hs at position %d but couldn't patch file.",m_pattern[1],mc_pos);
			}
		}
	}
}

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
	int i=0;
	while(m_pattern[i] != NULL)
	{
		delete m_pattern[i];
		m_pattern[i] = NULL;
		i++;
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
	m_titleID = p_util->getTitleID(path);
	if(m_titleID != 0)
	{
		p_log->WLog(L"");
		p_log->WLog(L"Couldn't obtain titleID from %s. using default section.",path);
		m_section = false;
	}
	
	p_IO.GetXbePath(path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;
	sprintf(path,"%s\\dvd2xbox.acl",path);

	FILE* stream;
	stream = fopen(path,"r");
	if(stream == NULL)
	{
		p_log->WLog(L"Couldn't open acl file: %s",path);
		return false;
	}
	if(m_section)
        sprintf(gameID,"[%s]",m_titleID);
	while((fgets(buffer,1024,stream) != NULL) && m_section)
	{
		if(strstr(buffer,gameID))
		{
			m_default = false;
			p_log->WLog(L"Matched %X section.",gameID);
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
	//m_currentpattern = new char[strlen(pattern)+1];
	//strcpy(m_currentpattern,pattern);

	if(!_strnicmp(pattern,"HR:",3))
	{
		sscanf(pattern,"HR:%[^:]:%[^:]:%[^:]:%[^:];",m_currentmask,m_pattern[0],m_pattern[1],m_pattern[2]);
		DPf_H("HR: %X; Pattern: %s,%s,%s,%s",m_titleID,m_currentmask,m_pattern[0],m_pattern[1],m_pattern[2]);
		m_pattern[3] = NULL;
		m_acltype = ACL_HEXREPLACE;
		processFiles(m_destination);
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
			strcat(sourcefile,wfd.cFileName);

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
					if(stricmp(m_currentmask,sourcefile+strlen(m_destination)+1))
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

void D2Xacl::HexReplace(char* file)
{
	
	int mc_pos=0;
	int pos = 0;
	char patch_pos[30];
	char cur_pos[10];
	sprintf(patch_pos,",%s,",m_pattern[0]);
	while((mc_pos = p_util->findHex(file,m_pattern[1],mc_pos))>=0)
	{
		pos++;
		sprintf(cur_pos,",%d,",pos);
		if((mc_pos >= 0) && (strstr(patch_pos,cur_pos)))
		{
			if(!p_util->writeHex(file,m_pattern[2],mc_pos))
			{
				p_log->WLog(L"Found %s at position %d and replaced by %s",m_pattern[1],mc_pos,m_pattern[2]);
			} else {
				p_log->WLog(L"Error patching file %s",file);
			}
		}
	}
}
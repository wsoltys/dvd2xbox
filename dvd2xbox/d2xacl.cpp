#include "D2Xacl.h"
#include <stdstring.h>
#include <xtl.h>
#include <helper.h>



D2Xacl::D2Xacl()
{
	reset();
	p_log = new D2Xlogger();
	p_util = new D2Xutils();
	m_acltype = ACL_UNKNOWN;
}

D2Xacl::~D2Xacl()
{
	//reset();
	delete p_log;
	delete p_util;
}

void D2Xacl::reset()
{ 
	/*
	if(m_destination != NULL)
	{ 
		delete[] m_destination;
		m_destination = NULL;
	}
	*/
	memset(m_destination,0,sizeof(m_destination));
	m_acltype = ACL_UNKNOWN;
}

void D2Xacl::resetPattern()
{
	for(int i=0;i<ACL_PATTERNS;i++)
	{
		memset(m_pattern[i],'0',ACL_PATTERN_LENGTH);
	}
}




bool D2Xacl::processACL(char* dest,int state)
{
	char buffer[1024];
	char path[1024];
	char all_path[1024];
	char item_path[1024];
	char default_path[1024];
	CIoSupport p_IO;
	FILE* stream;

	reset();
	resetPattern();
	//m_destination = new char[strlen(dest)+2];
	strcpy(m_destination,dest);
	p_util->addSlash(m_destination);
	strcpy(path,dest);
	p_util->addSlash(path);
	strcat(path,"default.xbe");
	DPf_H("default: %s",path);
	m_titleID = p_util->getTitleID(path);
	if(m_titleID == 0)
	{
		p_log->WLog(L"");
		p_log->WLog(L"Couldn't obtain titleID from %hs. using default section.",path);
	}
	DPf_H("title id %X",m_titleID);
	p_IO.GetXbePath(path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;
	_snprintf(all_path,1000,"%s\\acl\\all.acl",path);
	DPf_H("ACL ALL: %s",all_path);
	if(m_titleID != 0)
	{
        _snprintf(item_path,1000,"%s\\acl\\%X.acl",path,m_titleID);
		DPf_H("ACL title: %s",item_path);
	}
	_snprintf(default_path,1000,"%s\\acl\\default.acl",path);
	DPf_H("ACL default: %s",default_path);

	if((m_titleID != 0) && (GetFileAttributes(item_path) != -1))
	{
		
		stream = fopen(item_path,"r");
		if(stream == NULL)
		{
			p_log->WLog(L"Couldn't open acl file: %hs",item_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
			p_log->WLog(L"Using %hs for post processing.",item_path);
		else
			p_log->WLog(L"Using %hs for pre processing.",item_path);

	} else {
		DPf_H("before wlog");
		p_log->WLog(L"Couldnt find acl file for title %X. Using default acl.",m_titleID);
		DPf_H("after wlog");
		stream = fopen(default_path,"r");
		if(stream == NULL)
		{
			p_log->WLog(L"Couldn't open acl file: %hs",default_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
            p_log->WLog(L"Using %hs for post processing.",default_path);
		else 
			p_log->WLog(L"Using %hs for pre processing.",default_path);
	}
	

	memset(buffer,'0',1024);
	while((fgets(buffer,1024,stream) != NULL))
	{
		if(state == ACL_POSTPROCESS)
	        processSection(buffer);
		else
			PreprocessSection(buffer);
		memset(buffer,'0',1024);
	}
	fclose(stream);

	if((GetFileAttributes(all_path) != -1))
	{
		
		stream = fopen(all_path,"r");
		if(stream == NULL)
		{
			p_log->WLog(L"Couldn't open acl file: %hs",all_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
            p_log->WLog(L"Using %hs for post processing.",all_path);
		else
			p_log->WLog(L"Using %hs for pre processing.",all_path);

	} else {
		p_log->WLog(L"Couldn't find %hs",all_path);
		return false;
	}
	


	memset(buffer,'0',1024);
	while((fgets(buffer,1024,stream) != NULL))
	{
		if(state == ACL_POSTPROCESS)
	        processSection(buffer);
		else
			PreprocessSection(buffer);
		memset(buffer,'0',1024);
	}
	fclose(stream);

	DPf_H("Leaving processACL");
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
		if(strchr(m_currentmask,':'))
		{
			char t_dest[1024];
			strcpy(t_dest,m_currentmask);
			strcpy(m_currentmask,strrchr(t_dest,'\\')+1);
			t_dest[strlen(t_dest)-strlen(m_currentmask)] = '\0';
			processFiles(t_dest,false);
		} else
		{
			processFiles(m_destination,true);
		}
		
	} else if(!_strnicmp(pattern,"CP|",3))
	{
		sscanf(pattern,"CP|%[^|]|%[^|]|",m_pattern[0],m_pattern[1]);
		DPf_H("CP: %X; Pattern: %s,%s",m_titleID,m_pattern[0],m_pattern[1]);
		m_acltype = ACL_COPYFILES;
		FillVars(m_pattern[0]);
		FillVars(m_pattern[1]);
		if(CopyFile(m_pattern[0],m_pattern[1],false)!=0)
			p_log->WLog(L"Ok: Copied %hs to %hs.",m_pattern[0],m_pattern[1]);
		else
			p_log->WLog(L"Error: Failed to copy %hs to %hs.",m_pattern[0],m_pattern[1]);
	} else if(!_strnicmp(pattern,"RM|",3))
	{
		sscanf(pattern,"RM|%[^|]|",m_currentmask);
		DPf_H("RM: %s",m_currentmask);
		m_acltype = ACL_DELFILES;
		FillVars(m_currentmask);
		if(strchr(m_currentmask,':'))
		{
			if(strchr(m_currentmask,'*'))
			{
				char t_dest[1024];
				strcpy(t_dest,m_currentmask);
				strcpy(m_currentmask,strrchr(t_dest,'\\')+1);
				t_dest[strlen(t_dest)-strlen(m_currentmask)] = '\0';
				processFiles(t_dest,false);
			} else {
				DelItem(m_currentmask);
			}

		} else
		{
			processFiles(m_destination,true);
		} 
		
	} else if(!_strnicmp(pattern,"SM|",3))
	{
		sscanf(pattern,"SM|%[^|]|%[^|]|",m_currentmask,m_pattern[0]);
		DPf_H("SM: %X; Pattern: %s,%s",m_titleID,m_currentmask,m_pattern[0]);
		m_acltype = ACL_SETMEDIA;
		FillVars(m_currentmask);
		if(strchr(m_currentmask,':'))
		{
			char t_dest[1024];
			strcpy(t_dest,m_currentmask);
			strcpy(m_currentmask,strrchr(t_dest,'\\')+1);
			t_dest[strlen(t_dest)-strlen(m_currentmask)] = '\0';
			processFiles(t_dest,false);
		} else
		{
			processFiles(m_destination,true);
		}
	} else if(!_strnicmp(pattern,"MV|",3))
	{
		sscanf(pattern,"MV|%[^|]|%[^|]|",m_pattern[0],m_pattern[1]);
		DPf_H("MV: %X; Pattern: %s,%s",m_titleID,m_pattern[0],m_pattern[1]);
		m_acltype = ACL_MOVEFILES;
		FillVars(m_pattern[0]);
		FillVars(m_pattern[1]);
		if(MoveFileEx(m_pattern[0],m_pattern[1],MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING)!=0)
			p_log->WLog(L"Ok: Moved %hs to %hs.",m_pattern[0],m_pattern[1]);
		else
			p_log->WLog(L"Error: Failed to move %hs to %hs.",m_pattern[0],m_pattern[1]);
	}
	resetPattern();
	return true;
}

bool D2Xacl::PreprocessSection(char* pattern)
{
	if(!_strnicmp(pattern,"ED|",3))
	{
		sscanf(pattern,"ED|%[^|]|",m_pattern[0]);
		D2Xfilecopy::setExcludePatterns(NULL,m_pattern[0]);
		p_log->WLog(L"Set excludeDirs to %hs",m_pattern[0]);
	} else if(!_strnicmp(pattern,"EF|",3))
	{
		sscanf(pattern,"EF|%[^|]|",m_pattern[0]);
		D2Xfilecopy::setExcludePatterns(m_pattern[0],NULL);
		p_log->WLog(L"Set excludeFiles to %hs",m_pattern[0]);
	}	
	resetPattern();
	return true;
}



bool D2Xacl::processFiles(char *path, bool rec)
{
	char* sourcesearch = new char[1024];
	char* sourcefile = new char[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	DPf_H("pF: path %s",path);

	strcpy(sourcesearch,path);
	p_util->addSlash(sourcesearch);
	if(rec)
		strcat(sourcesearch,"*");
	else
        strcat(sourcesearch,m_currentmask);


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
				if(!rec)
					continue;
				// Recursion
				processFiles( sourcefile ,true);
			}
			else
			{
				
				if(rec && strrchr(m_currentmask,'*'))
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
				else if(rec)
				{
					// no * assuming file
					if(stricmp(m_currentmask,wfd.cFileName))
						continue;
				}
				
				switch(m_acltype)
				{
                    case ACL_HEXREPLACE:
						HexReplace(sourcefile);
						break;
					case ACL_SETMEDIA:
						ULONG mt;
						if(p_util->SetMediatype(sourcefile,mt,m_pattern[0]))
						{
							p_log->WLog(L"Ok: Setting media type on %hs from 0x%08X to 0x%hs",sourcefile,mt,m_pattern[0]);
						} else {
							p_log->WLog(L"Error: setting media type on %hs",sourcefile);
						}
						break;
					case ACL_DELFILES:
						DelItem(sourcefile);
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
		strcpy(temp,pat+8);
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
		char *pdest;
		pdest = strstr(patch_pos,cur_pos);
		//DPf_H("found at %d count %s",mc_pos,cur_pos);
		if(pdest != NULL)
		{
			//DPf_H("patch pos: %s, cur pos: %s",patch_pos,cur_pos);
			if(!p_util->writeHex(file,m_pattern[2],mc_pos))
			{
				p_log->WLog(L"Ok: Replaced %hs by %hs at position %d",m_pattern[1],m_pattern[2],mc_pos);
			} else {
				p_log->WLog(L"Error: Found %hs at position %d but couldn't patch file.",m_pattern[1],mc_pos);
			}
		}
		mc_pos++;
	}
}

void D2Xacl::DelItem(char* item)
{
	DWORD dwAttr = GetFileAttributes(item);
	if(strlen(m_pattern[0]) > 2)
	{
		if(dwAttr == FILE_ATTRIBUTE_DIRECTORY)
		{
			/*
			if(p_util->DelTree(item) == true)
				p_log->WLog(L"Ok: %hs deleted.",item);
			else
				p_log->WLog(L"Error: could not delete %hs.",item);
				*/
			DPf_H("Called DelTree with %s",item);
			
		} else
		{
			/*
			if(DeleteFile(m_pattern[0]) != 0)
				p_log->WLog(L"Ok: %hs deleted.",item);
			else
				p_log->WLog(L"Error: could not delete %hs.",item);
				*/
			DPf_H("Called Delfile with %s",item);
		}
	} else
		p_log->WLog(L"Info: %hs tried to delete a partition ? ;-)",item);
}

#include "D2Xacl.h"
#include <stdstring.h>
#include <xtl.h>
#include <helper.h>
#include "D2Xapplyppf3.h"


D2Xacl::D2Xacl()
{
	reset();
	m_acltype = ACL_UNKNOWN;
}

D2Xacl::~D2Xacl()
{
}

void D2Xacl::reset()
{ 
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
	//char all_path[1024];
	char pal_path[1024];
	char ntsc_path[1024];
	char item_path[1024];
	char default_path[1024];
	CIoSupport p_IO;
	FILE* stream;
	reset();
	resetPattern();
	D2Xfilecopy::excludeList.clear();

	// get Videostandard
	XKEEPROM x_ee;
	x_ee.ReadFromXBOX();
	x_ee.Decrypt();

	strcpy(m_destination,dest);
	p_util.addSlash(m_destination);
	strcpy(path,dest);
	p_util.addSlash(path);
	strcat(path,"default.xbe");
	DPf_H("default: %s",path);
	m_titleID = p_util.getTitleID(path);
	if(m_titleID == 0)
	{
		p_log.WLog(L"");
		p_log.WLog(L"Couldn't obtain titleID from %hs. using default section.",path);
	}
	DPf_H("title id %X",m_titleID);
	p_IO.GetXbePath(path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;
	//_snprintf(all_path,1000,"%s\\acl\\all.acl",path);
	//DPf_H("ACL ALL: %s",all_path);
	if(m_titleID != 0)
	{
        _snprintf(item_path,1000,"%s\\acl\\%X.acl",path,m_titleID);
		_snprintf(pal_path, 1000,"%s\\acl\\%X_pal.acl",path,m_titleID);
		_snprintf(ntsc_path,1000,"%s\\acl\\%X_ntsc.acl",path,m_titleID);
		DPf_H("ACL title: %s",item_path);
	}
	_snprintf(default_path,1000,"%s\\acl\\default.acl",path);
	DPf_H("ACL default: %s",default_path);

	// PAL ACL
	if((x_ee.GetVideoStandardVal() == XKEEPROM::PAL_I) && (m_titleID != 0) && (GetFileAttributes(pal_path) != -1))
	{
		
		stream = fopen(pal_path,"r");
		if(stream == NULL)
		{
			p_log.WLog(L"Couldn't open acl file: %hs",pal_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
			p_log.WLog(L"Using %hs for post processing.",pal_path);
		else
			p_log.WLog(L"Using %hs for pre processing.",pal_path);

	} 
	// NTSC ACL
	else if(((x_ee.GetVideoStandardVal() == XKEEPROM::NTSC_M) || (x_ee.GetVideoStandardVal() == XKEEPROM::NTSC_J)) && (m_titleID != 0) && (GetFileAttributes(ntsc_path) != -1))
	{
		
		stream = fopen(ntsc_path,"r");
		if(stream == NULL)
		{
			p_log.WLog(L"Couldn't open acl file: %hs",ntsc_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
			p_log.WLog(L"Using %hs for post processing.",ntsc_path);
		else
			p_log.WLog(L"Using %hs for pre processing.",ntsc_path);

	}
	// TITLE ACL
	else if((m_titleID != 0) && (GetFileAttributes(item_path) != -1))
	{
		
		stream = fopen(item_path,"r");
		if(stream == NULL)
		{
			p_log.WLog(L"Couldn't open acl file: %hs",item_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
			p_log.WLog(L"Using %hs for post processing.",item_path);
		else
			p_log.WLog(L"Using %hs for pre processing.",item_path);

	}
	// DEFAULT ACL
	else 
	{
		DPf_H("before wlog");
		p_log.WLog(L"Couldnt find acl file for title %X. Using default acl.",m_titleID);
		DPf_H("after wlog");
		stream = fopen(default_path,"r");
		if(stream == NULL)
		{
			p_log.WLog(L"Couldn't open acl file: %hs",default_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
            p_log.WLog(L"Using %hs for post processing.",default_path);
		else 
			p_log.WLog(L"Using %hs for pre processing.",default_path);
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

	/*if((GetFileAttributes(all_path) != -1))
	{
		
		stream = fopen(all_path,"r");
		if(stream == NULL)
		{
			p_log.WLog(L"Couldn't open acl file: %hs",all_path);
			return false;
		}
		if(state == ACL_POSTPROCESS)
            p_log.WLog(L"Using %hs for post processing.",all_path);
		else
			p_log.WLog(L"Using %hs for pre processing.",all_path);

	} else {
		p_log.WLog(L"Couldn't find %hs",all_path);
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
	fclose(stream);*/

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
		} else if(!_strnicmp(m_currentmask,"*.xbe",5) && !D2Xfilecopy::XBElist.empty())
		{
			iXBElist it;
			it = D2Xfilecopy::XBElist.begin();
			while (it != D2Xfilecopy::XBElist.end() )
			{
				string& item = *it;
				HexReplace(item.c_str(),true);
				it++;
			}
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
			p_log.WLog(L"Ok: Copied %hs to %hs.",m_pattern[0],m_pattern[1]);
		else
			p_log.WLog(L"Error: Failed to copy %hs to %hs.",m_pattern[0],m_pattern[1]);
	} else if(!_strnicmp(pattern,"RM|",3))
	{
		if(g_d2xSettings.enableRMACL)
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
		} else
			p_log.WLog(L"RM disabled: %hs",pattern); 
		
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
		} else if(!_strnicmp(m_currentmask,"*.xbe",5) && !D2Xfilecopy::XBElist.empty())
		{
			iXBElist it;
			it = D2Xfilecopy::XBElist.begin();
			while (it != D2Xfilecopy::XBElist.end() )
			{
				string& item = *it;
				ULONG mt;
				if(p_util.SetMediatype(item.c_str(),mt,m_pattern[0]))
				{
					p_log.WLog(L"Ok: Setting media type on %hs from 0x%08X to 0x%hs (cache)",item.c_str(),mt,m_pattern[0]);
				} else {
					p_log.WLog(L"Error: setting media type on %hs (cache)",item.c_str());
				}
				it++;
			}
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
			p_log.WLog(L"Ok: Moved %hs to %hs.",m_pattern[0],m_pattern[1]);
		else
			p_log.WLog(L"Error: Failed to move %hs to %hs.",m_pattern[0],m_pattern[1]);
	} 
	else if(!_strnicmp(pattern,"FR|",3) && !D2Xfilecopy::RENlist.empty())
	{
		sscanf(pattern,"FR|%[^|]|",m_currentmask);
		DPf_H("FR: %X; Pattern: %s",m_titleID,m_currentmask);
		m_acltype = ACL_FILENAMEREPLACE;
		FillVars(m_currentmask);
		if(strchr(m_currentmask,':'))
		{
			char t_dest[1024];
			strcpy(t_dest,m_currentmask);
			strcpy(m_currentmask,strrchr(t_dest,'\\')+1);
			t_dest[strlen(t_dest)-strlen(m_currentmask)] = '\0';
			processFiles(t_dest,false);
		} else if(!_strnicmp(m_currentmask,"*.xbe",5) && !D2Xfilecopy::XBElist.empty())
		{
			iXBElist it;
			it = D2Xfilecopy::XBElist.begin();
			while (it != D2Xfilecopy::XBElist.end() )
			{
				string& item = *it;
				FileNameReplace(item.c_str(),true);
				it++;
			}
		} else
		{
			processFiles(m_destination,true);
		}
		
	} else if(!_strnicmp(pattern,"AP|",3))
	{
		sscanf(pattern,"AP|%[^|]|%[^|]|",m_pattern[0],m_pattern[1]);
		m_acltype = ACL_APPLYPPF;
		FillVars(m_pattern[0]);
		char ppf[1024];
		strcpy(ppf,g_d2xSettings.HomePath);
		p_util.addSlash(ppf);
		strcat(ppf,"acl\\");
		strcat(ppf,m_pattern[1]);
		D2Xppf p_ppf;
		p_ppf.ApplyPPF3('a',m_pattern[0],ppf);
	} 
	resetPattern();
	return true;
}

bool D2Xacl::PreprocessSection(char* pattern)
{
	/*
	if(!_strnicmp(pattern,"ED|",3))
	{
		sscanf(pattern,"ED|%[^|]|",m_pattern[0]);
		D2Xfilecopy::setExcludePatterns(NULL,m_pattern[0]);
		p_log.WLog(L"Set excludeDirs to %hs",m_pattern[0]);
	} else if(!_strnicmp(pattern,"EF|",3))
	{
		sscanf(pattern,"EF|%[^|]|",m_pattern[0]);
		D2Xfilecopy::setExcludePatterns(m_pattern[0],NULL);
		p_log.WLog(L"Set excludeFiles to %hs",m_pattern[0]);
	}
	*/
	if(!_strnicmp(pattern,"EP|",3))
	{	
		sscanf(pattern,"EP|%[^|]|",m_pattern[0]);
		string pat(m_pattern[0]);
		D2Xfilecopy::excludeList.push_back(pat);
		p_log.WLog(L"Set excludeFiles to %hs",m_pattern[0]);
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
	p_util.addSlash(sourcesearch);
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
			p_util.addSlash(sourcefile);
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
						HexReplace(sourcefile,false);
						break;
					case ACL_SETMEDIA:
						ULONG mt;
						if(p_util.SetMediatype(sourcefile,mt,m_pattern[0]))
						{
							p_log.WLog(L"Ok: Setting media type on %hs from 0x%08X to 0x%hs",sourcefile,mt,m_pattern[0]);
						} else {
							p_log.WLog(L"Error: setting media type on %hs",sourcefile);
						}
						break;
					case ACL_DELFILES:
						DelItem(sourcefile);
						break;
					case ACL_FILENAMEREPLACE:
						FileNameReplace(sourcefile,false);
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

void D2Xacl::HexReplace(const char* file,bool cache)
{
	
	int mc_pos=0;
	int pos = 0;
	char patch_pos[30];
	char cur_pos[10];
	sprintf(patch_pos,",%s,",m_pattern[0]);
	if(cache)
		p_log.WLog(L"Checking %hs for %hs (cache)",file,m_pattern[1]);
	else
        p_log.WLog(L"Checking %hs for %hs",file,m_pattern[1]);
	while((mc_pos = p_util.findHex(file,m_pattern[1],mc_pos))>=0)
	{
		pos++;
		sprintf(cur_pos,",%d,",pos);
		char *pdest;
		pdest = strstr(patch_pos,cur_pos);
		//DPf_H("found at %d count %s",mc_pos,cur_pos);
		if(pdest != NULL)
		{
			//DPf_H("patch pos: %s, cur pos: %s",patch_pos,cur_pos);
			if(!p_util.writeHex(file,m_pattern[2],mc_pos))
			{
				p_log.WLog(L"Ok: Replaced %hs by %hs at position %d",m_pattern[1],m_pattern[2],mc_pos);
			} else {
                p_log.WLog(L"Error: Found %hs at position %d but couldn't patch file",m_pattern[1],mc_pos);
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
			
			if(p_util.DelTree(item) == true)
				p_log.WLog(L"Ok: %hs deleted.",item);
			else
				p_log.WLog(L"Error: could not delete %hs.",item);
			
			DPf_H("Called DelTree with %s",item);
			
		} else
		{
			
			if(DeleteFile(item) != 0)
				p_log.WLog(L"Ok: %hs deleted.",item); 
			else
				p_log.WLog(L"Error: could not delete %hs.",item);
				
			DPf_H("Called Delfile with %s",item);
		}
	} else
		p_log.WLog(L"Info: %hs tried to delete a partition ? ;-)",item);
}

void D2Xacl::FileNameReplace(const char* file,bool cache)
{
	int mc_pos=0;
	iRENlist it;
	it = D2Xfilecopy::RENlist.begin();

	while (it != D2Xfilecopy::RENlist.end() )
	{
		char ofile[100];
		char rfile[100];
		if(cache)
			p_log.WLog(L"Checking %hs for %hs (cache)",file,it->first.c_str());
		else
			p_log.WLog(L"Checking %hs for %hs",file,it->first.c_str());

		p_util.str2hex(it->first,ofile);
		p_util.str2hex(it->second,rfile);

		while((mc_pos = p_util.findHex(file,ofile,mc_pos))>=0)
		{
			
			if(!p_util.writeHex(file,rfile,mc_pos))
			{
				p_log.WLog(L"Ok: Replaced %hs by %hs at position %d",ofile,rfile,mc_pos);
			} else {
				p_log.WLog(L"Error: Found %hs at position %d but couldn't patch file",ofile,mc_pos);
			}
	
			mc_pos++;
		}

		it++;
	}
}

/*
 * ApplyPPF v2.0 for Linux/Unix. Coded by Icarus/Paradox 2k
 * If you want to compile applyppf just enter "gcc applyppf.c"
 * that's it but i think the Linux users know :)
 * 
 * This one applies both, PPF1.0 and PPF2.0 patches.
 *
 * Sorry for the bad code i had no time for some cleanup.. but
 * it works 100% ! Byebye!
 *
 * Btw feel free to use this in your own projects etc of course!!
 */
 
 

int D2Xacl::ApplyPPF(char* file,char* ppf)
{
	
		FILE *binfile; 
		FILE *ppffile;
		char buffer[5];
		char method, in;
		char desc[50];
		char diz[3072];
		int  dizlen, binlen, dizyn, dizlensave;
		char ppfmem[512];
		int  count, seekpos, pos, anz;
		char ppfblock[1025];
		char binblock[1025];
        /*printf("ApplyPPF v2.0 for Linux/Unix (c) Icarus/Paradox\n");
        if(argc==1){
        printf("Usage: ApplyPPF <Binfile> <PPF-File>\n"); 
        return 0;
        }*/

        /* Open the bin and ppf file */
        binfile=fopen(file, "rb+");
        if(binfile==NULL){
			p_log.WLog(L"Warning: File %s does not exist (ppf).",file);
        return 0;
        }
        ppffile=fopen(ppf, "rb");
        if(ppffile==NULL){
			p_log.WLog(L"Warning: File %s does not exist (ppf).",ppf);
        return 0;
        }

        /* Is it a PPF File ? */
        fread(buffer, 3, 1, ppffile);
        if(strncmp("PPF", buffer,3)){
			p_log.WLog(L"Error: File %s is no ppf file).",ppf);
        fclose(ppffile);
        fclose(binfile);
        return 0;
        }

        /* What encoding Method? PPF1.0 or PPF2.0? */
        fseek(ppffile, 5, SEEK_SET);
        fread(&method, 1, 1,ppffile);

        switch(method)
        {
                case 0:
			 /* Show PPF-Patchinformation. */
			 /* This is a PPF 1.0 Patch! */
                         fseek(ppffile, 6,SEEK_SET);  /* Read Desc.line */
                         fread(desc, 50, 1,ppffile);
                         /*printf("\nFilename       : %s\n",argv[2]);
                         printf("Enc. Method    : %d (PPF1.0)\n",method);
                         printf("Description    : %s\n",desc);
                         printf("File_id.diz    : no\n\n");*/
                         
			 /* Calculate the count for patching the image later */
			 /* Easy calculation on a PPF1.0 Patch! */
                         fseek(ppffile, 0, SEEK_END);
                         count=ftell(ppffile);
                         count-=56;
                         seekpos=56;
                         printf("Patching ... ");
                         break;
                case 1:
			 /* Show PPF-Patchinformation. */
			 /* This is a PPF 2.0 Patch! */
                         fseek(ppffile, 6,SEEK_SET);
                         fread(desc, 50, 1,ppffile);
                        /* printf("\nFilename       : %s\n",argv[2]);
                         printf("Enc. Method    : %d (PPF2.0)\n",method);
                         printf("Description    : %s\n",desc);*/

                         fseek(ppffile, -8,SEEK_END);
                         fread(buffer, 4, 1,ppffile);

			 /* Is there a File id ?! */
                         if(strcmp(".DIZ", buffer)){
                         printf("File_id.diz    : no\n\n");
                         dizyn=0;
                         }
                         else{
                         printf("File_id.diz    : yes, showing...\n");
                         fread(&dizlen, 4, 1, ppffile);
                         fseek(ppffile, -dizlen-20, SEEK_END);
                         fread(diz, dizlen, 1, ppffile);
                         diz[dizlen-7]='\0';
                         printf("%s\n",diz);
                         dizyn=1;
                         dizlensave=dizlen;
                         }
                        
                         /* Do the BINfile size check! */
                         fseek(ppffile, 56, SEEK_SET);
                         fread(&dizlen, 4, 1,ppffile);
                         fseek(binfile, 0, SEEK_END);
                         binlen=ftell(binfile);
                         if(dizlen!=binlen){
                         printf("The size of the BIN file isnt correct\nCONTINUE though? (y/n): ");
							p_log.WLog(L"Warning: The size of differs from original size (ppf).",file);
                         /*in=getc(stdin);
                         if(in!='y'&&in!='Y'){
                         fclose(ppffile);
                         fclose(binfile);
                         printf("\nAborted...\n");
                         return 0;
                         }*/}

			 /* do the Binaryblock check! this check is 100% secure! */
                         /*fseek(ppffile, 60, SEEK_SET);
                         fread(ppfblock, 1024, 1, ppffile);
                         fseek(binfile, 0x9320, SEEK_SET);
                         fread(binblock, 1024, 1, binfile);
                         in=memcmp(ppfblock, binblock, 1024);
                         if(in!=0){
                         printf("The BINfile does not seem to be the right one\nCONTINUE though? (Suggestion: NO) (y/n): ");
                         in=getc(stdin);
                         if(in!='y'&&in!='Y'){
                         fclose(ppffile);
                         fclose(binfile);
                         printf("\nAborted...\n");
                         return 0;
                         }}*/

			 /* Calculate the count for patching the image later */
                         fseek(ppffile, 0, SEEK_END);
                         count=ftell(ppffile);
                         if(dizyn==0){
                         count-=1084;
                         seekpos=1084;
                         }else{
                         count-=1084;
                         count-=38;
                         count-=dizlensave;
                         seekpos=1084;
                         }
                         printf("Patching ... ");
                         break;
                default:
                
                	 /* Enc. Method wasnt 0 or 1 i bet you wont see this */
                         p_log.WLog(L"Warning: Unknown ppf format.");
                         fclose(ppffile);
                         fclose(binfile);
                         return 0;

        }

        /* Patch the Image */
        
        do{
        fseek(ppffile, seekpos, SEEK_SET);  /* seek to patchdataentry */
        fread(&pos, 4, 1, ppffile);	    /* Get POS for binfile */
        fread(&anz, 1, 1, ppffile);         /* How many byte do we have to write? */
        fread(ppfmem, anz, 1, ppffile);     /* And this is WHAT we have to write */
        fseek(binfile, pos, SEEK_SET);      /* Go to the right position in the BINfile */
        fwrite(ppfmem, anz, 1, binfile);    /* write 'anz' bytes to that pos from our ppfmem */
        seekpos=seekpos+5+anz;		    /* calculate next patchentry! */
        count=count-5-anz;                  /* have we reached the end of the PPFfile?? */
        }
        while(count!=0);		    /* if not -> LOOOOOP! */

        printf("DONE..\n");		    /* byebye :) */
        fclose(ppffile);
        fclose(binfile);
        return 0;
}

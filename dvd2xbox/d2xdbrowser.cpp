#include "d2xdbrowser.h"

bool D2Xdbrowser::renewAll = true;

////////////////////////////////////////////////////

D2Xdbrowser::D2Xdbrowser()
{

	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;
	mdirscount = 0;
	mfilescount = 0;
	level = 0;
	renew = true;
	cDirs[0]=NULL;
	cFiles[0]=NULL;
	browse_item.clear();
	relbrowse_item.clear();
	offset_item.clear();
	selected_item.clear();
	p_file = NULL;
	prev_type = UNDEFINED;
}

D2Xdbrowser::~D2Xdbrowser()
{
	if(p_file != NULL)
	{
		delete p_file;
		p_file = NULL;
	}
}

void D2Xdbrowser::Renew()
{
	renew = true;
}

bool D2Xdbrowser::RenewStatus()
{
	return renew;
}

void D2Xdbrowser::ResetCurrentDir()
{
	renew = true;
	cbrowse=1;
	crelbrowse = 1;
	coffset = 0;
}


int compare( const void *arg1, const void *arg2 )
{
   /* Compare all of both strings: */
   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
}

HDDBROWSEINFO D2Xdbrowser::processDirBrowser(int lines,char* path,XBGAMEPAD gp, XBIR_REMOTE ir,int type)
{
	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	HDDBROWSEINFO info;
	HelperX	p_help;

	if(!(p_help.isdriveD(path)) && (type != D2X_SMB))
		type = GAME;
	if(!strncmp(path,"ftp:",4))
	{
		type = FTP;
	}
	if(!strncmp(path,"smb:",4))
	{
		type = D2X_SMB;
	}

	if(strncmp(path,prevurl,3))
	{
		resetDirBrowser();
		/*if(!strncmp(prevurl,"ftp:",4))
            p_ftp.Close();*/
	}

	strncpy(prevurl,path,4);


	info.button = NO_PRESSED;
	info.mode = type;

	if(renew || renewAll)
	{
		renew = false;
		D2Xdbrowser::renewAll = false;
		for(int i=0;i<mdirscount;i++)
		{
			if(cDirs[i])
			{
				delete[] cDirs[i];
				cDirs[i]=NULL;
			}
		}
	
		for(int i=0;i<mfilescount;i++)
		{
			if(cFiles[i])
			{
				delete[] cFiles[i];
				cFiles[i]=NULL;
			}
		}
		mdirscount = 0;
		mfilescount = 0;
	
		selected_item.clear();
		

		DPf_H("browser type %d",type);

		if(type != CDDA)
		{
			if(level > 0)
			{
				mdirscount = 1;
				cDirs[0] = new char[3];
				strcpy(cDirs[0],"..");
			}

			VECFILEITEMS directory;

			if((prev_type != type) && (p_file != NULL))
			{
				delete p_file;
				p_file = NULL;

			}

			if(p_file == NULL)
			{
				D2Xff factory;
				p_file = factory.Create(type);
				prev_type = type;
			}


			strcpy(currentdir,path);

			if(p_file != NULL)
			{
				p_file->GetDirectory(path, &directory);

				for(int i=0;i<directory.size();i++)
				{
					if(directory[i].isDirectory)
					{
						cDirs[mdirscount] = new char[directory[i].name.size()+1]; 
						strcpy(cDirs[mdirscount],directory[i].name.c_str());
						mdirscount++;
					}
					else
					{
						cFiles[mfilescount] = new char[directory[i].name.size()+1];
						strcpy(cFiles[mfilescount],directory[i].name.c_str());
						mfilescount++;
					}
				}
			}


			//strcpy(sourcesearch,path);
			//if((type != D2X_SMB) && (type != FTP))
			//	strcat(sourcesearch,"*");

			//strcpy(currentdir,path);

			//// Start the find and check for failure.
			//hFind = D2XFindFirstFile( sourcesearch, &wfd ,type);

			//if( INVALID_HANDLE_VALUE == hFind )
			//{
			//	return info;
			//}
	
			//do
			//{
			//	if (wfd.cFileName[0]!=0)
		 //       {
		
			//	// Only do files
			//	if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			//	{
			//		CStdString strDir=wfd.cFileName;
			//		if (strDir != "." && strDir != "..")
			//		{
			//			cDirs[mdirscount] = new char[strlen(wfd.cFileName)+1];
			//			strcpy(cDirs[mdirscount],wfd.cFileName);
			//			mdirscount++;
			//		}
			//	}
			//	else
			//	{
			//	
			//		cFiles[mfilescount] = new char[strlen(wfd.cFileName)+1];
			//		strcpy(cFiles[mfilescount],wfd.cFileName);
			//		mfilescount++;

			//	}
			//	}
 
			//}
			//while(D2XFindNextFile( hFind, &wfd ,type));

			//// Close the find handle.
			//D2XFindClose( hFind,type );

			qsort( (void *)cDirs, (size_t)mdirscount, sizeof( char * ), compare );
			qsort( (void *)cFiles, (size_t)mfilescount, sizeof( char * ), compare );
		} else {
			D2Xcdrip p_cdripx;
			D2Xtitle p_title;
			/*if(p_cdripx.Init()==E_FAIL)
			{
				DPf_H("Failed to init cdripx");
				return info;
			}*/
			mfilescount = p_cdripx.GetNumTocEntries();
			DPf_H("Found %d Tracks",mfilescount);
			//p_cdripx.DeInit();
			if(D2Xtitle::i_network)
			{
				char temp[100];
				if(p_title.getCDDADiskTitle(temp))
				{
					
					for(int i=1;i<=mfilescount;i++)
					{
						strcpy(temp,"\0");
						p_title.getCDDATrackTitle(temp,i);
						cFiles[i-1] = new char[strlen(temp)+1];
						strcpy(cFiles[i-1],temp);
					}
					
				} 
				else
				{
					DPf_H("error during getCDDADiskTitle");
					for(int i=0;i<mfilescount;i++)
					{
						cFiles[i] = new char[8];
						sprintf(cFiles[i],"Track%02d",i+1);
					}
				}
			}
			else
			{
				for(int i=0;i<mfilescount;i++)
				{
					cFiles[i] = new char[8];
					sprintf(cFiles[i],"Track%02d",i+1);
				}
			}
		}
	}

	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) || (ir.wPressedButtons == XINPUT_IR_REMOTE_UP)) {
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
	if((gp.fY1 > 0.5)) {
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
	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) || (ir.wPressedButtons == XINPUT_IR_REMOTE_DOWN)) {
		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-lines))
				coffset++;
		}
	}
	if(gp.fY1 < -0.5) {
		Sleep(100);
		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-lines))
				coffset++;
		}
	}

	if(p_help.pressA(gp) || p_help.IRpressSELECT(ir))
	{
		if(cbrowse <= mdirscount)
		{
			renew = true;
			if(!strncmp(cDirs[cbrowse-1],"..",2))
			{
				level--;
				strcpy(path,cprevdir[level]);
				int& temp_browse = browse_item.back();
				cbrowse = temp_browse;
				browse_item.pop_back();
				temp_browse = relbrowse_item.back();
				crelbrowse = temp_browse;
				relbrowse_item.pop_back();
				temp_browse = offset_item.back();
				coffset = temp_browse;
				offset_item.pop_back();
				info.item[0] = '\0';
				info.title[0] = '\0';
				info.size = 0;
				return info;
			} 
			else
			{
				strcat(currentdir,cDirs[cbrowse-1]);
				browse_item.push_back(cbrowse);
				relbrowse_item.push_back(crelbrowse);
				offset_item.push_back(coffset); 
				cbrowse = 1;
				crelbrowse = 1;
				coffset = 0;
				strcpy(cprevdir[level],path);
				level++;
				strcpy(path,currentdir);
				if((type == FTP) || (type == D2X_SMB))
                    strcat(path,"/");
				else
					strcat(path,"\\");
				DPf_H("new path: %hs, old path: %hs",path,cprevdir[level-1]);
			}
			selected_item.clear();
		} else {
			//strcat(currentdir,cFiles[cbrowse-mdirscount]);
		}
		
	}

	if(cbrowse <= mdirscount)
	{
		// Directory
		strcpy(info.path,path);
		sprintf(info.item,"%s%s",path,cDirs[cbrowse-1]);
		strcpy(info.name,cDirs[cbrowse-1]);
		info.size = 0;
		wcscpy(info.title,L"");
		info.type=BROWSE_DIR;
	} else {
		if(cFiles[cbrowse-mdirscount-1] != NULL)
		{	
			strcpy(info.path,path);
			sprintf(info.item,"%s%s",path,cFiles[cbrowse-mdirscount-1]);
			strcpy(info.name,cFiles[cbrowse-mdirscount-1]);
			info.size = p_help.getFilesize(info.item);	
			//p_help.getXBETitle(info.item,info.title);
			p_title.getXBETitle(info.item,info.title);
			info.track = cbrowse-mdirscount;
		}
		else 
		{
			sprintf(info.item,"%s",path);
			info.size = 0;
			wcscpy(info.title,L"");
		}
		////DPf_H("Delete file %hs",info.item);
		info.type=BROWSE_FILE;
	}
	
	if(p_help.pressX(gp) || p_help.pressSTART(gp))
	{
		if(mdirscount || mfilescount)
		{
			info.button = p_help.pressX(gp) ? BUTTON_X : BUTTON_START;
		}
		return info;	
	}
	
	if(p_help.pressY(gp))
	{
		if(cbrowse > mdirscount && !_stricmp(cFiles[cbrowse-mdirscount-1],"default.xbe"))
		{
			info.button = BUTTON_Y;
			strcpy(info.item,path);
			return info;
		}
	}

	if(p_help.pressB(gp))
	{
		info.button = BUTTON_B;
		//if(selected_item[cbrowse-1].button != BUTTON_B)
		map<int,HDDBROWSEINFO>::iterator sel_iter;
		sel_iter = selected_item.find(cbrowse-1);
		if(sel_iter == selected_item.end( ))
		{
            //selected_item[cbrowse-1] = info;
			selected_item.insert(pair<int,HDDBROWSEINFO>((cbrowse-1),info));
		}
		else
			selected_item.erase(sel_iter);

		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-lines))
				coffset++;
		}
	}

	if(p_help.pressRTRIGGER(gp))
	{
		info.button = BUTTON_RTRIGGER;
	}
	if(p_help.pressLTRIGGER(gp))
	{
		info.button = BUTTON_LTRIGGER;
	}
	if(p_help.pressBLACK(gp))
	{
		info.button = BUTTON_BLACK;
	}
	if(p_help.pressWHITE(gp))
	{
		info.button = BUTTON_WHITE;
	}

	
	return info;
}

bool D2Xdbrowser::resetDirBrowser()
{

	for(int i=0;i<mdirscount;i++)
	{
		if(cDirs[i]!=NULL)
			delete[] cDirs[i];
		cDirs[i]=NULL;
	}
	
	for(int i=0;i<mfilescount;i++)
	{
		if(cFiles[i]!=NULL)
			delete[] cFiles[i];
		cFiles[i]=NULL;
	}
	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;
	mdirscount = 0;
	mfilescount = 0;
	level = 0;
	renew = true;
	browse_item.clear();
	relbrowse_item.clear();
	offset_item.clear();
	selected_item.clear();
	//p_ftp.Close();
	return true;
}

map<int,HDDBROWSEINFO> D2Xdbrowser::GetSelected()
{
	return selected_item;
}

bool D2Xdbrowser::showDirBrowser(int lines,float x,float y,DWORD fc,DWORD hlfc, CXBFont &font)
{
	WCHAR text[50];
	float tmpy=0;
	int c=0;
	char path[1024];
	D2Xgraphics	p_graph;


	strcpy(path,currentdir);
	strcat(path,"\\");

	for(int i=0;i<lines;i++)
	{
		c = i+coffset;
		tmpy = i*font.m_fFontHeight;
		if(c >= (mdirscount+mfilescount))
			break;
		char tname[34];
		if(c < mdirscount)
		{		
			strncpy(tname,cDirs[c],30);
			if(30 <= strlen(cDirs[c]))
				tname[30] = '\0';
			wsprintfW(text,L"<%hs>",tname);
		} else {
			strncpy(tname,cFiles[c-mdirscount],32);
			if(32 <= strlen(cFiles[c-mdirscount]))
				tname[32] = '\0'; 
			wsprintfW(text,L"%hs",tname);
		}
		if((i+coffset) == (cbrowse-1))
		{
			p_graph.RenderBrowserBar(x,y+tmpy,font.m_fFontHeight);
			font.DrawText( x, y+tmpy, hlfc, text );
		} else {
			font.DrawText( x, y+tmpy, fc, text );
		}

		map<int,HDDBROWSEINFO>::iterator sel_iter;
		sel_iter = selected_item.find(i+coffset);
		if(sel_iter != selected_item.end( ))
		//if((selected_item.size() > 0) && (selected_item[i+coffset].button == BUTTON_B))
				p_graph.RenderBrowserBarSelected(x,y+tmpy,font.m_fFontHeight);

	}
	
	return true;
}

bool D2Xdbrowser::showDirBrowser2(int lines,float x,float y,int width,DWORD fc,DWORD hlfc, const CStdString& font)
{
	WCHAR text[50];
	float tmpy=0;
	int c=0;
	char path[1024];
	D2Xgraphics	p_graph;


	strcpy(path,currentdir);
	strcat(path,"\\");

	for(int i=0;i<lines;i++)
	{
		c = i+coffset;
		tmpy = i*p_ml.getFontHeight(font);
		if(c >= (mdirscount+mfilescount))
			break;
		char tname[34];
		if(c < mdirscount)
		{		
			strncpy(tname,cDirs[c],width-2);
			if(30 <= strlen(cDirs[c]))
				tname[30] = '\0';
			wsprintfW(text,L"<%hs>",tname);
		} else {
			strncpy(tname,cFiles[c-mdirscount],width);
			if(32 <= strlen(cFiles[c-mdirscount]))
				tname[32] = '\0'; 
			wsprintfW(text,L"%hs",tname);
		}
		if((i+coffset) == (cbrowse-1))
		{
			//p_graph.RenderBrowserBar(x,y+tmpy,p_ml.getFontHeight(font));
			p_ml.DrawText(font, x, y+tmpy, hlfc, text );
		} else {
			p_ml.DrawText(font, x, y+tmpy, fc, text );
		}

		map<int,HDDBROWSEINFO>::iterator sel_iter;
		sel_iter = selected_item.find(i+coffset);
		//if(sel_iter != selected_item.end( ))
				//p_graph.RenderBrowserBarSelected(x,y+tmpy,p_ml.getFontHeight(font));

	}
	
	return true;
}


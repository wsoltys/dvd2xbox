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
	/*cDirs[0]=NULL;
	cFiles[0]=NULL;*/
	browse_item.clear();
	relbrowse_item.clear();
	offset_item.clear();
	selected_item.clear();
	p_file = NULL;
	prev_type = UNDEFINED;
	show_lines = 20;
	p_input = D2Xinput::Instance();
	i_vspace = 0;
	b_x = 0;
	b_y = 0;
	start_x = 0;
	start_y = 0;
	p_ml = D2Xmedialib::Instance();
}

D2Xdbrowser::~D2Xdbrowser()
{
	if(p_file != NULL)
	{
		delete p_file;
		p_file = NULL;
	}
	/*for(int i=0;i<mdirscount;i++)
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
	}*/
	c_Dir.directories.clear();
	c_Dir.files.clear();
	browse_item.clear();
	relbrowse_item.clear();
	offset_item.clear();
	selected_item.clear();
	drives.clear();
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


//int compare( const void *arg1, const void *arg2 )
//{
//   /* Compare all of both strings: */
//   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
//}

struct SortNames
{
  bool operator()(const CStdString& a, const CStdString& b)
  {
	  return _stricmp(a.c_str(),b.c_str()) <= 0 ? true : false;
  }
};

void D2Xdbrowser::getXY(float* posX, float* posY)
{
	*posX = b_x;
	*posY = b_y;
}

void D2Xdbrowser::getOrigin(float* posX, float* posY)
{
	*posX = start_x;
	*posY = start_y;
}

int D2Xdbrowser::getItems(int* vspace)
{
	if(vspace != NULL)
		*vspace = v_space;

	if(show_lines < (mdirscount+mfilescount))
		return show_lines;
	else
		return mdirscount+mfilescount;
}

void D2Xdbrowser::getInfo(HDDBROWSEINFO* s_info)
{
	*s_info = info;
}


HDDBROWSEINFO D2Xdbrowser::processDirBrowser(int lines,char* path,XBGAMEPAD gp, XBIR_REMOTE ir,int type)
{
	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	
	HelperX	p_help;

	//show_lines = lines;

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
	}

	strncpy(prevurl,path,4);


	info.button = NO_PRESSED;
	info.mode = type;

	if(renew || renewAll)
	{
		renew = false;
		D2Xdbrowser::renewAll = false;
		/*for(int i=0;i<mdirscount;i++)
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
		}*/
		c_Dir.directories.clear();
		c_Dir.files.clear();
		mdirscount = 0;
		mfilescount = 0;
	
		selected_item.clear();
		

		if(type != CDDA)
		{
			
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


			//strcpy(currentdir,path);
			c_Dir.current_directory = path;

			if(p_file != NULL)
			{
				p_file->GetDirectory(path, &directory);

				for(int i=0;i<directory.size();i++)
				{
					if(directory[i].isDirectory)
					{
						/*cDirs[mdirscount] = new char[directory[i].name.size()+1]; 
						strcpy(cDirs[mdirscount],directory[i].name.c_str());*/
						c_Dir.directories.push_back(directory[i].name.c_str());
						mdirscount++;
					}
					else
					{
						/*cFiles[mfilescount] = new char[directory[i].name.size()+1];
						strcpy(cFiles[mfilescount],directory[i].name.c_str());*/
						c_Dir.files.push_back(directory[i].name.c_str());
						mfilescount++;
					}
				}
			}


			/*qsort( (void *)cDirs, (size_t)mdirscount, sizeof( char * ), compare );
			qsort( (void *)cFiles, (size_t)mfilescount, sizeof( char * ), compare );*/
			std::sort(c_Dir.directories.begin(),c_Dir.directories.end(),SortNames());
			std::sort(c_Dir.files.begin(),c_Dir.files.end(),SortNames());

		} else {
			D2Xcdrip p_cdripx;
			D2Xtitle p_title;
			
			mfilescount = p_cdripx.GetNumTocEntries();
			DPf_H("Found %d Tracks",mfilescount);

			if(g_network.IsAvailable())
			{
				char temp[256];
				if(p_title.getCDDADiskTitle(temp))
				{
					
					for(int i=1;i<=mfilescount;i++)
					{
						strcpy(temp,"\0");
						p_title.getCDDATrackTitle(temp,i);
						/*cFiles[i-1] = new char[strlen(temp)+1];
						strcpy(cFiles[i-1],temp);*/
						c_Dir.files.push_back(temp);
					}
					
				} 
				else
				{
					DPf_H("error during getCDDADiskTitle");
					CStdString strTemp;
					for(int i=0;i<mfilescount;i++)
					{
						/*cFiles[i] = new char[8];
						sprintf(cFiles[i],"Track%02d",i+1);*/
						strTemp.Format("Track%02d",i+1);
						c_Dir.files.push_back(strTemp);
					}
				}
			}
			else
			{
				CStdString strTemp;
				for(int i=0;i<mfilescount;i++)
				{
					/*cFiles[i] = new char[8];
					sprintf(cFiles[i],"Track%02d",i+1);*/
					strTemp.Format("Track%02d",i+1);
					c_Dir.files.push_back(strTemp);
				}
			}
		}
	}

	//if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) || (ir.wPressedButtons == XINPUT_IR_REMOTE_UP)) 
	if(p_input->pressed(C_UP))
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
	//if((gp.fY1 > 0.5)) 
	if(p_input->pressed(GP_LTRIGGER_P) || (gp.fY1 > 0.5))
	{
		//Sleep(100);
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
	//if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) || (ir.wPressedButtons == XINPUT_IR_REMOTE_DOWN)) 
	if(p_input->pressed(C_DOWN))
	{
		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<show_lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-show_lines))
				coffset++;
		}
	}
	//if(gp.fY1 < -0.5) 
	if(p_input->pressed(GP_RTRIGGER_P) || (gp.fY1 < -0.5) )
	{
		//Sleep(100);
		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<show_lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-show_lines))
				coffset++;
		}
	}

	//if(p_help.pressA(gp) || p_help.IRpressSELECT(ir))
	if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
	{
		if(cbrowse <= mdirscount)
		{
			renew = true;
			{
				//strcat(currentdir,cDirs[cbrowse-1]);
				c_Dir.current_directory.append(c_Dir.directories[cbrowse-1]);
				browse_item.push_back(cbrowse);
				relbrowse_item.push_back(crelbrowse);
				offset_item.push_back(coffset); 
				cbrowse = 1;
				crelbrowse = 1;
				coffset = 0;
				//strcpy(cprevdir[level],path);
				c_Dir.previous_directory.push_back(path);
				level++;
				//strcpy(path,currentdir);
				strcpy(path,c_Dir.current_directory.c_str());
				if((type == FTP) || (type == D2X_SMB))
                    strcat(path,"/");
				else
					strcat(path,"\\");
			}
			selected_item.clear();
		} 
	}
	if(p_input->pressed(GP_B) || p_input->pressed(IR_BACK) || p_input->pressed(IR_INFO))
	{
		if(level > 0)
		{
			renew = true;
			level--;
			//strcpy(path,cprevdir[level]);
			strcpy(path,c_Dir.previous_directory.back().c_str());
			c_Dir.previous_directory.pop_back();
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
			selected_item.clear();
			return info;
		}
	}

	if(cbrowse <= mdirscount)
	{
		// Directory
		strcpy(info.path,path);
		//sprintf(info.item,"%s%s",path,cDirs[cbrowse-1]);
		sprintf(info.item,"%s%s",path,c_Dir.directories[cbrowse-1].c_str());
		//strcpy(info.name,cDirs[cbrowse-1]);
		strcpy(info.name,c_Dir.directories[cbrowse-1].c_str());
		info.size = 0;
		wcscpy(info.title,L"");
		info.type=BROWSE_DIR;
	} else {
		//if(cFiles[cbrowse-mdirscount-1] != NULL)
		if(c_Dir.files.size() > cbrowse-mdirscount-1 && !c_Dir.files[cbrowse-mdirscount-1].empty())
		{	
			strcpy(info.path,path);
			//sprintf(info.item,"%s%s",path,cFiles[cbrowse-mdirscount-1]);
			sprintf(info.item,"%s%s",path,c_Dir.files[cbrowse-mdirscount-1].c_str());
			//strcpy(info.name,cFiles[cbrowse-mdirscount-1]);
			strcpy(info.name,c_Dir.files[cbrowse-mdirscount-1].c_str());
			info.size = p_help.getFilesize(info.item);	
			p_title.getXBETitle(info.item,info.title);
			info.track = cbrowse-mdirscount;
		}
		else 
		{
			sprintf(info.item,"%s",path);
			info.size = 0;
			wcscpy(info.title,L"");
		}
		info.type=BROWSE_FILE;
	}


	if(p_input->pressed(GP_Y) || p_input->pressed(IR_DISPLAY))
	{
		info.button = BUTTON_B;
		map<int,HDDBROWSEINFO>::iterator sel_iter;
		sel_iter = selected_item.find(cbrowse-1);
		if(sel_iter == selected_item.end( ))
		{
			selected_item.insert(pair<int,HDDBROWSEINFO>((cbrowse-1),info));
		}
		else
			selected_item.erase(sel_iter);

		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<show_lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-show_lines))
				coffset++;
		}
	}

	
	if(p_input->pressed(GP_BLACK))
	{
		info.button = BUTTON_BLACK;
	}

	if(p_input->pressed(GP_WHITE))
	{
		info.button = BUTTON_WHITE;
	}

	// stupid workaround
	for(int i=0;i<show_lines;i++)
	{
		short c = i+coffset;
		short tmpy = i*i_vspace;
		if(c >= (mdirscount+mfilescount))
			break;
		if((i+coffset) == (cbrowse-1))
		{
			b_x = start_x;
			b_y = start_y+tmpy;
		} 
	}

	if(coffset > 0)
		info.top_items = true;
	else
		info.top_items = false;

	if(coffset < (mdirscount+mfilescount-show_lines))
		info.bottom_items = true;
	else
		info.bottom_items = false;

	return info;
}

bool D2Xdbrowser::resetDirBrowser()
{

	/*for(int i=0;i<mdirscount;i++)
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
	}*/
	c_Dir.directories.clear();
	c_Dir.files.clear();
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
	return true;
}

map<int,HDDBROWSEINFO> D2Xdbrowser::GetSelected()
{
	return selected_item;
}

//bool D2Xdbrowser::showDirBrowser(int lines,float x,float y,DWORD fc,DWORD hlfc, CXBFont &font)
//{
//	WCHAR text[50];
//	float tmpy=0;
//	int c=0;
//	char path[1024];
//	D2Xgraphics	p_graph;
//
//
//	strcpy(path,currentdir);
//	strcat(path,"\\");
//
//	i_vspace = font.m_fFontHeight;
//
//	for(int i=0;i<lines;i++)
//	{
//		c = i+coffset;
//		tmpy = i*i_vspace;
//		if(c >= (mdirscount+mfilescount))
//			break;
//		char tname[34];
//		if(c < mdirscount)
//		{		
//			strncpy(tname,cDirs[c],30);
//			if(30 <= strlen(cDirs[c]))
//				tname[30] = '\0';
//			wsprintfW(text,L"<%hs>",tname);
//		} else {
//			strncpy(tname,cFiles[c-mdirscount],32);
//			if(32 <= strlen(cFiles[c-mdirscount]))
//				tname[32] = '\0'; 
//			wsprintfW(text,L"%hs",tname);
//		}
//		if((i+coffset) == (cbrowse-1))
//		{
//			p_graph.RenderBrowserBar(x,y+tmpy,font.m_fFontHeight);
//			font.DrawText( x, y+tmpy, hlfc, text );
//		} else {
//			font.DrawText( x, y+tmpy, fc, text );
//		}
//
//		map<int,HDDBROWSEINFO>::iterator sel_iter;
//		sel_iter = selected_item.find(i+coffset);
//		if(sel_iter != selected_item.end( ))
//		//if((selected_item.size() > 0) && (selected_item[i+coffset].button == BUTTON_B))
//				p_graph.RenderBrowserBarSelected(x,y+tmpy,font.m_fFontHeight);
//
//	}
//	
//	return true;
//}

bool D2Xdbrowser::showDirBrowser2(float x,float y,int width,int widthpx,int vspace,int lines,DWORD fc,DWORD hlfc,DWORD sfc, const CStdString& font, DWORD dwFlags, bool scroll)
{
	WCHAR text[256];
	float tmpy=0;
	int c=0;
	char path[1024];
	CStdString	strName;
	D2Xgraphics	p_graph;
	if(width > 255)
		width = 255;

	if(lines <= 0)
		lines = 60;

	show_lines = lines;


	//strcpy(path,currentdir);
	strcpy(path,c_Dir.current_directory.c_str());
	strcat(path,"\\");

	// for the workaround
	if(vspace == 0)
        i_vspace = p_ml->getFontHeight(font);
	else
		i_vspace = vspace;

	v_space = i_vspace;

	start_x = x;
	start_y = y;

	for(int i=0;i<show_lines;i++)
	{
		c = i+coffset;
		tmpy = i*i_vspace;
		if(c >= (mdirscount+mfilescount))
			break;
		char tname[256];
		if(c < mdirscount)
		{		
			/*strncpy(tname,cDirs[c],width-2);
			if(30 <= strlen(cDirs[c]))
				tname[30] = '\0';
			wsprintfW(text,L"<%hs>",tname);*/
			strName.Format("<%s>",c_Dir.directories[c].substr(0,width-2));
		} 
		else 
		{
			/*strncpy(tname,cFiles[c-mdirscount],width);
			if(32 <= strlen(cFiles[c-mdirscount]))
				tname[32] = '\0'; 
			wsprintfW(text,L"%hs",tname);*/
			strName.Format("%s",c_Dir.files[c-mdirscount].substr(0,width));
		}
		if((i+coffset) == (cbrowse-1))
		{
			// workaround
			if(b_x == 0)
			{
				b_x = x;
				b_y = y+tmpy;
			}
			p_ml->DrawText(font, x, y+tmpy, hlfc, strName.c_str(), XBFONT_TRUNCATED, dwFlags, (float)widthpx, scroll );
		} 
		else 
		{
			map<int,HDDBROWSEINFO>::iterator sel_iter;
			sel_iter = selected_item.find(i+coffset);
			if(sel_iter != selected_item.end( ))
				p_ml->DrawText(font, x, y+tmpy, sfc, strName.c_str(), XBFONT_TRUNCATED, dwFlags, (float)widthpx, false );
			else
				p_ml->DrawText(font, x, y+tmpy, fc, strName.c_str(), XBFONT_TRUNCATED, dwFlags, (float)widthpx, false );
		}

	}
	
	return true;
}


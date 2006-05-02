#include "D2Xgui.h"

std::auto_ptr<D2Xgui> D2Xgui::sm_inst;

D2Xgui* D2Xgui::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xgui> tmp(new D2Xgui);
        sm_inst = tmp;
    }
    return sm_inst.get();
}


D2Xgui::D2Xgui()
{
	p_gm = NULL;
	p_vk = NULL;
	p_sg = NULL;
	p_v = NULL;
	a_browser[0] = NULL;
	a_browser[1] = NULL;
	prev_id = 0;
	skip_frame = false;
	context_counter = -1;

	p_input = D2Xinput::Instance();
}


int D2Xgui::LoadSkin(CStdString strSkinName)
{
	//p_ml = new D2Xmedialib();
	p_ml = D2Xmedialib::Instance();

	strSkin = strSkinName;

	CStdString	strValue;
	strValue = "q:\\skins\\"+strSkinName;

	p_ml->LoadMedia(strValue);

	// load the skin xmls
	LoadXML("mainmenu.xml");
	LoadXML("gamemanager.xml");
	LoadXML("filemanager.xml");
	LoadXML("keyboard.xml");
	LoadXML("settings.xml");
	LoadXML("viewer.xml");
	LoadXML("diskcopy.xml");
	LoadXML("error.xml");
	LoadXML("startup.xml");

	return 1;
}

int D2Xgui::LoadXML(CStdString strXMLName)
{
	CStdString	strValue;
	strValue = "q:\\skins\\"+strSkin+"\\"+strXMLName;
	TiXmlDocument* xmldoc;
	xmldoc = new TiXmlDocument();
	bool loadOkay = xmldoc->LoadFile(strValue);
	vXML.push_back(xmldoc);

	if ( !loadOkay )
		return 0;

	return 1;
}

void D2Xgui::SetKeyValue(CStdString key,CStdString value)
{
	map<CStdString,CStdString>::iterator ikey;

	ikey = strcText.find(key.c_str());
	if(ikey != strcText.end())
		ikey->second = value;
	else
        strcText.insert(pair<CStdString,CStdString>(key,value));
}

void D2Xgui::SetKeyInt(CStdString key,int value)
{
	map<CStdString,int>::iterator ikey;

	if(value < 0)
		value = 0;

	ikey = strcInt.find(key.c_str());
	if(ikey != strcInt.end())
		ikey->second = value;
	else
        strcInt.insert(pair<CStdString,int>(key,value));
}

int D2Xgui::getKeyInt(CStdString key)
{
	map<CStdString,int>::iterator ikey;

	ikey = strcInt.find(key.c_str());
	if(ikey != strcInt.end())
		return ikey->second;
	else
		return 0;
}

CStdString D2Xgui::getKeyValue(CStdString key)
{
	map<CStdString,CStdString>::iterator ikey;

	ikey = strcText.find(key.c_str());
	if(ikey != strcText.end())
		return ikey->second;
	else
		return "NA";
}

void D2Xgui::SetWindowObject(int id, D2Xswin* win)
{
	map<int,D2Xswin*>::iterator ikey;

	ikey = map_swin.find(id);
	if(ikey != map_swin.end())
		ikey->second = win;
	else
		map_swin.insert(pair<int,D2Xswin*>(id, win));
}

void D2Xgui::SetGMObject(D2XGM* gm)
{
	p_gm = gm;
}
void D2Xgui::SetVKObject(CXBVirtualKeyboard* vk)
{
	p_vk = vk;
}

void D2Xgui::SetSGObject(D2Xguiset* sg)
{
	p_sg = sg;
}

void D2Xgui::SetViewObject(D2Xviewer* v)
{
	p_v = v;
}

void D2Xgui::SetBrowserObject(int id, D2Xdbrowser* b)
{
	a_browser[id] = b;
}

void D2Xgui::SetShowIDs(int showid)
{
	v_showids.push_back(showid);
}

bool D2Xgui::IsShowIDinList(int showid)
{
	for(int i=0; i<v_showids.size();i++)
	{
		if(v_showids[i] == showid)
			return true;
	}
	return false;
}


void D2Xgui::DoClean()
{
	p_gm = NULL;
	p_vk = NULL;
	p_sg = NULL;
	p_v  = NULL;
	a_browser[0] = NULL;
	a_browser[1] = NULL;
	strcText.clear();
	strcInt.clear();
	map_swin.clear();
	v_showids.clear();
	p_ml->clearScrollCache();
	p_ml->clearSoundCache(200);
}	

float D2Xgui::getMenuPosXY(int XY, int id, int showID)
{
	float posX = 0.00,posY = 0.00;
	switch(id)
	{
	case GUI_MAINMENU:
		{
			switch(showID)
			{
			case 0:
			case 1:
				if(map_swin[1] != NULL)
					map_swin[1]->getXY(&posX,&posY);
				break;
			case 10:
				if(map_swin[2] != NULL)
					map_swin[2]->getXY(&posX,&posY);
				break;
			};
		}
		break;
	case GUI_GAMEMANAGER:
		{
			switch(showID)
			{
			case 0:
			case MODE_SHOWLIST:
				if(p_gm != NULL)
					p_gm->getXY(&posX,&posY);
				break;
			case MODE_OPTIONS:
				if(map_swin[1] != NULL)
					map_swin[1]->getXY(&posX,&posY);
				break;
			default:
				break;
			};
		}
		break;
	case GUI_FILEMANAGER:
		{
			switch(showID)
			{
			case 1:
				if(a_browser[0] != NULL)
						a_browser[0]->getXY(&posX,&posY);
				break;
			case 2:
				if(a_browser[1] != NULL)
						a_browser[1]->getXY(&posX,&posY);
				break;
			case 12:
			case 22:
			case 102:
			case 202:
			case 104:
			case 204:
			case 600:
				if(map_swin[1] != NULL)
					map_swin[1]->getXY(&posX,&posY);
				break;
			default:
				break;
			}
		}
		break;
	case GUI_SETTINGS:
		if(p_sg != NULL)
			p_sg->getXY(&posX,&posY);
		break;
	case GUI_VIEWER:
		if(p_v != NULL)
			p_v->getXY(&posX,&posY);
		break;
	case GUI_DISKCOPY:
		if(map_swin[1] != NULL)
			map_swin[1]->getXY(&posX,&posY);
		break;
	default:
		break;
	};
	if(XY == X)
        return posX;
	else
		return posY;
}

int D2Xgui::getMenuItems( int id, int showID, int* vspace)
{
	int items = 0, v_space = 0;
	switch(id)
	{
	case GUI_MAINMENU:
		{
			switch(showID)
			{
			case 0:
			case 1:
				if(map_swin[1] != NULL)
					items = map_swin[1]->getItems(&v_space);
				break;
			case 10:
				if(map_swin[2] != NULL)
					items = map_swin[2]->getItems(&v_space);
				break;
			};
		}
		break;
	case GUI_GAMEMANAGER:
		{
			switch(showID)
			{
			case 0:
			case MODE_SHOWLIST:
				if(p_gm != NULL)
					items = p_gm->getItems(&v_space);
				break;
			case MODE_OPTIONS:
				if(map_swin[1] != NULL)
					items = map_swin[1]->getItems(&v_space);
				break;
			default:
				break;
			};
		}
		break;
	case GUI_FILEMANAGER:
		{
			switch(showID)
			{
			case 1:
				if(a_browser[0] != NULL)
						items = a_browser[0]->getItems(&v_space);
				break;
			case 2:
				if(a_browser[1] != NULL)
						items = a_browser[1]->getItems(&v_space);
				break;
			case 12:
			case 22:
			case 102:
			case 202:
			case 104:
			case 204:
			case 600:
				if(map_swin[1] != NULL)
					items = map_swin[1]->getItems(&v_space);
				break;
			default:
				break;
			}
		}
		break;
	case GUI_SETTINGS:
		if(p_sg != NULL)
			items = p_sg->getItems(&v_space);
		break;
	case GUI_VIEWER:
		if(p_v != NULL)
			items = p_v->getItems(&v_space);
		break;
	case GUI_DISKCOPY:
		if(map_swin[1] != NULL)
			items = map_swin[1]->getItems(&v_space);
		break;
	default:
		break;
	};

	if(vspace != NULL)
		*vspace = v_space;

	return items;
}

float D2Xgui::getMenuOrigin(int XY, int id, int showID)
{
	float posX = 0.00,posY = 0.00;
	switch(id)
	{
	case GUI_MAINMENU:
		{
			switch(showID)
			{
			case 0:
			case 1:
				if(map_swin[1] != NULL)
					map_swin[1]->getOrigin(&posX,&posY);
				break;
			case 10:
				if(map_swin[2] != NULL)
					map_swin[2]->getOrigin(&posX,&posY);
				break;
			};
		}
		break;
	case GUI_GAMEMANAGER:
		{
			switch(showID)
			{
			case 0:
			case MODE_SHOWLIST:
				if(p_gm != NULL)
					p_gm->getOrigin(&posX,&posY);
				break;
			case MODE_OPTIONS:
				if(map_swin[1] != NULL)
					map_swin[1]->getOrigin(&posX,&posY);
				break;
			default:
				break;
			};
		}
		break;
	case GUI_FILEMANAGER:
		{
			switch(showID)
			{
			case 1:
				if(a_browser[0] != NULL)
						a_browser[0]->getOrigin(&posX,&posY);
				break;
			case 2:
				if(a_browser[1] != NULL)
						a_browser[1]->getOrigin(&posX,&posY);
				break;
			case 12:
			case 22:
			case 102:
			case 202:
			case 104:
			case 204:
			case 600:
				if(map_swin[1] != NULL)
					map_swin[1]->getOrigin(&posX,&posY);
				break;
			default:
				break;
			}
		}
		break;
	case GUI_SETTINGS:
		if(p_sg != NULL)
			p_sg->getOrigin(&posX,&posY);
		break;
	case GUI_VIEWER:
		if(p_v != NULL)
			p_v->getOrigin(&posX,&posY);
		break;
	case GUI_DISKCOPY:
		if(map_swin[1] != NULL)
			map_swin[1]->getOrigin(&posX,&posY);
		break;
	default:
		break;
	};
	if(XY == X)
        return posX;
	else
		return posY;
}

int D2Xgui::getContextCounter(CStdString str_context)
{
	int iContext;
	if(str_context == "Gamemanager")
	{
		iContext = D2X_GAMEMANAGER;
	}
	else if(str_context == "Settings")
	{
		iContext = D2X_SETTINGS;
	}
	else if(str_context == "Mainmenu")
	{
		iContext = D2X_MAINMENU;
	}
	else if(str_context == "Filemanager")
	{
		iContext = D2X_FILEMANAGER;
	}
	else if(str_context == "Smbcopy")
	{
		iContext = D2X_SMBCOPY;
	}
	else if(str_context == "Disccopy")
	{
		iContext = D2X_DISCCOPY;
	}
	return iContext;
}

void D2Xgui::getContext(int& mCounter)
{
	if(context_counter != -1)
	{
        mCounter = context_counter;
		context_counter = -1;
	}
}

void D2Xgui::RenderGUI(int id)
{
	if(id != prev_id)
	{
		if(prev_id == GUI_GAMEMANAGER)
			p_ml->UnloadGameIcons();
		//DoClean();
		prev_id = id;
		//return;
		skip_frame=2;
	}

	TiXmlElement*		itemElement;
	TiXmlNode*			itemNode;
	int showID=0;

	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.5f, 1.0f );

	xmlgui = vXML[id];
	itemElement = xmlgui->RootElement();
	if( !itemElement )
	{
		p_ml->DrawText("D2XDefaultFont",20,10,0xffff0000,"Error: Check the skin XML for this context.");
		return;
	}
	for( itemNode = itemElement->FirstChild( "item" );
	itemNode;
	itemNode = itemNode->NextSibling( "item" ) )
	{
		const TiXmlNode *pNode = itemNode->FirstChild("showID");
		if(pNode)
            showID = atoi(pNode->FirstChild()->Value());
		else
			showID = 0;

		if (!showID || IsShowIDinList(showID))
		{
			
			pNode = itemNode->FirstChild("type");
			if (pNode)
			{
				if(!_strnicmp(pNode->FirstChild()->Value(),"text",4))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0, width = 0, widthpx = 0;
					CStdString	font,text,color,stext, align, direction;
					DWORD c = 0, dwFlags = 0L;
					bool scroll = false;

					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuPosXY(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuPosXY(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

				
					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("widthpx");
					if (pNode)
						widthpx = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("scroll");
					if (pNode)
					{
						if(!_stricmp(pNode->FirstChild()->Value(),"yes"))
							scroll = true;
					}

					pNode = itemNode->FirstChild("align");
					if (pNode)
					{
						align = pNode->FirstChild()->Value();
						if(align == "center")
							dwFlags |= (D2XFONT_CENTER);
						else if(align == "right")
							dwFlags |= (D2XFONT_RIGHT);
						else 
							dwFlags |= (D2XFONT_LEFT);
					}

					pNode = itemNode->FirstChild("direction");
					if (pNode)
					{
						direction = pNode->FirstChild()->Value();
						if(direction == "vertical")
							dwFlags |= (D2XFONT_VERTICAL);
					}

					pNode = itemNode->FirstChild("color");
					if (pNode)
					{
						color = pNode->FirstChild()->Value();
						sscanf( color.c_str(),"%X",&c);
					}
					
					pNode = itemNode->FirstChild("text");
					if (pNode)
					{
						bool scan = true;
						basic_string <char>::size_type index1, index2;
						text = pNode->FirstChild()->Value();
						while(scan)
						{
							index1 = text.find ( "${" );
							if(index1 != -1)
							{
								index2 = text.find( "}" , index1);
								if(index2 != -1)
								{
									CStdString item = text.substr(index1+2,index2-index1-2);
									map<CStdString,CStdString>::iterator ikey;
									ikey = strcText.find(item.c_str());
									if(ikey != strcText.end())
										text.replace(index1, index2-index1+1, ikey->second);
									else
										text.replace(index1, index2-index1+1, " ");
								}
								else
									scan = false;
							}
							else
								scan = false;

						}
						
					}

					pNode = itemNode->FirstChild("font");
					if (pNode)
					{			
						font = pNode->FirstChild()->Value();
			
						bool scan = true;
						while(scan)
						{
							basic_string <char>::size_type index1 = 0;
							index1 = text.find ( "\\\\n" );
							if(index1 == -1)
							{
								scan = false;
								if(width != 0 && width < text.size())
								{
									stext = text.substr(0, width) + "...";
									p_ml->DrawText(font,posX,posY,c,stext,XBFONT_TRUNCATED, dwFlags, (float)widthpx,scroll);
								}
								else
									p_ml->DrawText(font,posX,posY,c,text,XBFONT_TRUNCATED, dwFlags, (float)widthpx,scroll);

							}
							else
							{
								if(width != 0 && width < index1)
								{
									stext = text.substr(0, width) + "...";
									p_ml->DrawText(font,posX,posY,c,stext,XBFONT_TRUNCATED, dwFlags, (float)widthpx,scroll);
								}
								else
								{
									p_ml->DrawText(font,posX,posY,c,text.substr(0, index1),XBFONT_TRUNCATED, dwFlags, (float)widthpx,scroll);
								}
								text = text.substr(index1+3);
								posY += p_ml->getFontHeight(font);

							}
						}
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"image",5))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0;
					CStdString	image;
					DWORD c = 0;

					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuPosXY(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuPosXY(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("texture");
					if (pNode)
					{			
						image = pNode->FirstChild()->Value();
						p_ml->RenderTexture2(image,posX,posY,width,height);
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"rect",4))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0, relwidth = 0;
					DWORD c = 0, f = 0;
					CStdString col, fill;
					
					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuPosXY(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuPosXY(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relwidth");
					if (showID && pNode)
					{
						CStdString		strID;
						width = atoi(pNode->FirstChild()->Value());
						pNode = itemNode->FirstChild("id");
						if (pNode)
							strID = pNode->FirstChild()->Value();

						int percent = getKeyInt(strID);
						percent = percent <= 100 ? percent : 100;
						relwidth = int(width*percent/100);
					}
					else
					{
						pNode = itemNode->FirstChild("width");
						if (pNode)
							width = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("fillcolor");
					if (pNode)
					{
						fill = pNode->FirstChild()->Value();
						sscanf( fill.c_str(),"%X",&f);
						p_graph.DrawRect(posX,posY,posX+width,posY+height,f,f);
					}
					
					pNode = itemNode->FirstChild("color");
					if (pNode)
					{
						col = pNode->FirstChild()->Value();
						sscanf( col.c_str(),"%X",&c);
						if(relwidth > 0)
							p_graph.DrawRect(posX,posY,posX+relwidth,posY+height,c,c);
						p_graph.DrawRectOutline(posX,posY,posX+width,posY+height,c);
					}	


				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"menu",4))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 255, hspace = 0, lines = 60,i = 0,vspace = 0, widthpx = 0;
					DWORD c = 0, h = 0, dwFlags = 0L;
					CStdString col, high, font, align;
					map<int,string>	str_items;
					bool scroll = false;

					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("widthpx");
					if (pNode)
						widthpx = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("align");
					if (pNode)
					{
						align = pNode->FirstChild()->Value();
						if(align == "center")
							dwFlags |= (D2XFONT_CENTER);
						else if(align == "right")
							dwFlags |= (D2XFONT_RIGHT);
						else 
							dwFlags |= (D2XFONT_LEFT);
					}

					pNode = itemNode->FirstChild("scroll");
					if (pNode)
					{
						if(!_stricmp(pNode->FirstChild()->Value(),"yes"))
							scroll = true;
					}

					pNode = itemNode->FirstChild("vspace");
					if (pNode)
						vspace = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("lines");
					if (pNode)
						lines = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("hspace");
					if (pNode)
						hspace = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("highlight");
					if (pNode)
					{
						high = pNode->FirstChild()->Value();
						sscanf( high.c_str(),"%X",&h);
					}
					
					pNode = itemNode->FirstChild("color");
					if (pNode)
					{
						col = pNode->FirstChild()->Value();
						sscanf( col.c_str(),"%X",&c);
					}
					for( pNode = itemNode->FirstChild( "label" );
					pNode;
					pNode = pNode->NextSibling( "label" ) )
					{
						str_items.insert(pair<int,string>(i++,pNode->FirstChild()->Value()));
					}

					pNode = itemNode->FirstChild("font");
					if(pNode)
					{
						font = pNode->FirstChild()->Value();

						// hacks for all different menu implementations :-(
						switch(id)
						{
						case GUI_MAINMENU:
							{
								switch(showID)
								{
								case 0:
								case 1:
									if(map_swin[1] != NULL)
									{
										map_swin[1]->refreshScrollWindowSTR(str_items);
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									}
									break;
								case 10:
									if(map_swin[2] != NULL)
									{
										map_swin[2]->refreshScrollWindowSTR(str_items);
										map_swin[2]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									}
									break;
								};
							}
							break;
						case GUI_GAMEMANAGER:
							{
								switch(showID)
								{
								case 0:
								case MODE_SHOWLIST:
									if(p_gm != NULL)
										p_gm->ShowGameMenu(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									break;
								case MODE_OPTIONS:
									if(map_swin[1] != NULL)
									{
										map_swin[1]->refreshScrollWindowSTR(str_items);
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									}
									break;
								default:
									break;
								};
							}
							break;
						case GUI_FILEMANAGER:
							{
								switch(showID)
								{
								case 102:
								case 202:
									if(map_swin[1] != NULL)
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									break;
								case 104:
								case 204:
									if(map_swin[1] != NULL)
									{
										map_swin[1]->refreshScrollWindowSTR(str_items);
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									}
									break;
								case 600:
									if(map_swin[1] != NULL)
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									if(map_swin[2] != NULL)
										map_swin[2]->showScrollWindowSTR2(posX+hspace,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									break;
								default:
									break;
								}
							}
							break;
						case GUI_SETTINGS:
							if(p_sg != NULL)
								p_sg->ShowGUISettings2(posX,posY,hspace,width,widthpx,vspace,c,h,font,dwFlags,scroll);
							break;
						case GUI_DISKCOPY:
							{
								switch(showID)
								{
								case 10:
									if(map_swin[1] != NULL)
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font, dwFlags,scroll);
									if(map_swin[2] != NULL)
										map_swin[2]->showScrollWindowSTR2(posX+hspace,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									break;
								case 100:
									if(map_swin[1] != NULL)
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,widthpx,vspace,lines,c,h,font,dwFlags,scroll);
									break;
								}
							}
							break;
						default:
							break;
						};
					}

				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"browser",7))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 255, win = 0, lines = 0, vspace = 0, widthpx = 0;
					DWORD c = 0, h = 0, s = 0, dwFlags=0L;
					CStdString col, high, sel, font, align;
					bool scroll = false;

					pNode = itemNode->FirstChild("showID");
					if (pNode)
					{
						win = atoi(pNode->FirstChild()->Value());
						win--;
						if(win < 0 || win > 1)
							win = 0;
					}

					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("widthpx");
					if (pNode)
						widthpx = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("align");
					if (pNode)
					{
						align = pNode->FirstChild()->Value();
						if(align == "center")
							dwFlags |= (D2XFONT_CENTER);
						else if(align == "right")
							dwFlags |= (D2XFONT_RIGHT);
						else 
							dwFlags |= (D2XFONT_LEFT);
					}

					pNode = itemNode->FirstChild("scroll");
					if (pNode)
					{
						if(!_stricmp(pNode->FirstChild()->Value(),"yes"))
							scroll = true;
					}

					pNode = itemNode->FirstChild("vspace");
					if (pNode)
						vspace = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("lines");
					if (pNode)
						lines = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("highlight");
					if (pNode)
					{
						high = pNode->FirstChild()->Value();
						sscanf( high.c_str(),"%X",&h);
					}
					
					pNode = itemNode->FirstChild("color");
					if (pNode)
					{
						col = pNode->FirstChild()->Value();
						sscanf( col.c_str(),"%X",&c);
					}

					pNode = itemNode->FirstChild("selected");
					if (pNode)
					{
						sel = pNode->FirstChild()->Value();
						sscanf( sel.c_str(),"%X",&s);
					}

					pNode = itemNode->FirstChild("font");
					if(pNode)
					{
						font = pNode->FirstChild()->Value();		
						if(a_browser[win] != NULL)
							a_browser[win]->showDirBrowser2(posX,posY,width,widthpx,vspace,lines,c,h,s,font,dwFlags,scroll);								
					}

				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"keyboard",8))
				{
					int posY_keyboard = 250, posY_textbox = 208;

					pNode = itemNode->FirstChild("posytextbox");
					if (pNode)
						posY_textbox = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posykeyboard");
					if (pNode)
						posY_keyboard = atoi(pNode->FirstChild()->Value());

					if (p_vk != NULL)
					{		
						p_vk->SetPosY(posY_textbox, posY_keyboard);
						p_vk->Render();
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"viewer",10))
				{
					int posX = 0,posY = 0;
					DWORD c = 0, h = 0;
					CStdString col, high, font;

					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());
					
					pNode = itemNode->FirstChild("highlight");
					if (pNode)
					{
						high = pNode->FirstChild()->Value();
						sscanf( high.c_str(),"%X",&h);
					}
					
					pNode = itemNode->FirstChild("color");
					if (pNode)
					{
						col = pNode->FirstChild()->Value();
						sscanf( col.c_str(),"%X",&c);
					}
					
					pNode = itemNode->FirstChild("font");
					if(pNode)
					{
						font = pNode->FirstChild()->Value();
						if (p_v != NULL)
						{			
							p_v->show2(posX,posY,c,h,font);
						}
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"dvdxbeicon",7))
				{

					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0;
					CStdString	image;
					DWORD c = 0;

					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuPosXY(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuPosXY(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					if(p_ml->IsTextureLoaded("DVDxbeIcon"))
					{
						p_ml->RenderTexture2("DVDxbeIcon",posX,posY,width,height);
					}
					else
					{
						pNode = itemNode->FirstChild("texture");
						if (pNode)
						{			
							image = pNode->FirstChild()->Value();
							p_ml->RenderTexture2(image,posX,posY,width,height);
						}
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"activexbeicon",13))
				{

					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0;
					CStdString	image,titleid;
					DWORD c = 0;

					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuPosXY(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuPosXY(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					titleid = getKeyValue("titleid");

					if(!p_ml->IsTextureLoaded(titleid) && p_ml->getTextureStatus(titleid) && titleid != "NA" && titleid != "")
					{
						p_ml->LoadTextureFromTitleID(titleid, getKeyValue("full_path")+"default.xbe", titleid, 0x00000000);
					}

					if(p_ml->IsTextureLoaded(titleid))
					{
						p_ml->RenderTexture2(titleid,posX,posY,width,height);
					}
					else
					{
						pNode = itemNode->FirstChild("texture");
						if (pNode)
						{			
							image = pNode->FirstChild()->Value();
							p_ml->RenderTexture2(image,posX,posY,width,height);
						}
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"defaultmenuback",15))
				{

					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0, items = 0, vspace = 0;
					CStdString	image;
					DWORD c = 0;

					pNode = itemNode->FirstChild("relX");
					if (pNode)
						posX = getMenuOrigin(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (pNode)
						posY = getMenuOrigin(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					items = getMenuItems(id,showID,&vspace);

					pNode = itemNode->FirstChild("vspace");
					if (pNode)
						vspace = atoi(pNode->FirstChild()->Value())+height;
			
						
					pNode = itemNode->FirstChild("texture");
					if (pNode)
					{	
						for(int i = 0;i < items; i++)
						{	
							
							image = pNode->FirstChild()->Value();
							p_ml->RenderTexture2(image,posX,posY+vspace*i,width,height);
							
						}
					}
					
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"sound",5))
				{

					const TiXmlNode *pNode;
											
					pNode = itemNode->FirstChild("name");
					if (pNode)
					{	
						p_ml->PlaySoundOnce(pNode->FirstChild()->Value());
					}
					
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"shortcut",8))
				{

					const TiXmlNode *pNode;
											
					if(pNode = itemNode->FirstChild("dpadright"))
					{	
						if(p_input->pressed(GP_DPAD_RIGHT))
						{
							context_counter = getContextCounter(pNode->FirstChild()->Value());
						}
					}
					if(pNode = itemNode->FirstChild("dpadleft"))
					{
						if(p_input->pressed(GP_DPAD_LEFT))
						{
							context_counter = getContextCounter(pNode->FirstChild()->Value());
						}
					}
					
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"UpArrow",5))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0;
					CStdString	image;
					DWORD c = 0;

					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuOrigin(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuOrigin(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("texture");
					if (pNode)
					{			
						image = pNode->FirstChild()->Value();
						switch(id)
						{
						case GUI_GAMEMANAGER:
							{
								switch(showID)
								{
								case 0:
								case MODE_SHOWLIST:
									if(p_gm != NULL)
									{
										if(p_gm->info.top_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);
									}
									break;
								default:
									break;
								};
							}
							break;
						case GUI_FILEMANAGER:
							{
								switch(showID)
								{
								case 1:
									if(a_browser[0] != NULL)
									{
										HDDBROWSEINFO tinfo;
										a_browser[0]->getInfo(&tinfo);
										if(tinfo.top_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);						
									}
									break;
								case 2:
									if(a_browser[1] != NULL)
									{
										HDDBROWSEINFO tinfo;
										a_browser[1]->getInfo(&tinfo);
										if(tinfo.top_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);	
									}
									break;
								case 102:
								case 202:
									if(map_swin[1] != NULL)
									{
										if(map_swin[1]->info.top_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);
									}
									break;
								default:
									break;
								}
							}
							break;
						case GUI_DISKCOPY:
							{
								switch(showID)
								{
								case 10:
								case 100:
									if(map_swin[1] != NULL)
									{
										if(map_swin[1]->info.top_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);
									}
									break;
								default:
									break;
								}
							}
						default:
							break;
						};
						
					}
				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"DownArrow",5))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0,height = 0;
					CStdString	image;
					DWORD c = 0;

					pNode = itemNode->FirstChild("relX");
					if (showID && pNode)
						posX = getMenuOrigin(X,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posX");
						if (pNode)
							posX = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("relY");
					if (showID && pNode)
						posY = getMenuOrigin(Y,id,showID) + atoi(pNode->FirstChild()->Value());
					else
					{
						pNode = itemNode->FirstChild("posY");
						if (pNode)
							posY = atoi(pNode->FirstChild()->Value());
					}

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("height");
					if (pNode)
						height = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("texture");
					if (pNode)
					{			
						image = pNode->FirstChild()->Value();
						switch(id)
						{
						case GUI_GAMEMANAGER:
							{
								switch(showID)
								{
								case 0:
								case MODE_SHOWLIST:
									if(p_gm != NULL)
									{
										if(p_gm->info.bottom_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);
									}
									break;
								default:
									break;
								};
							}
							break;
						case GUI_FILEMANAGER:
							{
								switch(showID)
								{
								case 1:
									if(a_browser[0] != NULL)
									{
										HDDBROWSEINFO tinfo;
										a_browser[0]->getInfo(&tinfo);
										if(tinfo.bottom_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);						
									}
									break;
								case 2:
									if(a_browser[1] != NULL)
									{
										HDDBROWSEINFO tinfo;
										a_browser[1]->getInfo(&tinfo);
										if(tinfo.bottom_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);	
									}
									break;
								case 102:
								case 202:
									if(map_swin[1] != NULL)
									{
										if(map_swin[1]->info.bottom_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);
									}
									break;
								default:
									break;
								}
							}
							break;
						case GUI_DISKCOPY:
							{
								switch(showID)
								{
								case 10:
								case 100:
									if(map_swin[1] != NULL)
									{
										if(map_swin[1]->info.bottom_items)
											p_ml->RenderTexture2(image,posX,posY,width,height);
									}
									break;
								default:
									break;
								}
							}
					
						default:
							break;
						};
						
					}
				}
			}

		}
	}

	if(getKeyInt("screensaver")==1)
		p_graph.ScreenSaver();

	p_ml->DoSoundWork();
	DoClean();

	if(skip_frame > 0)
	{
		skip_frame--;
		return;
	}
	
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
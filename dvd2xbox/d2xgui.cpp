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
	a_browser[0] = NULL;
	a_browser[1] = NULL;
	prev_id = 0;
}


int D2Xgui::LoadSkin(CStdString strSkinName)
{
	p_ml = new D2Xmedialib();

	strSkin = strSkinName;

	CStdString	strValue;
	strValue = "q:\\skins\\"+strSkinName;

	p_ml->LoadMedia(strValue);

	// load the skin xmls
	LoadXML("mainmenu.xml");
	LoadXML("gamemanager.xml");
	LoadXML("filemanager.xml");

	return 1;
}

int D2Xgui::LoadXML(CStdString strXMLName)
{
	CStdString	strValue;
	strValue = "q:\\skins\\"+strSkin+"\\"+strXMLName;
	TiXmlDocument* xmldoc;
	xmldoc = new TiXmlDocument();
	bool loadOkay = xmldoc->LoadFile(strValue);
	if ( !loadOkay )
		return 0;

	vXML.push_back(xmldoc);

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
	a_browser[0] = NULL;
	a_browser[1] = NULL;
	strcText.clear();
	map_swin.clear();
}	

void D2Xgui::RenderGUI(int id)
{
	if(id != prev_id)
	{
		DoClean();
		prev_id = id;
		return;
	}

	TiXmlElement*		itemElement;
	TiXmlNode*			itemNode;
	int showID=0;

	xmlgui = vXML[id];
	itemElement = xmlgui->RootElement();
	if( !itemElement )
		return;
	for( itemNode = itemElement->FirstChild( "item" );
	itemNode;
	itemNode = itemNode->NextSibling( "item" ) )
	{
		const TiXmlNode *pNode = itemNode->FirstChild("showID");
		if(pNode)
            showID = atoi(pNode->FirstChild()->Value());
		else
			showID = 0;
		//if (!showID || (showid == showID))
		if (!showID || IsShowIDinList(showID))
		{
			
			pNode = itemNode->FirstChild("type");
			if (pNode)
			{

				if(!_strnicmp(pNode->FirstChild()->Value(),"text",4))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0;
					CStdString	font,text,color;
					DWORD c = 0;

					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());

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
										text.replace(index1, index2-index1+1, "#");
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
							index1 = text.find ( "\\n" );
							if(index1 == -1)
							{
								scan = false;
								p_ml->DrawText(font,posX,posY,c,text);
							}
							else
							{
								p_ml->DrawText(font,posX,posY,c,text.substr(0, index1));
								text = text.substr(index1+2);
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

					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());

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
					int posX = 0,posY = 0,width = 0,height = 0;
					DWORD c = 0, f = 0;
					CStdString col, fill;
					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

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
						p_graph.DrawRectOutline(posX,posY,posX+width,posY+height,c);
					}


				}
				else if(!_strnicmp(pNode->FirstChild()->Value(),"menu",4))
				{
					const TiXmlNode *pNode;
					int posX = 0,posY = 0,width = 0;
					DWORD c = 0, h = 0;
					CStdString col, high, font;
					pNode = itemNode->FirstChild("posX");
					if (pNode)
						posX = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("posY");
					if (pNode)
						posY = atoi(pNode->FirstChild()->Value());

					pNode = itemNode->FirstChild("width");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

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

						// hacks for all different menu implementations :-(
						switch(id)
						{
						case GUI_MAINMENU:
							if(map_swin[1] != NULL)
								map_swin[1]->showScrollWindow2(posX,posY,width,c,h,font);
							break;
						case GUI_GAMEMANAGER:
							{
								switch(showID)
								{
								case 0:
									if(p_gm != NULL)
										p_gm->ShowGameMenu(posX,posY,width,c,h,font);
									break;
								case MODE_OPTIONS:
									if(map_swin[1] != NULL)
										map_swin[1]->showScrollWindowSTR2(posX,posY,width,c,h,font);
									break;
								default:
									break;
								};
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
					int posX = 0,posY = 0,width = 0, win = 0, lines = 0;
					DWORD c = 0, h = 0;
					CStdString col, high, font;

					pNode = itemNode->FirstChild("window");
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

					pNode = itemNode->FirstChild("rows");
					if (pNode)
						width = atoi(pNode->FirstChild()->Value());

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

					pNode = itemNode->FirstChild("font");
					if(pNode)
					{
						font = pNode->FirstChild()->Value();
						if(a_browser[win] != NULL)
							a_browser[win]->showDirBrowser2(lines, posX,posY,width,c,h,font);
				
					}

				}
			}

		}
	}
	v_showids.clear();
}
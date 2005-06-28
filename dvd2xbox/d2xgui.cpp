#include "D2Xgui.h"


D2Xgui::D2Xgui()
{
	p_win = NULL;
}

D2Xgui::~D2Xgui()
{

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

void D2Xgui::SetWindowObject(D2Xswin* win)
{
	p_win = win;
}

void D2Xgui::RenderGUI(int id)
{
	TiXmlElement*		itemElement;
	TiXmlNode*			itemNode;

	xmlgui = vXML[id];
	itemElement = xmlgui->RootElement();
	if( !itemElement )
		return;
	for( itemNode = itemElement->FirstChild( "item" );
	itemNode;
	itemNode = itemNode->NextSibling( "item" ) )
	{
		const TiXmlNode *pNode = itemNode->FirstChild("type");
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
					p_graph.DrawRect(posX,posY,width,height,f,f);
				}
				
				pNode = itemNode->FirstChild("color");
				if (pNode)
				{
					col = pNode->FirstChild()->Value();
					sscanf( col.c_str(),"%X",&c);
					p_graph.DrawRectOutline(posX,posY,width,height,c);
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
				if(pNode && p_win != NULL)
				{
					font = pNode->FirstChild()->Value();
					p_win->showScrollWindow2(posX,posY,width,c,h,font);
				}

			}
		}
	}
}
#include "d2xswindow.h"


D2Xswin::D2Xswin()
{
	//p_help = new HelperX();
}

D2Xswin::~D2Xswin()
{
	//delete p_help;
}

void D2Xswin::initScrollWindow(char** array,int lines2show,bool sortitems)
{
	showlines = lines2show;
	sort = sortitems;
	for(int i=0;i<itemscount;i++)
	{
		if(items[i])
		{
			delete[] items[i];
			items[i]=NULL;
		}
	}
	itemscount = 0;
	while(array[itemscount] != NULL)
	{
		items[itemscount] = new char[strlen(array[itemscount])+1];
		strcpy(items[itemscount],array[itemscount]);
		itemscount++;
	}
	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;
}


int compare_swin( const void *arg1, const void *arg2 )
{
   // Compare all of both strings:
   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
}

SWININFO D2Xswin::processScrollWindow(XBGAMEPAD pad)
{
	
	
	info.button = NO_PRESSED;

	if(sort)
        qsort( (void *)items, (size_t)itemscount, sizeof( char * ), compare_swin );
	

	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) {
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
	if((pad.fY1 > 0.5)) {
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
	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
		if(cbrowse < itemscount)
            cbrowse++;
		if(crelbrowse<showlines)
		{
            crelbrowse++;
		} else {
			if(coffset < (itemscount-showlines))
				coffset++;
		}
	}
	if(pad.fY1 < -0.5) {
		Sleep(100);
		if(cbrowse < itemscount)
            cbrowse++;
		if(crelbrowse<showlines)
		{
            crelbrowse++;
		} else {
			if(coffset < (itemscount-showlines))
				coffset++;
		}
	}

	strcpy(info.item,items[cbrowse-1]);
	/*
	if(p_help->pressSTART(pad))
	{	
		info.button =  BUTTON_START;
		return info;	
	}
	if(p_help->pressX(pad))
	{	
		info.button =  BUTTON_X;
		return info;	
	}

	if(p_help->pressA(pad))
	{
		info.button = BUTTON_A;
		return info;	
	}
	
	if(p_help->pressB(pad))
	{
		info.button = BUTTON_B;
		return info;	
	}

	if(p_help->pressRTRIGGER(pad))
	{
		info.button = BUTTON_RTRIGGER;
		return info;
	}
	if(p_help->pressBLACK(pad))
	{
		info.button = BUTTON_BLACK;
		return info;	
	}
	if(p_help->pressWHITE(pad))
	{
		info.button = BUTTON_WHITE;
		return info;	
	}*/
	
	return info;
}

void D2Xswin::showScrollWindow(float x,float y,int width,DWORD fc,DWORD hlfc, CXBFont &font)
{
	WCHAR text[128];
	float tmpy=0;
	int c=0;

	for(int i=0;i<showlines;i++)
	{
		c = i+coffset;
		tmpy = i*font.m_fFontHeight;
		if(c >= itemscount)
			break;
		
		char tname[128];
		strncpy(tname,items[c],width);
		if(width <= strlen(items[c]))
			tname[width] = '\0';
		wsprintfW(text,L"%hs",tname);
		 
		if((i+coffset) == (cbrowse-1))
		{
            font.DrawText( x, y+tmpy, hlfc, text );
		} else {
			font.DrawText( x, y+tmpy, fc, text );
		}

	}
}
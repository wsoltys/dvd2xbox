#ifndef D2XVIEWER
#define D2XVIEWER

#include <xtl.h>
#include <stdstring.h>
#include <vector>
#include "xbApplicationEx.h"
#include <xbfontEx.h>
#include "d2xgraphics.h"
#include "d2xmedialib.h"
#include "d2xinput.h"

#define VIEW_OK		0
#define VIEW_ERROR	1

#define BUFFER	40

#define START		100
#define	END			110
#define NEXT		120
#define PREVIOUS	130


class D2Xviewer
{
protected:

	class D2Xbuf
	{
	protected:
	public:
		D2Xbuf(){};
		~D2Xbuf(){};
		vector<string>		buf;
		string				file;
		string				file_stripped;
		int					lines;
		int					chars;
		int					max_chars;
		int					browse;
		int					yoffset;
		int					xoffset;
	};

	D2Xbuf			c_view;
	D2Xmedialib*	p_ml;
	D2Xinput*		p_input;
	float			v_x;
	float			v_y;
	unsigned short	start_x;
	unsigned short	start_y;
	unsigned short	v_space;

	int		fillBuffer();
	
public:
	D2Xviewer();
	~D2Xviewer();

	void	init(char* filename,int lines,int chars);
	void	reset();
	void	process(XBGAMEPAD gp);
	void	show(float x,float y,DWORD fc,DWORD hlfc, CXBFont &font);

	void	show2(float x,float y,DWORD fc,DWORD hlfc, const CStdString& font);
	int		getRow();
	int		getAllRows();
	int		getCol();
	int		getAllCols();
	void	getXY(float* posX, float* posY);
	void	getOrigin(float* posX, float* posY);
	int		getItems(int* vspace=NULL);
	string	getFileName();
	

};

#endif
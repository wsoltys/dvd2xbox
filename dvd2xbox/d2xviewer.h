#ifndef D2XVIEWER
#define D2XVIEWER

#include <xtl.h>
#include <stdstring.h>
#include <vector>
#include "xbApplicationEx.h"
#include <XBFont.h>
#include "d2xgraphics.h"

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

	D2Xbuf	c_view;

	int		fillBuffer();
	
public:
	D2Xviewer();
	~D2Xviewer();

	void	init(char* filename,int lines,int chars);
	void	reset();
	void	process(XBGAMEPAD gp);
	void	show(float x,float y,DWORD fc,DWORD hlfc, CXBFont &font);
							
	

};

#endif
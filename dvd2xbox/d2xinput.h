#ifndef D2XINPUT
#define D2XINPUT

#include <xtl.h>
#include "xbApplicationEx.h"
#include <memory>

#define INPUT_IR	100

#define GP_A		10200
#define GP_B		10210
#define GP_X		10220
#define GP_Y		10230
#define GP_LTRIGGER	10240
#define GP_RTRIGGER	10250
#define GP_BLACK	10260
#define GP_WHITE	10270
#define GP_BACK		10280
#define GP_START	10290
#define GP_TL_LEFT	10300
#define GP_TL_RIGHT	10310
#define GP_DPAD_RIGHT	10320
#define GP_DPAD_LEFT	10330
#define GP_DPAD_UP		10340
#define GP_DPAD_DOWN	10350
#define GP_TL			10360
#define GP_LTRIGGER_P	10370
#define GP_RTRIGGER_P	10380

#define IR_LEFT		11200
#define IR_RIGHT	11210
#define IR_UP		11220
#define IR_DOWN		11230
#define IR_SELECT	11240
#define	IR_BACK		11250
#define IR_MENU		11260
#define IR_TITLE	11270
#define	IR_INFO		11280
#define IR_SKIP_MINUS	11290
#define IR_SKIP_PLUS	11300
#define IR_PLAY		11310
#define IR_REVERSE	11320
#define IR_FORWARD	11330
#define IR_DISPLAY	11340

#define C_UP		12200
#define C_DOWN		12210
#define C_RIGHT		12220
#define C_LEFT		12230

class D2Xinput
{
protected:
	XBGAMEPAD*	gp;
	XBIR_REMOTE* ir;
	int			ret;
	bool		locked;

	static std::auto_ptr<D2Xinput> sm_inst;
	D2Xinput();

public:
	static D2Xinput* Instance();

	void update(XBGAMEPAD* gamepad, XBIR_REMOTE* iremote);
	void update(XBGAMEPAD* gamepad);
	bool pressed(int button);
	void Lock();
	void Unlock();
};

#endif
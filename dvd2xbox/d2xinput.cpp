#include "D2Xinput.h"


D2Xinput::D2Xinput()
{

}


D2Xinput::~D2Xinput()
{	

}


void D2Xinput::update(XBGAMEPAD gamepad, XBIR_REMOTE iremote)
{
	gp = gamepad;
	ir = iremote;
}

void D2Xinput::update(XBGAMEPAD gamepad)
{
	gp = gamepad;
}

bool D2Xinput::pressed(int button)
{
	switch(button)
	{
	// Gamepad
	case GP_A:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_A]))
			return true;
		break;
	case GP_B:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_B]))
			return true;
		break;
	case GP_X:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_X]))
			return true;
		break;
	case GP_Y:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_Y]))
			return true;
		break;
	case GP_LTRIGGER:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]))
			return true;
		break;
	case GP_RTRIGGER:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]))
			return true;
		break;
	case GP_BLACK:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]))
			return true;
		break;
	case GP_WHITE:
		if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]))
			return true;
		break;
	case GP_BACK:
		if((gp.wPressedButtons & XINPUT_GAMEPAD_BACK))
			return true;
		break;
	case GP_START:
		if(gp.wPressedButtons & XINPUT_GAMEPAD_START)
			return true;
		break;
	case GP_TL_LEFT:
		if(gp.fX1 < -0.5)
			return true;
		break;
	case GP_TL_RIGHT:
		if(gp.fX1 > 0.5)
			return true;
		break;
	};
	return false;
}
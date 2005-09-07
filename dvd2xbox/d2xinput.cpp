#include "D2Xinput.h"


D2Xinput::D2Xinput()
{
	ret = false;
}


D2Xinput::~D2Xinput()
{	

}


void D2Xinput::update(XBGAMEPAD* gamepad, XBIR_REMOTE* iremote)
{
	gp = gamepad;
	ir = iremote;
}

void D2Xinput::update(XBGAMEPAD* gamepad)
{
	gp = gamepad;
}

bool D2Xinput::pressed(int button)
{

	ret = false;

	switch(button)
	{
	// Gamepad
	case GP_A:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_A]))
			ret = true;
		/*else if(ir->wPressedButtons == XINPUT_IR_REMOTE_SELECT)
			ret = true;*/
		break;
	case GP_B:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_B]))
			ret = true;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_BACK)
			ret = true;
		break;
	case GP_X:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_X]))
			ret = true;
		break;
	case GP_Y:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_Y]))
			ret = true;
		break;
	case GP_LTRIGGER:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]))
			ret = true;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_MINUS)
			ret = true;
		break;
	case GP_RTRIGGER:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]))
			ret = true;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_PLUS)
			ret = true;
		break;
	case GP_BLACK:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]))
			ret = true;
		break;
	case GP_WHITE:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]))
			ret = true;
		break;
	case GP_BACK:
		if((gp->wPressedButtons & XINPUT_GAMEPAD_BACK))
			ret = true;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_BACK)
			ret = true;
		break;
	case GP_START:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_START)
			ret = true;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_SELECT)
			ret = true;
		break;
	case GP_TL_LEFT:
		if(gp->fX1 < -0.5)
			ret = true;
		break;
	case GP_TL_RIGHT:
		if(gp->fX1 > 0.5)
			ret = true;
		break;
	case IR_LEFT:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_LEFT)
			ret = true;
		break;
	case IR_RIGHT:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_RIGHT)
			ret = true;
		break;
	case IR_UP:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_UP)
			ret = true;
		break;
	case IR_DOWN:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_DOWN)
			ret = true;
		break;
	case IR_SELECT:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_SELECT)
			ret = true;
		break;
	case IR_BACK:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_BACK)
			ret = true;
		break;
	case IR_MENU:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_MENU)
			ret = true;
		break;
	case IR_TITLE:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_TITLE)
			ret = true;
		break;
	case IR_INFO:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_INFO)
			ret = true;
		break;
	case IR_SKIP_MINUS:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_MINUS)
			ret = true;
		break;
	case IR_SKIP_PLUS:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_PLUS)
			ret = true;
		break;
	default:
		break;
	};

	if(ret == true)
		ir->wPressedButtons = 0;

	return ret;
}
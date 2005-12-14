#include "D2Xinput.h"

std::auto_ptr<D2Xinput> D2Xinput::sm_inst;

D2Xinput::D2Xinput()
{
	ret = false;
	locked = false;
}

D2Xinput* D2Xinput::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xinput> tmp(new D2Xinput);
        sm_inst = tmp;
    }
    return sm_inst.get();
}



void D2Xinput::update(XBGAMEPAD* gamepad, XBIR_REMOTE* iremote)
{
	gp = gamepad;
	ir = iremote;
}

void D2Xinput::Lock()
{
	locked = true;
}

void D2Xinput::Unlock()
{
	locked = false;
}


bool D2Xinput::pressed(int button)
{

	if(locked)
		return false;

	ret = 0;

	switch(button)
	{
	// Gamepad
	case GP_A:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_A]))
			ret = 1;
		break;
	case GP_B:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_B]))
			ret = 1;
		/*else if(ir->wPressedButtons == XINPUT_IR_REMOTE_BACK)
			ret = 1;*/
		break;
	case GP_X:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_X]))
			ret = 1;
		break;
	case GP_Y:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_Y]))
			ret = 1;
		break;
	case GP_LTRIGGER:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]))
			ret = 1;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_MINUS)
			ret = 1;
		break;
	case GP_RTRIGGER:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]))
			ret = 1;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_PLUS)
			ret = 1;
		break;
	case GP_BLACK:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]))
			ret = 1;
		break;
	case GP_WHITE:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]))
			ret = 1;
		break;
	case GP_BACK:
		if((gp->wPressedButtons & XINPUT_GAMEPAD_BACK))
			ret = 1;
		else if(ir->wPressedButtons == XINPUT_IR_REMOTE_BACK)
			ret = 1;
		break;
	case GP_START:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_START)
			ret = 1;
		/*else if(ir->wPressedButtons == XINPUT_IR_REMOTE_DISPLAY)
			ret = 1;*/
		break;
	case GP_TL:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB)
			ret = 1;
		break;
	case GP_TL_LEFT:
		if(gp->fX1 < -0.5)
			ret = 1;
		break;
	case GP_TL_RIGHT:
		if(gp->fX1 > 0.5)
			ret = 1;
		break;
	case GP_DPAD_RIGHT:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			ret = 1;
		break;
	case GP_DPAD_LEFT:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			ret = 1;
		break;
	case GP_DPAD_UP:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
			ret = 1;
		break;
	case GP_DPAD_DOWN:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			ret = 1;
		break;
	case IR_LEFT:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_LEFT)
			ret = INPUT_IR;
		break;
	case IR_RIGHT:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_RIGHT)
			ret = INPUT_IR;
		break;
	case IR_UP:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_UP)
			ret = INPUT_IR;
		break;
	case IR_DOWN:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_DOWN)
			ret = INPUT_IR;
		break;
	case IR_SELECT:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_SELECT)
			ret = IR_SELECT;
		break;
	case IR_BACK:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_BACK)
			ret = INPUT_IR;
		break;
	case IR_MENU:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_MENU)
			ret = INPUT_IR;
		break;
	case IR_TITLE:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_TITLE)
			ret = INPUT_IR;
		break;
	case IR_INFO:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_INFO)
			ret = INPUT_IR;
		break;
	case IR_SKIP_MINUS:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_MINUS)
			ret = INPUT_IR;
		break;
	case IR_SKIP_PLUS:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_SKIP_PLUS)
			ret = INPUT_IR;
		break;
	case IR_REVERSE:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_REVERSE)
			ret = INPUT_IR;
		break;
	case IR_FORWARD:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_FORWARD)
			ret = INPUT_IR;
		break;
	case IR_PLAY:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_PLAY)
			ret = INPUT_IR;
		break;
	case IR_DISPLAY:
		if(ir->wPressedButtons == XINPUT_IR_REMOTE_DISPLAY)
			ret = INPUT_IR;
		break;
	case C_UP:
		if(pressed(GP_DPAD_UP) || pressed(IR_UP))
			ret = 1;
		break;
	case C_DOWN:
		if(pressed(GP_DPAD_DOWN) || pressed(IR_DOWN))
			ret = 1;
		break;
	case C_RIGHT:
		if(pressed(GP_DPAD_RIGHT) || pressed(IR_RIGHT))
			ret = 1;
		break;
	case C_LEFT:
		if(pressed(GP_DPAD_LEFT) || pressed(IR_LEFT))
			ret = 1;
		break;
	default:
		break;
	};

	/*if(ret == INPUT_IR)
		Sleep(200);
	else if(ret == IR_SELECT)
		Sleep(500);*/

	if(ret > 0)
		return true;
	else
		return false;
}
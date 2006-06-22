#include "D2Xinput.h"

std::auto_ptr<D2Xinput> D2Xinput::sm_inst;

D2Xinput::D2Xinput()
{
	ret = false;
	locked = false;
	p_ml = D2Xmedialib::Instance();
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
	g_Keyboard.Update();
	dWord = g_Keyboard.GetKey();
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
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_A]) || dWord==65 || dWord==13)
		{
			ret = 1;
			p_ml->PlayKeySound("gp_a");
		}
		break;
	case GP_B:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_B]) || dWord==66)
		{
			ret = 1;
			p_ml->PlayKeySound("gp_b");
		}
		break;
	case GP_X:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_X]) || dWord==88)
		{
			ret = 1;
			p_ml->PlayKeySound("gp_x");
		}
		break;
	case GP_Y:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_Y]) || dWord==90)
		{
			ret = 1;
			p_ml->PlayKeySound("gp_y");
		}
		break;
	case GP_LTRIGGER:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]))
			ret = 1;
		else if(ir->wButtons == XINPUT_IR_REMOTE_SKIP_MINUS)
			ret = 1;
		break;
	case GP_LTRIGGER_P:
		if((gp->bLastAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]))
			ret = 1;
		break;
	case GP_RTRIGGER:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]))
			ret = 1;
		else if(ir->wButtons == XINPUT_IR_REMOTE_SKIP_PLUS)
			ret = 1;
		break;
	case GP_RTRIGGER_P:
		if((gp->bLastAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]))
			ret = 1;
		break;
	case GP_BLACK:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]) || dWord==112)
		{
			ret = 1;
			p_ml->PlayKeySound("black");
		}
		break;
	case GP_WHITE:
		if((gp->bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]) || dWord==113)
		{
			ret = 1;
			p_ml->PlayKeySound("white");
		}
		break;
	case GP_BACK:
		if((gp->wPressedButtons & XINPUT_GAMEPAD_BACK) || dWord==27)
		{
			ret = 1;
			p_ml->PlayKeySound("back");
		}
		else if(ir->wButtons == XINPUT_IR_REMOTE_BACK)
		{
			ret = 1;
			p_ml->PlayKeySound("back");
		}
		break;
	case GP_START:
		if((gp->wPressedButtons & XINPUT_GAMEPAD_START) || dWord==13)
		{
			ret = 1;
			p_ml->PlayKeySound("start");
		}
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
	case GP_DPAD_RIGHT_P:
		if(gp->wLastButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			ret = 1;
		break;
	case GP_DPAD_LEFT:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			ret = 1;
		break;
	case GP_DPAD_LEFT_P:
		if(gp->wLastButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			ret = 1;
		break;
	case GP_DPAD_UP:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
			ret = 1;
		break;
	case GP_DPAD_UP_P:
		if(gp->wLastButtons & XINPUT_GAMEPAD_DPAD_UP)
			ret = 1;
		break;
	case GP_DPAD_DOWN:
		if(gp->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			ret = 1;
		break;
	case GP_DPAD_DOWN_P:
		if(gp->wLastButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			ret = 1;
		break;
	case IR_LEFT:
		if(ir->wButtons == XINPUT_IR_REMOTE_LEFT)
			ret = INPUT_IR;
		break;
	case IR_RIGHT:
		if(ir->wButtons == XINPUT_IR_REMOTE_RIGHT)
			ret = INPUT_IR;
		break;
	case IR_UP:
		if(ir->wButtons == XINPUT_IR_REMOTE_UP)
			ret = INPUT_IR;
		break;
	case IR_DOWN:
		if(ir->wButtons == XINPUT_IR_REMOTE_DOWN)
			ret = INPUT_IR;
		break;
	case IR_SELECT:
		if(ir->wButtons == XINPUT_IR_REMOTE_SELECT)
		{
			ret = IR_SELECT;
			p_ml->PlayKeySound("gp_a");
		}
		break;
	case IR_BACK:
		if(ir->wButtons == XINPUT_IR_REMOTE_BACK)
		{
			ret = INPUT_IR;
			p_ml->PlayKeySound("back");
		}
		break;
	case IR_MENU:
		if(ir->wButtons == XINPUT_IR_REMOTE_MENU)
			ret = INPUT_IR;
		break;
	case IR_TITLE:
		if(ir->wButtons == XINPUT_IR_REMOTE_TITLE)
			ret = INPUT_IR;
		break;
	case IR_INFO:
		if(ir->wButtons == XINPUT_IR_REMOTE_INFO)
			ret = INPUT_IR;
		break;
	case IR_SKIP_MINUS:
		if(ir->wButtons == XINPUT_IR_REMOTE_SKIP_MINUS)
			ret = INPUT_IR;
		break;
	case IR_SKIP_PLUS:
		if(ir->wButtons == XINPUT_IR_REMOTE_SKIP_PLUS)
			ret = INPUT_IR;
		break;
	case IR_REVERSE:
		if(ir->wButtons == XINPUT_IR_REMOTE_REVERSE)
			ret = INPUT_IR;
		break;
	case IR_FORWARD:
		if(ir->wButtons == XINPUT_IR_REMOTE_FORWARD)
			ret = INPUT_IR;
		break;
	case IR_PLAY:
		if(ir->wButtons == XINPUT_IR_REMOTE_PLAY)
			ret = INPUT_IR;
		break;
	case IR_DISPLAY:
		if(ir->wButtons == XINPUT_IR_REMOTE_DISPLAY)
			ret = INPUT_IR;
		break;
	case C_UP:
		if(pressed(GP_DPAD_UP) || pressed(IR_UP) || dWord==38)
		{
			ret = 1;
			p_ml->PlayKeySound("up");
		}
		break;
	case C_DOWN:
		if(pressed(GP_DPAD_DOWN) || pressed(IR_DOWN) || dWord==40)
		{
			ret = 1;
			p_ml->PlayKeySound("down");
		}
		break;
	case C_RIGHT:
		if(pressed(GP_DPAD_RIGHT) || pressed(IR_RIGHT) || dWord==39)
		{
			ret = 1;
			p_ml->PlayKeySound("right");
		}
		break;
	case C_LEFT:
		if(pressed(GP_DPAD_LEFT) || pressed(IR_LEFT) || dWord==37)
		{
			ret = 1;
			p_ml->PlayKeySound("left");
		}
		break;
	case C_UP_P:
		if(pressed(GP_DPAD_UP_P) || pressed(IR_UP) || dWord==38)
		{
			ret = 1;
			p_ml->PlayKeySound("up");
		}
		break;
	case C_DOWN_P:
		if(pressed(GP_DPAD_DOWN_P) || pressed(IR_DOWN) || dWord==40)
		{
			ret = 1;
			p_ml->PlayKeySound("down");
		}
		break;
	case C_RIGHT_P:
		if(pressed(GP_DPAD_RIGHT_P) || pressed(IR_RIGHT) || dWord==39)
		{
			ret = 1;
			p_ml->PlayKeySound("right");
		}
		break;
	case C_LEFT_P:
		if(pressed(GP_DPAD_LEFT_P) || pressed(IR_LEFT) || dWord==37)
		{
			ret = 1;
			p_ml->PlayKeySound("left");
		}
		break;
	case C_SELECT:
		if(pressed(GP_A) || pressed(IR_SELECT) || dWord==13)
		{
			ret = 1;
			p_ml->PlayKeySound("gp_a");
		}
		break;
	default:
		break;
	};


	if(ret > 0)
		return true;
	else
		return false;
}
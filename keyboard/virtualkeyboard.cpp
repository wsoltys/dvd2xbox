//-----------------------------------------------------------------------------
// File: VirtualKeyboard.cpp
//
// Desc: Virtual keyboard reference UI
//
// Note: This sample is intended to show appropriate functionality only.
//       Please do not lift the graphics for use in your game. A description
//       of the user research that went into the creation of this sample is
//       located in the XDK documentation at Developing for Xbox - Reference
//       User Interface.
//
// Hist: 02.13.01 - New for March XDK release 
//       03.07.01 - Localized for April XDK release
//       04.10.01 - Updated for May XDK with full translations
//       06.06.01 - Japanese keyboard added
//       07.22.02 - Japanese keyboard (keyboard) added
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "VirtualKeyboard.h"
#include <cassert>
#include <algorithm>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"


const UINT XC_LANGUAGE_MAX = XC_LANGUAGE_ITALIAN + 1;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// These are the widths of the keys on the display. GAP2 is the distance
// between the mode keys on the left and the rest of the keyboard. The safe
// title area is 512 pixels across, so these values must meet the following
// guideline: MODEKEY_WIDTH + GAP2_WIDTH + (10*KEY_WIDTH) + (9*GAP_WIDTH) <=512
const DWORD GAP_WIDTH     = 0;
const DWORD GAP2_WIDTH    = 4;
const DWORD MODEKEY_WIDTH = 110;
const DWORD KEY_INSET     = 1;

const DWORD MAX_KEYS_PER_ROW = 14;

// Must be this far from center on 0.0 - 1.0 scale
const FLOAT JOY_THRESHOLD = 0.25f; 

// How often (per second) the caret blinks
const FLOAT fCARET_BLINK_RATE = 1.0f;

// During the blink period, the amount the caret is visible. 0.5 equals
// half the time, 0.75 equals 3/4ths of the time, etc.
const FLOAT fCARET_ON_RATIO = 0.75f;

// Text colors for keys
const D3DCOLOR COLOR_HIGHLIGHT     = 0xff00ff00;   // green
const D3DCOLOR COLOR_PRESSED       = 0xff808080;   // gray
const D3DCOLOR COLOR_NORMAL        = 0xff000000;   // black
const D3DCOLOR COLOR_DISABLED      = 0xffffffff;   // white
const D3DCOLOR COLOR_HELPTEXT      = 0xffffffff;   // white
const D3DCOLOR COLOR_FONT_DISABLED = 0xff808080;   // gray
const D3DCOLOR COLOR_INVISIBLE     = 0xff0000ff;   // blue
const D3DCOLOR COLOR_RED           = 0xffff0000;   // red

// Controller repeat values
const FLOAT fINITIAL_REPEAT = 0.333f; // 333 mS recommended for first repeat
const FLOAT fSTD_REPEAT     = 0.085f; // 85 mS recommended for repeat rate

// Maximum number of characters in string
const DWORD MAX_CHARS = 64;

// Width of text box
const FLOAT fTEXTBOX_WIDTH = 576.0f - 64.0f - 4.0f - 4.0f - 10.0f;

const FLOAT BUTTON_Y_POS    = 411.0f;      // button text line
const FLOAT BUTTON_X_OFFSET =  40.0f;      // space between button and text
const D3DCOLOR BUTTONTEXT_COLOR  = 0xffffffff;
const FLOAT FIXED_JSL_SIZE = 3.0f;

// Xboxdings font button mappings
const WCHAR* TEXT_A_BUTTON = L"A";
const WCHAR* TEXT_B_BUTTON = L"B";
const WCHAR* TEXT_X_BUTTON = L"C";
const WCHAR* TEXT_Y_BUTTON = L"D";





//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// Localizable String Data
const WCHAR* g_strEnglish[] =
{
    L"English",
    L"Choose Keyboard",
    L"Sample graphics. Don't use in your game",
    L"Select",
    L"Back",
    L"Help",
    L"SPACE",
    L"BACKSPACE",
    L"SHIFT",
    L"CAPS LOCK",
    L"ALPHABET",
    L"SYMBOLS",
    L"ACEENTS",
    L"DONE",
    L"Select",
    L"Cancel",
    L"Toggle\nmode",
    L"Display help",
    L"Backspace",
    L"Space",
    L"Trigger buttons move cursor",
};

const WCHAR** g_pStringTable = g_strEnglish;    // Current string table

// keyboard information
CXBVirtualKeyboard::KeyboardInfo g_aKeyboardInfo[] =
{
    CXBVirtualKeyboard::KeyboardInfo( XC_LANGUAGE_ENGLISH, 0, g_strEnglish ),
};
const UINT KEYBOARD_MAX = sizeof( g_aKeyboardInfo ) / sizeof( g_aKeyboardInfo[0] );






//-----------------------------------------------------------------------------
// Name: CXBVirtualKeyboard::Key()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBVirtualKeyboard::Key::Key( Xkey xk, DWORD w )
:
    xKey( xk ),
    dwWidth( w ),
    strName()
{
    // Special keys get their own names
    switch( xKey )
    {
        case XK_SPACE:
            strName = GetString( STR_KEY_SPACE );
            break;
        case XK_BACKSPACE:
            strName = GetString( STR_KEY_BACKSPACE );
            break;
        case XK_SHIFT:
            strName = GetString( STR_KEY_SHIFT );
            break;
        case XK_CAPSLOCK:
            strName = GetString( STR_KEY_CAPSLOCK );
            break;
        case XK_ALPHABET:
            strName = GetString( STR_KEY_ALPHABET );
            break;
        case XK_SYMBOLS:
            strName = GetString( STR_KEY_SYMBOLS );
            break;
        case XK_ACCENTS:
            strName = GetString( STR_KEY_ACCENTS );
            break;
        case XK_OK:
            strName = GetString( STR_KEY_DONE );
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: CXBVirtualKeyboard()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBVirtualKeyboard::CXBVirtualKeyboard()
//:CGUIDialog(1000)
{
    m_bIsCapsLockOn  = FALSE;
    m_bIsShiftOn     = FALSE;
    m_State          = STATE_KEYBOARD;
    m_iPos           = 0;
    m_iCurrBoard     = TYPE_ALPHABET;
    m_iCurrRow       = 0;
    m_iCurrKey       = 0;
    m_iLastColumn    = 0;
    m_fRepeatDelay   = fINITIAL_REPEAT;
    m_pKeyTexture    = NULL;
    
    m_pCurrKeyboard  = g_aKeyboardInfo;
    m_fKeyHeight     = 42.0f;
    m_dwMaxRows      = 5;
    m_bConfirmed=false;
    m_CaretTimer.Start();

    SelectKeyboard( 0 );
    InitBoard();
}


bool CXBVirtualKeyboard::IsConfirmed() const
{
  return m_bConfirmed;
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Sets up the virtual keyboard example
//-----------------------------------------------------------------------------
HRESULT CXBVirtualKeyboard::Initialize()
{
    // Create the resources
	if( FAILED( m_xprResource.Create( "Resource.xpr", resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the matrices
    /*
    D3DXVECTOR3 vEye(-2.5f, 2.0f, -4.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    g_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/
    // Arial Unicode MS 18, regular, 32-376, for keys
    if( FAILED( m_Font18.Create( "Font18.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Arial 12, bold, 32-255, for capital words on keys
    if( FAILED( m_Font12.Create( "Font12.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Xbox dingbats (buttons) 24
    if( FAILED( m_FontButtons.Create( "Xboxdings_24.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;


    // Validate key sizes
    assert( MODEKEY_WIDTH + GAP2_WIDTH + (10 * KEY_WIDTH) + (9 * GAP_WIDTH) <= 512 );

    // Create the keyboard key texture
    m_pKeyTexture = m_xprResource.GetTexture( resource_KeyboardKey_OFFSET );

    return S_OK;
}

void CXBVirtualKeyboard::Reset()
{
  
    m_bConfirmed=false;
    m_bIsCapsLockOn  = FALSE;
    m_bIsShiftOn     = FALSE;
    m_State          = STATE_KEYBOARD;
    m_iPos           = 0;
    m_iCurrBoard     = TYPE_ALPHABET;
    m_iCurrRow       = 0;
    m_iCurrKey       = 0;
    m_iLastColumn    = 0;
    m_fRepeatDelay   = fINITIAL_REPEAT;
    //m_pKeyTexture    = NULL;
    
    m_pCurrKeyboard  = g_aKeyboardInfo;
    m_fKeyHeight     = 42.0f;
    m_dwMaxRows      = 5;
    m_iPos=0;
    m_strData=L"";
    m_CaretTimer.Start();

    SelectKeyboard( 0 );
    InitBoard();
}

const WCHAR* CXBVirtualKeyboard::GetText()
{
  return m_strData.c_str();
}
void CXBVirtualKeyboard::SetText(const WCHAR* wszText)
{
  m_strData=wszText;
}
//void CXBVirtualKeyboard::OnAction(const CAction &action)
void CXBVirtualKeyboard::OnAction(int action)
{
	Event ev;
	switch (action)
	{
		
		case ACTION_SELECT_ITEM:
			ev=EV_A_BUTTON;
			UpdateState( ev );
		break;

		case ACTION_MOVE_DOWN:
			ev=EV_DOWN;
			UpdateState( ev );
		break;

		case ACTION_MOVE_UP:
			ev=EV_UP;
			UpdateState( ev );
		break;

		case ACTION_MOVE_LEFT:
			ev=EV_LEFT;
			UpdateState( ev );
		break;

		case ACTION_MOVE_RIGHT:
			ev=EV_RIGHT;
			UpdateState( ev );
		break;

		case ACTION_PARENT_MENU: // FIXME
			ev=EV_BACK_BUTTON;
			UpdateState( ev );
		break;
	
	}
}
//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame; the entry point for animating the scene
//-----------------------------------------------------------------------------
void CXBVirtualKeyboard::FrameMove()
{
    
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d rendering.
//       This function sets up render states, clears the viewport, and renders
//       the scene.
//-----------------------------------------------------------------------------
void CXBVirtualKeyboard::Render()
{
	try
	{
    // Clear the viewport, zbuffer, and stencil buffer
  	//if (m_pParentWindow) 
	  //	m_pParentWindow->Render();

    RenderKeyboardLatin();    
    
	}
	catch(...)
	{
	}
}




//-----------------------------------------------------------------------------
// Name: ValidateState()
// Desc: Check object invariants
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::ValidateState() const
{
}




//-----------------------------------------------------------------------------
// Name: InitBoard()
// Desc: Sets up the virtual keyboard for the selected language
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::InitBoard()
{
    // Restore keyboard to default state
    m_iCurrRow = 1;
    m_iCurrKey = 1;
    m_iLastColumn = 1;
    m_iCurrBoard = TYPE_ALPHABET;
    m_bIsCapsLockOn = FALSE;
    m_bIsShiftOn = FALSE;
    m_strData.erase();
    m_iPos = 0;
    m_fKeyHeight    = 42.0f;
    m_dwMaxRows = 5;

    // Destroy old keyboard
    m_KeyboardList.clear();

    
    //-------------------------------------------------------------------------
    // Alpha keyboard
    //-------------------------------------------------------------------------

    Keyboard keyBoard;
    keyBoard.reserve( m_dwMaxRows );
    keyBoard.clear();

    KeyRow keyRow;
    keyRow.reserve( MAX_KEYS_PER_ROW );

    // First row is Done, 1-0
    keyRow.clear();
    keyRow.push_back( Key( XK_OK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_1 ) );
    keyRow.push_back( Key( XK_2 ) );
    keyRow.push_back( Key( XK_3 ) );
    keyRow.push_back( Key( XK_4 ) );
    keyRow.push_back( Key( XK_5 ) );
    keyRow.push_back( Key( XK_6 ) );
    keyRow.push_back( Key( XK_7 ) );
    keyRow.push_back( Key( XK_8 ) );
    keyRow.push_back( Key( XK_9 ) );
    keyRow.push_back( Key( XK_0 ) );
    keyBoard.push_back( keyRow );

    // Second row is Shift, A-J
    keyRow.clear();
    keyRow.push_back( Key( XK_SHIFT, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_A ) );
    keyRow.push_back( Key( XK_B ) );
    keyRow.push_back( Key( XK_C ) );
    keyRow.push_back( Key( XK_D ) );
    keyRow.push_back( Key( XK_E ) );
    keyRow.push_back( Key( XK_F ) );
    keyRow.push_back( Key( XK_G ) );
    keyRow.push_back( Key( XK_H ) );
    keyRow.push_back( Key( XK_I ) );
    keyRow.push_back( Key( XK_J ) );
    keyBoard.push_back( keyRow );

    // Third row is Caps Lock, K-T
    keyRow.clear();
    keyRow.push_back( Key( XK_CAPSLOCK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_K ) );
    keyRow.push_back( Key( XK_L ) );
    keyRow.push_back( Key( XK_M ) );
    keyRow.push_back( Key( XK_N ) );
    keyRow.push_back( Key( XK_O ) );
    keyRow.push_back( Key( XK_P ) );
    keyRow.push_back( Key( XK_Q ) );
    keyRow.push_back( Key( XK_R ) );
    keyRow.push_back( Key( XK_S ) );
    keyRow.push_back( Key( XK_T ) );
    keyBoard.push_back( keyRow );

    // Fourth row is Symbols, U-Z, Backspace
    keyRow.clear();
    keyRow.push_back( Key( XK_SYMBOLS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_U ) );
    keyRow.push_back( Key( XK_V ) );
    keyRow.push_back( Key( XK_W ) );
    keyRow.push_back( Key( XK_X ) );
    keyRow.push_back( Key( XK_Y ) );
    keyRow.push_back( Key( XK_Z ) );
    keyRow.push_back( Key( XK_BACKSPACE, (KEY_WIDTH * 4) + (GAP_WIDTH * 3) ) );
    keyBoard.push_back( keyRow );

    // Fifth row is Accents, Space, Left, Right
    keyRow.clear();
    keyRow.push_back( Key( XK_ACCENTS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_SPACE, (KEY_WIDTH * 6) + (GAP_WIDTH * 5) ) );
    keyRow.push_back( Key( XK_ARROWLEFT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyRow.push_back( Key( XK_ARROWRIGHT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyBoard.push_back( keyRow );

    // Add the alpha keyboard to the list
    m_KeyboardList.push_back( keyBoard );

    //-------------------------------------------------------------------------
    // Symbol keyboard
    //-------------------------------------------------------------------------

    keyBoard.clear();

    // First row
    keyRow.clear();
    keyRow.push_back( Key( XK_OK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LPAREN ) );
    keyRow.push_back( Key( XK_RPAREN ) );
    keyRow.push_back( Key( XK_AMPER ) );
    keyRow.push_back( Key( XK_UNDERS ) );
    keyRow.push_back( Key( XK_CARET ) );
    keyRow.push_back( Key( XK_PERCENT ) );
    keyRow.push_back( Key( XK_BSLASH ) );
    keyRow.push_back( Key( XK_FSLASH ) );
    keyRow.push_back( Key( XK_AT ) );
    keyRow.push_back( Key( XK_NSIGN ) );
    keyBoard.push_back( keyRow );

    // Second row
    keyRow.clear();
    keyRow.push_back( Key( XK_SHIFT, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LBRACK ) );
    keyRow.push_back( Key( XK_RBRACK ) );
    keyRow.push_back( Key( XK_DOLLAR ) );
    keyRow.push_back( Key( XK_POUND_SIGN ) );
    keyRow.push_back( Key( XK_YEN_SIGN ) );
    keyRow.push_back( Key( XK_EURO_SIGN ) );
    keyRow.push_back( Key( XK_SEMI ) );
    keyRow.push_back( Key( XK_COLON ) );
    keyRow.push_back( Key( XK_QUOTE ) );
    keyRow.push_back( Key( XK_DQUOTE ) );
    keyBoard.push_back( keyRow );

    // Third row
    keyRow.clear();
    keyRow.push_back( Key( XK_CAPSLOCK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LT ) );
    keyRow.push_back( Key( XK_GT ) );
    keyRow.push_back( Key( XK_QMARK ) );
    keyRow.push_back( Key( XK_EXCL ) );
    keyRow.push_back( Key( XK_INVERTED_QMARK ) );
    keyRow.push_back( Key( XK_INVERTED_EXCL ) );
    keyRow.push_back( Key( XK_DASH ) );
    keyRow.push_back( Key( XK_STAR ) );
    keyRow.push_back( Key( XK_PLUS ) );
    keyRow.push_back( Key( XK_EQUAL ) );
    keyBoard.push_back( keyRow );

    // Fourth row
    keyRow.clear();
    keyRow.push_back( Key( XK_ALPHABET, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_LBRACE ) );
    keyRow.push_back( Key( XK_RBRACE ) );
    keyRow.push_back( Key( XK_LT_DBL_ANGLE_QUOTE ) );
    keyRow.push_back( Key( XK_RT_DBL_ANGLE_QUOTE ) );
    keyRow.push_back( Key( XK_COMMA ) );
    keyRow.push_back( Key( XK_PERIOD ) );
    keyRow.push_back( Key( XK_BACKSPACE, (KEY_WIDTH * 4) + (GAP_WIDTH * 3) ) );
    keyBoard.push_back( keyRow );

    // Fifth row is Accents, Space, Left, Right
    keyRow.clear();
    keyRow.push_back( Key( XK_ACCENTS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_SPACE, (KEY_WIDTH * 6) + (GAP_WIDTH * 5) ) );
    keyRow.push_back( Key( XK_ARROWLEFT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyRow.push_back( Key( XK_ARROWRIGHT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyBoard.push_back( keyRow );

    // Add the symbol keyboard to the list
    m_KeyboardList.push_back( keyBoard );

    //-------------------------------------------------------------------------
    // Accents keyboard
    //-------------------------------------------------------------------------

    keyBoard.clear();

    // First row
    keyRow.clear();
    keyRow.push_back( Key( XK_OK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_1 ) );
    keyRow.push_back( Key( XK_2 ) );
    keyRow.push_back( Key( XK_3 ) );
    keyRow.push_back( Key( XK_4 ) );
    keyRow.push_back( Key( XK_5 ) );
    keyRow.push_back( Key( XK_6 ) );
    keyRow.push_back( Key( XK_7 ) );
    keyRow.push_back( Key( XK_8 ) );
    keyRow.push_back( Key( XK_9 ) );
    keyRow.push_back( Key( XK_0 ) );
    keyBoard.push_back( keyRow );

    // Second row
    keyRow.clear();
    keyRow.push_back( Key( XK_SHIFT, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_CAP_A_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_A_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_A_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_A_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_C_CEDILLA ) );
    keyRow.push_back( Key( XK_CAP_E_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_E_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_E_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_E_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_I_GRAVE ) );
    keyBoard.push_back( keyRow );

    // Third row
    keyRow.clear();
    keyRow.push_back( Key( XK_CAPSLOCK, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_CAP_I_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_I_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_I_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_N_TILDE ) );
    keyRow.push_back( Key( XK_CAP_O_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_O_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_O_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_O_TILDE ) );
    keyRow.push_back( Key( XK_CAP_O_DIAERESIS ) );
    keyRow.push_back( Key( XK_SM_SHARP_S ) );
    keyBoard.push_back( keyRow );

    // Fourth row
    keyRow.clear();
    keyRow.push_back( Key( XK_ALPHABET, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_CAP_U_GRAVE ) );
    keyRow.push_back( Key( XK_CAP_U_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_U_CIRCUMFLEX ) );
    keyRow.push_back( Key( XK_CAP_U_DIAERESIS ) );
    keyRow.push_back( Key( XK_CAP_Y_ACUTE ) );
    keyRow.push_back( Key( XK_CAP_Y_DIAERESIS ) );
    keyRow.push_back( Key( XK_BACKSPACE, (KEY_WIDTH * 4) + (GAP_WIDTH * 3) ) );
    keyBoard.push_back( keyRow );

    // Fifth row
    keyRow.clear();
    keyRow.push_back( Key( XK_ACCENTS, MODEKEY_WIDTH ) );
    keyRow.push_back( Key( XK_SPACE, (KEY_WIDTH * 6) + (GAP_WIDTH * 5) ) );
    keyRow.push_back( Key( XK_ARROWLEFT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyRow.push_back( Key( XK_ARROWRIGHT, (KEY_WIDTH * 2) + (GAP_WIDTH * 1) ) );
    keyBoard.push_back( keyRow );

    // Add the accents keyboard to the list
    m_KeyboardList.push_back( keyBoard );

}




//-----------------------------------------------------------------------------
// Name: UpdateState()
// Desc: State machine updates the current context based on the incoming event
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::UpdateState( Event ev )
{
    switch( m_State )
    {
        case STATE_KEYBOARD:
            switch( ev )
            {
                case EV_A_BUTTON:           // Select current key
                case EV_START_BUTTON:
                    PressCurrent();
                    break;

                case EV_B_BUTTON:           // Shift mode
                case EV_BACK_BUTTON:        // Back
                    m_State = STATE_BACK;
					//Close();	//Added by JM to close automatically
                    break;
                
                case EV_X_BUTTON:           // Toggle keyboard
                    if( m_iKeyboard == KEYBOARD_ENGLISH )
                    {
                        Press( m_iCurrBoard == TYPE_SYMBOLS ?
                               XK_ALPHABET : XK_SYMBOLS );
                    }
                    else
                    {
                        switch( m_iCurrBoard )
                        {
                            case TYPE_ALPHABET: Press( XK_SYMBOLS  ); break;
                            case TYPE_SYMBOLS:  Press( XK_ACCENTS  ); break;
                            case TYPE_ACCENTS:  Press( XK_ALPHABET ); break;
                        }
                    }
                    break;
                case EV_WHITE_BUTTON:       // Backspace
                    Press( XK_BACKSPACE );
                    break;
                case EV_BLACK_BUTTON:       // Space
                    Press( XK_SPACE );
                    break;
                case EV_LEFT_BUTTON:        // Left
                    Press( XK_ARROWLEFT );
                    break;
                case EV_RIGHT_BUTTON:       // Right
                    Press( XK_ARROWRIGHT );
                    break;

                // Navigation
                case EV_UP:     MoveUp();    break;
                case EV_DOWN:   MoveDown();  break;
                case EV_LEFT:   MoveLeft();  break;
                case EV_RIGHT:  MoveRight(); break;
            }
            break;
        default:
//          Close();
            break;
    }
}


//-----------------------------------------------------------------------------
// Name: PressCurrent()
// Desc: Press the current key on the keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::PressCurrent()
{
    // Determine the current key
    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];

    // Press it
    Press( key.xKey );
}




//-----------------------------------------------------------------------------
// Name: Press()
// Desc: Press the given key on the keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::Press( Xkey xk )
{
    if ( xk == XK_NULL ) // happens in Japanese keyboard (keyboard type)
        xk = XK_SPACE;

    // If the key represents a character, add it to the word
    if( xk < 0x10000 && xk != XK_ARROWLEFT && xk != XK_ARROWRIGHT )
    {
        // Don't add more than the maximum characters, and don't allow 
        // text to exceed the width of the text entry field
        if( m_strData.length() < MAX_CHARS )
        {
            FLOAT fWidth, fHeight;
            m_Font18.GetTextExtent( m_strData.c_str(), &fWidth, &fHeight );

            if( fWidth < fTEXTBOX_WIDTH )
            {
                m_strData.insert( m_iPos, 1, GetChar( xk ) );
                ++m_iPos; // move the caret
            }
        }

        // Unstick the shift key
        m_bIsShiftOn = FALSE;
    }

    // Special cases
    else switch( xk )
    {
        case XK_BACKSPACE:
            if( m_iPos > 0 )
            {
                --m_iPos; // move the caret
                m_strData.erase( m_iPos, 1 );
            }
            break;
        case XK_DELETE: // Used for Japanese only
            if( m_strData.length() > 0 )
                m_strData.erase( m_iPos, 1 );
            break;
        case XK_SHIFT:
            m_bIsShiftOn = !m_bIsShiftOn;
            break;
        case XK_CAPSLOCK:
            m_bIsCapsLockOn = !m_bIsCapsLockOn;
            break;
        case XK_ALPHABET:
            m_iCurrBoard = TYPE_ALPHABET;

            // Adjust mode keys
            m_KeyboardList[m_iCurrBoard][3][0] = Key( XK_SYMBOLS, MODEKEY_WIDTH );
            m_KeyboardList[m_iCurrBoard][4][0] = Key( XK_ACCENTS, MODEKEY_WIDTH );

            break;
        case XK_SYMBOLS:
            m_iCurrBoard = TYPE_SYMBOLS;

            // Adjust mode keys
            m_KeyboardList[m_iCurrBoard][3][0] = Key( XK_ALPHABET, MODEKEY_WIDTH );
            m_KeyboardList[m_iCurrBoard][4][0] = Key( XK_ACCENTS, MODEKEY_WIDTH );

            break;
        case XK_ACCENTS:
            m_iCurrBoard = TYPE_ACCENTS;

            // Adjust mode keys
            m_KeyboardList[m_iCurrBoard][3][0] = Key( XK_ALPHABET, MODEKEY_WIDTH );
            m_KeyboardList[m_iCurrBoard][4][0] = Key( XK_SYMBOLS, MODEKEY_WIDTH );

            break;
        case XK_ARROWLEFT:
            if( m_iPos > 0 )
                --m_iPos;
            break;
        case XK_ARROWRIGHT:
            if( m_iPos < m_strData.length() )
                ++m_iPos;
            break;
        case XK_OK:
//            Close();
            m_bConfirmed=true;
            break;
    }
}




//-----------------------------------------------------------------------------
// Name: MoveUp()
// Desc: Move the cursor up
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveUp()
{

    do
    {
        // Update key index for special cases
        switch( m_iCurrRow )
        {
            case 0:
                if( 1 < m_iCurrKey && m_iCurrKey < 7 )      // 2 - 6
                {
                    m_iLastColumn = m_iCurrKey;             // remember column
                    m_iCurrKey = 1;                         // move to spacebar
                }
                else if( 6 < m_iCurrKey && m_iCurrKey < 9 ) // 7 - 8
                {
                    m_iLastColumn = m_iCurrKey;             // remember column
                    m_iCurrKey = 2;                         // move to left arrow
                }
                else if( m_iCurrKey > 8 )                   // 9 - 0
                {
                    m_iLastColumn = m_iCurrKey;             // remember column
                    m_iCurrKey = 3;                         // move to right arrow
                }
                break;
            case 3:
                if( m_iCurrKey == 7 )                       // backspace
                    m_iCurrKey = max( 7, m_iLastColumn );   // restore column
                break;
            case 4:
                if( m_iCurrKey == 1 )                       // spacebar
                    m_iCurrKey = min( 6, m_iLastColumn );   // restore column
                else if( m_iCurrKey > 1 )                   // left and right
                    m_iCurrKey = 7;                         // backspace
                break;
        }

        // Update row
        m_iCurrRow = ( m_iCurrRow == 0 ) ? m_dwMaxRows - 1 : m_iCurrRow - 1;

    } while( IsKeyDisabled() );
}




//-----------------------------------------------------------------------------
// Name: MoveDown()
// Desc: Move the cursor down
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveDown()
{

    do
    {
        // Update key index for special cases
        switch( m_iCurrRow )
        {
            case 2:
                if( m_iCurrKey > 7 )                    // q - t
                {
                    m_iLastColumn = m_iCurrKey;         // remember column
                    m_iCurrKey = 7;                     // move to backspace
                }
                break;
            case 3:
                if( 0 < m_iCurrKey && m_iCurrKey < 7 )  // u - z
                {
                    m_iLastColumn = m_iCurrKey;         // remember column
                    m_iCurrKey = 1;                     // move to spacebar
                }
                else if( m_iCurrKey > 6 )               // backspace
                {
                    if( m_iLastColumn > 8 )
                        m_iCurrKey = 3;                 // move to right arrow
                    else
                        m_iCurrKey = 2;                 // move to left arrow
                }
                break;
            case 4:
                switch( m_iCurrKey )
                {
                    case 1:                             // spacebar
                        m_iCurrKey = min( 6, m_iLastColumn );
                        break;
                    case 2:                             // left arrow
                        m_iCurrKey = max( min( 8, m_iLastColumn ), 7 );
                        break;
                    case 3:                             // right arrow
                        m_iCurrKey = max( 9, m_iLastColumn );
                        break;
                }
                break;
        }

        // Update row
        m_iCurrRow = ( m_iCurrRow == m_dwMaxRows - 1 ) ? 0 : m_iCurrRow + 1;

    } while( IsKeyDisabled() );
}




//-----------------------------------------------------------------------------
// Name: MoveLeft()
// Desc: Move the cursor left
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveLeft()
{
    do
    {
        if( m_iCurrKey == 0 )
            m_iCurrKey = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ].size() - 1;
        else
            --m_iCurrKey;

    } while( IsKeyDisabled() );

    SetLastColumn();
}




//-----------------------------------------------------------------------------
// Name: MoveRight()
// Desc: Move the cursor right
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::MoveRight()
{
    do
    {
        if( m_iCurrKey == m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ].size() - 1 )
            m_iCurrKey = 0;
        else
            ++m_iCurrKey;

    } while( IsKeyDisabled() );

    SetLastColumn();
}




//-----------------------------------------------------------------------------
// Name: SetLastColumn()
// Desc: Remember the column position if we're on a single letter character
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::SetLastColumn()
{
    // If the new key is a single character, remember it for later
    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];
    if( key.strName.empty() )
    {
        switch( key.xKey )
        {
            // Adjust the last column for the arrow keys to confine it
            // within the range of the key width
            case XK_ARROWLEFT:
                m_iLastColumn = ( m_iLastColumn <= 7 ) ? 7 : 8; break;
            case XK_ARROWRIGHT:
                m_iLastColumn = ( m_iLastColumn <= 9 ) ? 9 : 10; break;

            // Single char, non-arrow
            default:
                m_iLastColumn = m_iCurrKey; break;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: RenderKey()
// Desc: Render the key at the given position
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderKey( FLOAT fX, FLOAT fY, const Key& key, 
                                    D3DCOLOR KeyColor, 
                                    D3DCOLOR TextColor ) const
{
    if( KeyColor == COLOR_INVISIBLE || key.xKey == XK_NULL )
        return;

    struct KEYVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };

    WCHAR strKey[2] = { GetChar( key.xKey ), 0 };
    const WCHAR* strName = key.strName.empty() ? strKey : key.strName.c_str();

    FLOAT x = fX + KEY_INSET;
    FLOAT y = fY + KEY_INSET;
    FLOAT z = fX + key.dwWidth - KEY_INSET + 2;
    FLOAT w = fY + m_fKeyHeight - KEY_INSET + 2;

    KEYVERTEX pVertices[16];
    pVertices[0].p  = D3DXVECTOR4( x-0.5f, y-0.5f, 1.0f, 1.0f );     pVertices[0].t  = D3DXVECTOR2( 0.0f, 0.0f );
    pVertices[1].p  = D3DXVECTOR4( x+17-0.5f, y-0.5f, 1.0f, 1.0f );  pVertices[1].t  = D3DXVECTOR2( 0.5f, 0.0f );
    pVertices[2].p  = D3DXVECTOR4( x+17-0.5f, w-0.5f, 1.0f, 1.0f );  pVertices[2].t  = D3DXVECTOR2( 0.5f, 1.0f );
    pVertices[3].p  = D3DXVECTOR4( x-0.5f, w-0.5f, 1.0f, 1.0f );     pVertices[3].t  = D3DXVECTOR2( 0.0f, 1.0f );

    pVertices[4].p  = D3DXVECTOR4( x+17-0.5f, y-0.5f, 1.0f, 1.0f );  pVertices[4].t  = D3DXVECTOR2( 0.5f, 0.0f );
    pVertices[5].p  = D3DXVECTOR4( z-17-0.5f, y-0.5f, 1.0f, 1.0f );  pVertices[5].t  = D3DXVECTOR2( 0.5f, 0.0f );
    pVertices[6].p  = D3DXVECTOR4( z-17-0.5f, w-0.5f, 1.0f, 1.0f );  pVertices[6].t  = D3DXVECTOR2( 0.5f, 1.0f );
    pVertices[7].p  = D3DXVECTOR4( x+17-0.5f, w-0.5f, 1.0f, 1.0f );  pVertices[7].t  = D3DXVECTOR2( 0.5f, 1.0f );

    pVertices[8].p  = D3DXVECTOR4( z-17-0.5f, y-0.5f, 1.0f, 1.0f );  pVertices[8].t  = D3DXVECTOR2( 0.5f, 0.0f );
    pVertices[9].p  = D3DXVECTOR4( z-0.5f, y-0.5f, 1.0f, 1.0f );     pVertices[9].t  = D3DXVECTOR2( 1.0f, 0.0f );
    pVertices[10].p = D3DXVECTOR4( z-0.5f, w-0.5f, 1.0f, 1.0f );     pVertices[10].t = D3DXVECTOR2( 1.0f, 1.0f );
    pVertices[11].p = D3DXVECTOR4( z-17-0.5f, w-0.5f, 1.0f, 1.0f );  pVertices[11].t = D3DXVECTOR2( 0.5f, 1.0f );

    pVertices[12].p  = D3DXVECTOR4( x-0.5f, y-0.5f, 1.0f, 1.0f );
    pVertices[13].p  = D3DXVECTOR4( z-0.5f, y-0.5f, 1.0f, 1.0f );
    pVertices[14].p  = D3DXVECTOR4( z-0.5f, w-0.5f, 1.0f, 1.0f );
    pVertices[15].p  = D3DXVECTOR4( x-0.5f, w-0.5f, 1.0f, 1.0f );


    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );

    // Draw the key background
    g_pd3dDevice->SetTexture( 0, m_pKeyTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    
    if( KeyColor )
        g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, KeyColor );
    else
        g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffffff );

    g_pd3dDevice->DrawVerticesUP( D3DPT_QUADLIST, 12, pVertices, sizeof(KEYVERTEX) );

    // Draw the key text. If key name is, use a slightly smaller font.
    if( key.strName.length() > 1 && iswupper( key.strName[1] ) )
    {
        m_Font12.DrawText( ( x + z ) / 2.0f, ( y + w ) / 2.0f , TextColor, strName, 
                           XBFONT_CENTER_X | XBFONT_CENTER_Y );
    }
    else
    {
        m_Font18.DrawText( ( x + z ) / 2.0f, ( y + w ) / 2.0f , TextColor, strName, 
                           XBFONT_CENTER_X | XBFONT_CENTER_Y );
    }

}




//-----------------------------------------------------------------------------
// Name: DrawTextBox()
// Desc: Display box containing text input
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawTextBox() const
{
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );

    D3DXVECTOR4 avRect[4];
    avRect[0] = D3DXVECTOR4(  64 - 0.5f,208 - 0.5f, 1.0f, 1.0f );
    avRect[1] = D3DXVECTOR4( 576 - 0.5f,208 - 0.5f, 1.0f, 1.0f );
    avRect[2] = D3DXVECTOR4( 576 - 0.5f,248 - 0.5f, 1.0f, 1.0f );
    avRect[3] = D3DXVECTOR4(  64 - 0.5f,248 - 0.5f, 1.0f, 1.0f );

    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffe0e0e0 );
    g_pd3dDevice->DrawVerticesUP( D3DPT_QUADLIST, 4, avRect, sizeof(D3DXVECTOR4) );
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff101010 );
    g_pd3dDevice->DrawVerticesUP( D3DPT_LINELOOP, 4, avRect, sizeof(D3DXVECTOR4) );
}




//-----------------------------------------------------------------------------
// Name: RenderKeyboardLatin()
// Desc: Display current latin keyboard
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::RenderKeyboardLatin() const
{
    // Show text and caret
    DrawTextBox();
    DrawText( 68.0f, 212.0f );

    // Draw each row
    FLOAT fY = 250.0f;
    const Keyboard& keyBoard = m_KeyboardList[ m_iCurrBoard ];
    for( DWORD row = 0; row < m_dwMaxRows; ++row, fY += m_fKeyHeight )
    {
        FLOAT fX = 64.0f;
        FLOAT fWidthSum = 0.0f;
        const KeyRow& keyRow = keyBoard[ row ];
        DWORD dwIndex = 0;
        for( KeyRow::const_iterator i = keyRow.begin(); i != keyRow.end(); ++i, ++dwIndex )
        {
            // Determine key name
            const Key& key = *i;
            D3DCOLOR selKeyColor = 0x00000000;
            D3DCOLOR selTextColor = COLOR_NORMAL;

            // Handle special key coloring
            switch( key.xKey )
            {
                case XK_SHIFT:
                    switch( m_iCurrBoard )
                    {
                        case TYPE_ALPHABET:
                        case TYPE_ACCENTS:
                            if( m_bIsShiftOn )
                                selKeyColor = COLOR_PRESSED;
                            break;
                        case TYPE_SYMBOLS:
                            selKeyColor = COLOR_DISABLED;
                            selTextColor = COLOR_FONT_DISABLED;
                            break;
                    }
                    break;
                case XK_CAPSLOCK:
                    switch( m_iCurrBoard )
                    {
                        case TYPE_ALPHABET:
                        case TYPE_ACCENTS:
                            if( m_bIsCapsLockOn )
                                selKeyColor = COLOR_PRESSED;
                            break;
                        case TYPE_SYMBOLS:
                            selKeyColor = COLOR_DISABLED;
                            selTextColor = COLOR_FONT_DISABLED;
                            break;
                    }
                    break;
                case XK_ACCENTS:
                    if( m_iKeyboard == KEYBOARD_ENGLISH )
                    {
                        selKeyColor = COLOR_INVISIBLE;
                        selTextColor = COLOR_INVISIBLE;
                    }
                    break;
            }

            // Highlight the current key
            if( row == m_iCurrRow && dwIndex == m_iCurrKey )
                selKeyColor |= COLOR_HIGHLIGHT;

            RenderKey( fX + fWidthSum, fY, key, selKeyColor, selTextColor );

            fWidthSum += key.dwWidth;

            // There's a slightly larger gap between the leftmost keys (mode
            // keys) and the main keyboard
            if( dwIndex == 0 )
                fWidthSum += GAP2_WIDTH;
            else
                fWidthSum += GAP_WIDTH;
        }
    }

}


//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draw text in the text input area, accounting for special characters
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawText( FLOAT x, FLOAT y ) const
{
    m_Font18.DrawText( x, y, 0xff10e010, m_strData.c_str() );

    // Draw blinking caret using line primitives.
    if( fmod( m_CaretTimer.GetElapsedSeconds(), fCARET_BLINK_RATE ) < fCARET_ON_RATIO )
    {
        if ( m_iPos )
        {
            String string;
            string.assign( m_strData.c_str(), m_iPos );

            FLOAT fCaretWidth = 0.0f;
            FLOAT fCaretHeight;
            m_Font18.GetTextExtent( string.c_str(), &fCaretWidth, &fCaretHeight );
            x += fCaretWidth;
        }

        g_pd3dDevice->SetTexture( 0, NULL );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
        g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff101010 );

        g_pd3dDevice->Begin( D3DPT_LINELIST );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, x-2, y+2,  1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, x+2, y+2,  1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, x-1, y+2,  1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, x-1, y+25, 1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX,  x,  y+2,  1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX,  x,  y+25, 1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, x-2, y+25, 1.0f, 1.0f );
        g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, x+2, y+25, 1.0f, 1.0f );
        g_pd3dDevice->End();
    }
}




//-----------------------------------------------------------------------------
// Name: DrawRedUnderlinedText()
// Desc: Draw text with a red drop shadow
//-----------------------------------------------------------------------------
VOID CXBVirtualKeyboard::DrawRedUnderlinedText( FLOAT x, FLOAT y, DWORD dwColor, 
                                                const WCHAR* str, DWORD dwFlags ) const
{
    m_Font18.DrawText( x+2, y+2, COLOR_RED, str, dwFlags );
    m_Font18.DrawText( x+0, y+0,   dwColor, str, dwFlags );
}






//-----------------------------------------------------------------------------
// Name: IsDisabled()
// Desc: TRUE if the current key (m_iCurrBoard, m_iCurrRow, m_iCurrKey) is
//       disabled.
//-----------------------------------------------------------------------------
BOOL CXBVirtualKeyboard::IsKeyDisabled() const
{
    Key key = m_KeyboardList[ m_iCurrBoard ][ m_iCurrRow ][ m_iCurrKey ];
    // On the symbols keyboard, Shift and Caps Lock are disabled
    if( m_iCurrBoard == TYPE_SYMBOLS )
    {
        if( key.xKey == XK_SHIFT || key.xKey == XK_CAPSLOCK )
            return TRUE;
    }

    // On the English keyboard, the Accents key is disabled
    if( m_iKeyboard == KEYBOARD_ENGLISH )
    {
        if( key.xKey == XK_ACCENTS )
            return TRUE;
    }

    return FALSE;
}







//-----------------------------------------------------------------------------
// Name: GetChar()
// Desc: Convert Xkey value to WCHAR given current capitalization settings
//-----------------------------------------------------------------------------
WCHAR CXBVirtualKeyboard::GetChar( Xkey xk ) const
{
    // Handle case conversion
    WCHAR wc = WCHAR( xk );

    if( ( m_bIsCapsLockOn && !m_bIsShiftOn ) || ( !m_bIsCapsLockOn && m_bIsShiftOn ) )
        wc = ToUpper( wc );
    else
        wc = ToLower( wc );

    return wc;
}




//-----------------------------------------------------------------------------
// Name: ToUpper()
// Desc: Convert WCHAR to upper case. Handles accented characters properly.
//-----------------------------------------------------------------------------
WCHAR CXBVirtualKeyboard::ToUpper( WCHAR c ) // static
{
#ifdef USE_CONVERSION_TABLE

    // The table-based solution is faster, but requires 512 bytes of space
    static const WCHAR arrToUpper[] =
    {
        0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96

        // alpha mapping here
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        
        123, 124, 125, 126, 127,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
        0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,

        // accented character mapping here
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
        0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xFE, 0x0178,
    };

    if( c > 0xFF )
        return c;
    return arrToUpper[ c ];

#else

    // The code solution is slower but smaller
    if( c >= 'a' && c <= 'z' )
        return c - ('a' - 'A');
    if( c >= 0xE0 && c <= 0xFD )
        return c - (0xE0 - 0xC0);
    if( c == XK_SM_Y_DIAERESIS )    // 0x00FF
        return XK_CAP_Y_DIAERESIS;  // 0x0178
    return c;

#endif
}




//-----------------------------------------------------------------------------
// Name: ToLower()
// Desc: Convert WCHAR to lower case. Handles accented characters properly.
//-----------------------------------------------------------------------------
WCHAR CXBVirtualKeyboard::ToLower( WCHAR c ) // static
{
#ifdef USE_CONVERSION_TABLE
   
    // The table-based solution is faster, but requires 512 bytes of space
    static const WCHAR arrToLower[] =
    {
        0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64

        // alpha mapping here
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
        105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
        118, 119, 120, 121, 122, 123, 124, 125, 126, 127,

        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,

        // accented character mapping here
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
        
        0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    };

    if( c == XK_CAP_Y_DIAERESIS ) // 0x0178
        return XK_SM_Y_DIAERESIS;
    if( c > 0xFF )
        return c;
    return arrToLower[ c ];

#else
    
    // The code solution is slower but smaller
    if( c >= 'A' && c <= 'Z' )
        return c + ( 'a' - 'A' );
    if( c >= 0xC0 && c <= 0xDD )
        return c + ( 0xE0 - 0xC0 );
    if( c == XK_CAP_Y_DIAERESIS ) // 0x0178
        return XK_SM_Y_DIAERESIS;
    return c;

#endif
}




//-----------------------------------------------------------------------------
// Name: SelectKeyboard()
// Desc: Select Keyboard and update related information
//-----------------------------------------------------------------------------
void CXBVirtualKeyboard::SelectKeyboard( UINT iKeyboard )
{
    if( iKeyboard > KEYBOARD_MAX )
        return;
    m_iKeyboard     = iKeyboard;
    m_pCurrKeyboard = g_aKeyboardInfo + m_iKeyboard;
    m_iLanguage     = m_pCurrKeyboard->GetLanguage();
    g_pStringTable  = m_pCurrKeyboard->GetStringTable();
}




//-----------------------------------------------------------------------------
// Name: SelectKeyboard()
// Desc: Select Keyboard and update related information
//-----------------------------------------------------------------------------
const WCHAR* GetString( const WCHAR* strTable[], UINT dwStringID )
{
    if( strTable && dwStringID < STR_MAX )
        return strTable[ dwStringID ];
    
    return L"";
};




//-----------------------------------------------------------------------------
// Name: SelectKeyboard()
// Desc: Select Keyboard and update related information
//-----------------------------------------------------------------------------
const WCHAR* GetString( UINT dwStringID )
{
    return GetString( g_pStringTable, dwStringID );
};

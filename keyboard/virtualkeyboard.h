//-----------------------------------------------------------------------------
// File: VirtualKeyboard.h
//
// Desc: Virtual keyboard reference UI
//
// Hist: 02.13.01 - New for March XDK release 
//       03.07.01 - Localized for April XDK release
//       04.10.01 - Updated for May XDK with full translations
//       06.06.01 - Japanese keyboard added
//       07.22.02 - Japanese keyboard (keyboard) added
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBVIRTUAL_KEYBOARD_H
#define XBVIRTUAL_KEYBOARD_H

#pragma warning( disable: 4710 4786 )
#include <xtl.h>
//#include "../xbox/XBApplicationEx.h"
#include <xbfontEx.h>
#include <XBStopWatch.h>
#include <XBResource.h>
#include "../dvd2xbox/d2xmedialib.h"
//#include "GUIDialog.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#include <string>
#include <vector>
#pragma warning( pop )

#define ACTION_MOVE_LEFT							 1	
#define ACTION_MOVE_RIGHT							 2
#define ACTION_MOVE_UP								 3
#define ACTION_MOVE_DOWN							 4
#define ACTION_SELECT_ITEM							 7
#define ACTION_PARENT_MENU							10
#define ACTION_MOVE_LEFT2							12
#define ACTION_MOVE_RIGHT2							14
#define ACTION_PRESS_B								16
#define ACTION_PRESS_Y								18
#define ACTION_PRESS_X								20


//-----------------------------------------------------------------------------
// Name: class CXBVirtualKeyboard
// Desc: Application object for load and save game reference UI
//-----------------------------------------------------------------------------
class CXBVirtualKeyboard //: public CGUIDialog
{
    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------

    static const DWORD KEY_WIDTH = 34;   // width of std key in pixels

	// skine engine
	FLOAT POSY_TEXTBOX;
	FLOAT POSY_KEYBOARD;

    //-------------------------------------------------------------------------
    // Types
    //-------------------------------------------------------------------------

    typedef std::basic_string< WCHAR > String;
    typedef String::size_type          StringIndex;

    //-------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------

    enum KeyboardTypes
    {
        TYPE_ALPHABET,
        TYPE_SYMBOLS,
        TYPE_ACCENTS,

        TYPE_HIRAGANA,
        TYPE_KATAKANA,
        TYPE_ANS,

        TYPE_MAX
    };

    enum State
    {
        STATE_BACK,         // Main menu
        STATE_KEYBOARD,     // Keyboard display
        STATE_MAX
    };

    enum ControllerSState
    {
        XKJ_START   = 1 << 0,
        XKJ_BACK    = 1 << 1,
        XKJ_A       = 1 << 2,
        XKJ_B       = 1 << 3,
        XKJ_X       = 1 << 4,
        XKJ_Y       = 1 << 5,
        XKJ_BLACK   = 1 << 6,
        XKJ_WHITE   = 1 << 7,
        XKJ_LEFTTR  = 1 << 8,
        XKJ_RIGHTTR = 1 << 9,

        XKJ_DUP     = 1 << 12,
        XKJ_DDOWN   = 1 << 13,
        XKJ_DLEFT   = 1 << 14,
        XKJ_DRIGHT  = 1 << 15,
        XKJ_UP      = 1 << 16,
        XKJ_DOWN    = 1 << 17,
        XKJ_LEFT    = 1 << 18,
        XKJ_RIGHT   = 1 << 19
    };

    enum Event
    {
        EV_NULL,            // No events
        EV_A_BUTTON,        // A button
        EV_START_BUTTON,    // Start button
        EV_B_BUTTON,        // B button
        EV_BACK_BUTTON,     // Back button
        EV_X_BUTTON,        // X button
        EV_Y_BUTTON,        // Y button
        EV_WHITE_BUTTON,    // White button
        EV_BLACK_BUTTON,    // Black button
        EV_LEFT_BUTTON,     // Left trigger
        EV_RIGHT_BUTTON,    // Right trigger
        EV_UP,              // Up Dpad or left joy
        EV_DOWN,            // Down Dpad or left joy
        EV_LEFT,            // Left Dpad or left joy
        EV_RIGHT,           // Right Dpad or left joy

        EVENT_MAX
    };

    enum Xkey
    {
        XK_NULL         = 0,

        XK_SPACE        = ' ',
        XK_LBRACK       = '[',
        XK_RBRACK       = ']',
        XK_LBRACE       = '{',
        XK_RBRACE       = '}',
        XK_LPAREN       = '(',
        XK_RPAREN       = ')',
        XK_FSLASH       = '/',
        XK_BSLASH       = '\\',
        XK_LT           = '<',
        XK_GT           = '>',
        XK_AT           = '@',
        XK_SEMI         = ';',
        XK_COLON        = ':',
        XK_QUOTE        = '\'',
        XK_DQUOTE       = '\"',
        XK_AMPER        = '&',
        XK_STAR         = '*',
        XK_QMARK        = '?',
        XK_COMMA        = ',',
        XK_PERIOD       = '.',
        XK_DASH         = '-',
        XK_UNDERS       = '_',
        XK_PLUS         = '+',
        XK_EQUAL        = '=',
        XK_DOLLAR       = '$',
        XK_PERCENT      = '%',
        XK_CARET        = '^',
        XK_TILDE        = '~',
        XK_APOS         = '`',
        XK_EXCL         = '!',
        XK_VERT         = '|',
        XK_NSIGN        = '#',

        // Numbers
        XK_0            = '0',
        XK_1,
        XK_2,
        XK_3,
        XK_4,
        XK_5,
        XK_6,
        XK_7,
        XK_8,
        XK_9,
        
        // Letters
        XK_A            = 'A',
        XK_B,
        XK_C,
        XK_D,
        XK_E,
        XK_F,
        XK_G,
        XK_H,
        XK_I,
        XK_J,
        XK_K,
        XK_L,
        XK_M,
        XK_N,
        XK_O,
        XK_P,
        XK_Q,
        XK_R,
        XK_S,
        XK_T,
        XK_U,
        XK_V,
        XK_W,
        XK_X,
        XK_Y,
        XK_Z,

        // Accented characters and other special characters

        XK_INVERTED_EXCL        = 0xA1, // ¡
        XK_CENT_SIGN            = 0xA2, // ¢
        XK_POUND_SIGN           = 0xA3, // £
        XK_YEN_SIGN             = 0xA5, // ¥
        XK_COPYRIGHT_SIGN       = 0xA9, // ©
        XK_LT_DBL_ANGLE_QUOTE   = 0xAB, // <<
        XK_REGISTERED_SIGN      = 0xAE, // ®
        XK_SUPERSCRIPT_TWO      = 0xB2, // ²
        XK_SUPERSCRIPT_THREE    = 0xB3, // ³
        XK_ACUTE_ACCENT         = 0xB4, // ´
        XK_MICRO_SIGN           = 0xB5, // µ
        XK_SUPERSCRIPT_ONE      = 0xB9, // ¹
        XK_RT_DBL_ANGLE_QUOTE   = 0xBB, // >>
        XK_INVERTED_QMARK       = 0xBF, // ¿
        XK_CAP_A_GRAVE          = 0xC0, // À
        XK_CAP_A_ACUTE          = 0xC1, // Á
        XK_CAP_A_CIRCUMFLEX     = 0xC2, // Â
        XK_CAP_A_TILDE          = 0xC3, // Ã
        XK_CAP_A_DIAERESIS      = 0xC4, // Ä
        XK_CAP_A_RING           = 0xC5, // Å
        XK_CAP_AE               = 0xC6, // Æ
        XK_CAP_C_CEDILLA        = 0xC7, // Ç
        XK_CAP_E_GRAVE          = 0xC8, // È
        XK_CAP_E_ACUTE          = 0xC9, // É
        XK_CAP_E_CIRCUMFLEX     = 0xCA, // Ê
        XK_CAP_E_DIAERESIS      = 0xCB, // Ë
        XK_CAP_I_GRAVE          = 0xCC, // Ì
        XK_CAP_I_ACUTE          = 0xCD, // Í
        XK_CAP_I_CIRCUMFLEX     = 0xCE, // Î
        XK_CAP_I_DIAERESIS      = 0xCF, // Ï
        XK_CAP_N_TILDE          = 0xD1, // Ñ
        XK_CAP_O_GRAVE          = 0xD2, // Ò
        XK_CAP_O_ACUTE          = 0xD3, // Ó
        XK_CAP_O_CIRCUMFLEX     = 0xD4, // Ô
        XK_CAP_O_TILDE          = 0xD5, // Õ
        XK_CAP_O_DIAERESIS      = 0xD6, // Ö
        XK_CAP_O_STROKE         = 0xD8, // Ø
        XK_CAP_U_GRAVE          = 0xD9, // Ù
        XK_CAP_U_ACUTE          = 0xDA, // Ú
        XK_CAP_U_CIRCUMFLEX     = 0xDB, // Û
        XK_CAP_U_DIAERESIS      = 0xDC, // Ü
        XK_CAP_Y_ACUTE          = 0xDD, // Ý
        XK_SM_SHARP_S           = 0xDF, // ß
        XK_SM_A_GRAVE           = 0xE0, // à
        XK_SM_A_ACUTE           = 0xE1, // á
        XK_SM_A_CIRCUMFLEX      = 0xE2, // â
        XK_SM_A_TILDE           = 0xE3, // ã
        XK_SM_A_DIAERESIS       = 0xE4, // ä
        XK_SM_A_RING            = 0xE5, // å
        XK_SM_AE                = 0xE6, // æ
        XK_SM_C_CEDILLA         = 0xE7, // ç
        XK_SM_E_GRAVE           = 0xE8, // è
        XK_SM_E_ACUTE           = 0xE9, // é
        XK_SM_E_CIRCUMFLEX      = 0xEA, // ê
        XK_SM_E_DIAERESIS       = 0xEB, // ë
        XK_SM_I_GRAVE           = 0xEC, // ì
        XK_SM_I_ACUTE           = 0xED, // í
        XK_SM_I_CIRCUMFLEX      = 0xEE, // î
        XK_SM_I_DIAERESIS       = 0xEF, // ï
        XK_SM_N_TILDE           = 0xF1, // ñ
        XK_SM_O_GRAVE           = 0xF2, // ò
        XK_SM_O_ACUTE           = 0xF3, // ó
        XK_SM_O_CIRCUMFLEX      = 0xF4, // ô
        XK_SM_O_TILDE           = 0xF5, // õ
        XK_SM_O_DIAERESIS       = 0xF6, // ö
        XK_SM_O_STROKE          = 0xF8, // ø
        XK_SM_U_GRAVE           = 0xF9, // ù
        XK_SM_U_ACUTE           = 0xFA, // ú
        XK_SM_U_CIRCUMFLEX      = 0xFB, // û
        XK_SM_U_DIAERESIS       = 0xFC, // ü
        XK_SM_Y_ACUTE           = 0xFD, // ý
        XK_SM_Y_DIAERESIS       = 0xFF, // ÿ

        // Unicode
        XK_CAP_Y_DIAERESIS = 0x0178, // Y umlaut
        XK_EURO_SIGN       = 0x20AC, // Euro symbol
        XK_ARROWLEFT       = 0x22B2, // left arrow
        XK_ARROWRIGHT      = 0x22B3, // right arrow

        // Special
        XK_BACKSPACE = 0x10000, // backspace
        XK_DELETE,              // delete           // !!!
        XK_SHIFT,               // shift
        XK_CAPSLOCK,            // caps lock
        XK_ALPHABET,            // alphabet
        XK_SYMBOLS,             // symbols
        XK_ACCENTS,             // accents
        XK_OK,                  // "done"
        XK_HIRAGANA,            // Hiragana
        XK_KATAKANA,            // Katakana
        XK_ANS,                 // Alphabet/numeral/symbol
    };

    enum KeyboardLanguageType
    {
        KEYBOARD_ENGLISH,
    };

    //-------------------------------------------------------------------------
    // Classes
    //-------------------------------------------------------------------------

    // Keyboard key information
    struct Key
    {
        Xkey    xKey;       // virtual key code
        DWORD   dwWidth;    // width of the key
        String  strName;    // name of key when vKey >= 0x10000

        explicit Key( Xkey, DWORD = KEY_WIDTH );
    };
public:
    // Keyboard information
    class KeyboardInfo
    {
    public:
        KeyboardInfo( UINT lang, UINT type, const WCHAR* strTable[] )
        {
            m_iLanguage = lang;
            m_dwKeyboardType = type;
            m_strTable = (const WCHAR**)strTable;
        };
        const UINT GetLanguage() { return m_iLanguage; };
        const UINT GetKeyboardType() { return m_dwKeyboardType; };
        const WCHAR** GetStringTable() { return m_strTable; };

    private:
        UINT m_iLanguage;
        UINT m_dwKeyboardType;
        const WCHAR** m_strTable;
    };
private:
    //-------------------------------------------------------------------------
    // Types
    //-------------------------------------------------------------------------

    typedef std::vector< Key > KeyRow;              // list of keys in a row
    typedef KeyRow::size_type KeyIndex;

    typedef std::vector< KeyRow > Keyboard;         // list of rows = keyboard
    typedef Keyboard::size_type RowIndex;

    typedef std::vector< Keyboard > KeyboardList;   // alpha, symbol, etc.
    typedef KeyboardList::size_type BoardIndex;

    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

    CXBPackedResource  m_xprResource;      // General app packed resource

    mutable CXBFont    m_Font18;           // Font renderer for most keys
    mutable CXBFont    m_Font12;           // Font renderer for capital name keys
    mutable CXBFont    m_FontButtons;      // Xbox Button font

    BOOL               m_bIsCapsLockOn;    // Caps lock status
    BOOL               m_bIsShiftOn;       // Sticky shift key status
    State              m_State;            // Current state
    UINT               m_iKeyboard;        // Keyboard
    UINT               m_iLanguage;        // Current language
    String             m_strData;          // Current string
    StringIndex        m_iPos;             // Current pos of caret in string
    KeyboardList       m_KeyboardList;     // All potential keyboards
    BoardIndex         m_iCurrBoard;       // Current keyboard
    RowIndex           m_iCurrRow;         // Current row
    KeyIndex           m_iCurrKey;         // Current key in the current row           
    KeyIndex           m_iLastColumn;      // Column of last single char key
    CXBStopWatch       m_RepeatTimer;      // Controller button repeat timer
    FLOAT              m_fRepeatDelay;     // Time between button repeats
    CXBStopWatch       m_CaretTimer;       // Controls caret blink
    LPDIRECT3DTEXTURE8 m_pKeyTexture;      // Keyboard "key" texture
    
    
    KeyboardInfo*      m_pCurrKeyboard;    // Current keyboard information
    FLOAT              m_fKeyHeight;       // Height of keys
    DWORD              m_dwMaxRows;        // Rows of keys

	D2Xmedialib*	   p_ml;

public:

    CXBVirtualKeyboard();

    virtual HRESULT Initialize();
    virtual void FrameMove();
    virtual void Render();
    //virtual void OnAction(const CAction &action);
	virtual void OnAction(int action);
    void Reset();
    void SetText(const WCHAR* wszText);
    const WCHAR* GetText();
    bool IsConfirmed() const;

	void SetPosY(FLOAT y_textbox, FLOAT y_keyboard);

private:
    bool m_bConfirmed;
    VOID ValidateState() const;
    VOID InitBoard();
    
    Event GetEvent();
    Event GetControllerEvent();

    VOID UpdateState( Event );
    VOID PressCurrent();
    VOID Press( Xkey );
    VOID MoveUp();
    VOID MoveDown();
    VOID MoveLeft();
    VOID MoveRight();
    VOID SetLastColumn();

    VOID DrawTextBox() const;
    
    VOID RenderKeyboardLatin() const;
    VOID RenderKey( FLOAT fX, FLOAT fY, const Key & pKey, D3DCOLOR KeyColor,
                    D3DCOLOR TextColor ) const;
    VOID DrawText( FLOAT x, FLOAT y ) const;
    VOID DrawRedUnderlinedText( FLOAT x, FLOAT y, DWORD, const WCHAR*, DWORD ) const;
    BOOL IsKeyDisabled() const;
    
    WCHAR GetChar( Xkey ) const;
    static WCHAR ToUpper( WCHAR );
    static WCHAR ToLower( WCHAR );
    void SelectKeyboard( UINT iKeyboard );
};

extern const WCHAR**      m_pStringTable;     // Current string table
const WCHAR* GetString( const WCHAR* strTable[], UINT dwStringID );
const WCHAR* GetString( UINT dwStringID );

enum StringID
{
    STR_MENU_KEYBOARD_NAME,
    STR_MENU_CHOOSE_KEYBOARD,
    STR_MENU_ILLUSTRATIVE_GRAPHICS,
    STR_MENU_A_SELECT,
    STR_MENU_B_BACK,
    STR_MENU_Y_HELP,
    STR_KEY_SPACE,
    STR_KEY_BACKSPACE,
    STR_KEY_SHIFT,
    STR_KEY_CAPSLOCK,
    STR_KEY_ALPHABET,
    STR_KEY_SYMBOLS,
    STR_KEY_ACCENTS,
    STR_KEY_DONE,
    STR_HELP_SELECT,
    STR_HELP_CANCEL,
    STR_HELP_TOGGLE,
    STR_HELP_HELP,
    STR_HELP_BACKSPACE,
    STR_HELP_SPACE,
    STR_HELP_TRIGGER,

    STR_MAX,
};

#endif // XBVIRTUAL_KEYBOARD_H

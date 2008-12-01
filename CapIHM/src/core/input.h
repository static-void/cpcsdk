//
// Caprice32 input manager
//

#ifndef _INPUT_H_
#define _INPUT_H_

#include "cap32type.h"
#include <SDL_keysym.h>
class t_CPC;

#define MOD_CPC_SHIFT   (0x01 << 8)
#define MOD_CPC_CTRL    (0x02 << 8)
#define MOD_EMU_KEY     (0x10 << 8)

#define MOD_PC_SHIFT    /*wxMOD_SHIFT */(KMOD_SHIFT << 16)
#define MOD_PC_CTRL     /*wxMOD_CONTROL */(KMOD_CTRL << 16)
#define MOD_PC_MODE     /*wxMOD_ALT */(KMOD_MODE << 16)

#define KBD_MAX_ENTRIES 160

typedef enum {
	CAP32_EXIT = MOD_EMU_KEY,
	CAP32_FPS,
	CAP32_FULLSCRN,
	CAP32_JOY,
	CAP32_LOADDRVA,
	CAP32_LOADDRVB,
	CAP32_LOADSNAP,
	CAP32_LOADTAPE,
	CAP32_MF2RESET,
	CAP32_MF2STOP,
	CAP32_OPTIONS,
	CAP32_PAUSE,
	CAP32_RESET,
	CAP32_SAVESNAP,
	CAP32_SCRNSHOT,
	CAP32_SPEED,
	CAP32_TAPEPLAY,
	CAP32_DEBUG
} CAP32_KEYS;

class t_Input
{
private:
	//! CPC keyboard layout (English, French, Spanish)
	static dword		cpc_kbd[3][149];
	//! PC keyboard layout (English, French, Spanish)
	static int			kbd_layout[4][KBD_MAX_ENTRIES][2];
	static int			joy_layout[12][2];
public:
	byte				keyboard_matrix[16];
	dword				keyboard_normal[SDLK_LAST];
	dword				keyboard_shift[SDLK_LAST];
	dword				keyboard_ctrl[SDLK_LAST];
	dword				keyboard_mode[SDLK_LAST];
public:
	
	void Reset();

	void input_swap_joy (t_CPC &CPC);
	int input_init (t_CPC &CPC);	
};

typedef enum {
	CPC_0,
	CPC_1,
	CPC_2,
	CPC_3,
	CPC_4,
	CPC_5,
	CPC_6,
	CPC_7,
	CPC_8,
	CPC_9,
	CPC_A,
	CPC_B,
	CPC_C,
	CPC_D,
	CPC_E,
	CPC_F,
	CPC_G,
	CPC_H,
	CPC_I,
	CPC_J,
	CPC_K,
	CPC_L,
	CPC_M,
	CPC_N,
	CPC_O,
	CPC_P,
	CPC_Q,
	CPC_R,
	CPC_S,
	CPC_T,
	CPC_U,
	CPC_V,
	CPC_W,
	CPC_X,
	CPC_Y,
	CPC_Z,
	CPC_a,
	CPC_b,
	CPC_c,
	CPC_d,
	CPC_e,
	CPC_f,
	CPC_g,
	CPC_h,
	CPC_i,
	CPC_j,
	CPC_k,
	CPC_l,
	CPC_m,
	CPC_n,
	CPC_o,
	CPC_p,
	CPC_q,
	CPC_r,
	CPC_s,
	CPC_t,
	CPC_u,
	CPC_v,
	CPC_w,
	CPC_x,
	CPC_y,
	CPC_z,
	CPC_AMPERSAND,
	CPC_ASTERISK,
	CPC_AT,
	CPC_BACKQUOTE,
	CPC_BACKSLASH,
	CPC_CAPSLOCK,
	CPC_CLR,
	CPC_COLON,
	CPC_COMMA,
	CPC_CONTROL,
	CPC_COPY,
	CPC_CPY_DOWN,
	CPC_CPY_LEFT,
	CPC_CPY_RIGHT,
	CPC_CPY_UP,
	CPC_CUR_DOWN,
	CPC_CUR_LEFT,
	CPC_CUR_RIGHT,
	CPC_CUR_UP,
	CPC_CUR_ENDBL,
	CPC_CUR_HOMELN,
	CPC_CUR_ENDLN,
	CPC_CUR_HOMEBL,
	CPC_DBLQUOTE,
	CPC_DEL,
	CPC_DOLLAR,
	CPC_ENTER,
	CPC_EQUAL,
	CPC_ESC,
	CPC_EXCLAMATN,
	CPC_F0,
	CPC_F1,
	CPC_F2,
	CPC_F3,
	CPC_F4,
	CPC_F5,
	CPC_F6,
	CPC_F7,
	CPC_F8,
	CPC_F9,
	CPC_FPERIOD,
	CPC_GREATER,
	CPC_HASH,
	CPC_LBRACKET,
	CPC_LCBRACE,
	CPC_LEFTPAREN,
	CPC_LESS,
	CPC_LSHIFT,
	CPC_MINUS,
	CPC_PERCENT,
	CPC_PERIOD,
	CPC_PIPE,
	CPC_PLUS,
	CPC_POUND,
	CPC_POWER,
	CPC_QUESTION,
	CPC_QUOTE,
	CPC_RBRACKET,
	CPC_RCBRACE,
	CPC_RETURN,
	CPC_RIGHTPAREN,
	CPC_RSHIFT,
	CPC_SEMICOLON,
	CPC_SLASH,
	CPC_SPACE,
	CPC_TAB,
	CPC_UNDERSCORE,
	CPC_J0_UP,
	CPC_J0_DOWN,
	CPC_J0_LEFT,
	CPC_J0_RIGHT,
	CPC_J0_FIRE1,
	CPC_J0_FIRE2,
	CPC_J1_UP,
	CPC_J1_DOWN,
	CPC_J1_LEFT,
	CPC_J1_RIGHT,
	CPC_J1_FIRE1,
	CPC_J1_FIRE2,
	CPC_ES_NTILDE,
	CPC_ES_nTILDE,
	CPC_ES_PESETA,
	CPC_FR_eACUTE,
	CPC_FR_eGRAVE,
	CPC_FR_cCEDIL,
	CPC_FR_aGRAVE,
	CPC_FR_uGRAVE
} CPC_KEYS;
#endif
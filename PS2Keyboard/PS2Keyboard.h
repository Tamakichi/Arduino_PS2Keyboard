/*
  PS2Keyboard.h - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
  ** Modified for use with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Version 2.4.sk001 (May 2015) BY きむ茶工房(きむしげ)
  - 日本語キーボード対応（ [右ALT]+[カナ/かな]キーでカタカナ入力モードに移行）
  - F4 key support from F1. Also change a little key code
  - We have removed German and French keyboard in order to save memory

*/


#ifndef PS2Keyboard_h
#define PS2Keyboard_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h" // for attachInterrupt, FALLING
#else
#include "WProgram.h"
#endif

#include "utility/int_pins.h"

// Every call to read() returns a single byte for each
// keystroke.  These configure what byte will be returned
// for each "special" key.  To ignore a key, use zero.
#define PS2_TAB			9
#define PS2_ENTER			0x0D		// CR
#define PS2_BACKSPACE		0x08		// BS
#define PS2_ESC			0x1B		// EC
#define PS2_INSERT			0
#define PS2_DELETE			0x7F		// DL
#define PS2_HOME			0
#define PS2_END			0
#define PS2_PAGEUP			0
#define PS2_PAGEDOWN		0
#define PS2_UPARROW			0x1E		// use RS as ↑
#define PS2_LEFTARROW		0x1D		// use GS as ←
#define PS2_DOWNARROW		0x1F		// use US as ↓
#define PS2_RIGHTARROW		0x1C		// use FS as →
#define PS2_F1				0x11		// use D1 as F1
#define PS2_F2				0x12		// use D2 as F2
#define PS2_F3				0x13		// use D3 as F3
#define PS2_F4				0x14		// use D4 as F4
#define PS2_F5				0
#define PS2_F6				0
#define PS2_F7				0
#define PS2_F8				0
#define PS2_F9				0
#define PS2_F10			0
#define PS2_F11			0
#define PS2_F12			0
#define PS2_SCROLL			0
#define PS2_KANA			0xA0		// カナ/かな

#define PS2_INVERTED_EXCLAMATION	161 // !
#define PS2_CENT_SIGN			162 // ￠
#define PS2_POUND_SIGN			163 // ￡
#define PS2_CURRENCY_SIGN		164 // ?
#define PS2_YEN_SIGN			165 // \
#define PS2_BROKEN_BAR                 	166 // |
#define PS2_SECTION_SIGN		167 // §
#define PS2_DIAERESIS			168 // ¨
#define PS2_COPYRIGHT_SIGN		169 // c
#define PS2_FEMININE_ORDINAL		170 // a
#define PS2_LEFT_DOUBLE_ANGLE_QUOTE	171 // ≪
#define PS2_NOT_SIGN			172 // ￢
#define PS2_HYPHEN			173
#define PS2_REGISTERED_SIGN		174 // R
#define PS2_MACRON			175 // ￣
#define PS2_DEGREE_SIGN			176 // °
#define PS2_PLUS_MINUS_SIGN		177 // ±
#define PS2_SUPERSCRIPT_TWO		178 // 2
#define PS2_SUPERSCRIPT_THREE		179 // 3
#define PS2_ACUTE_ACCENT		180 // ´
#define PS2_MICRO_SIGN			181 // μ
#define PS2_PILCROW_SIGN		182 // ¶
#define PS2_MIDDLE_DOT			183 // ・
#define PS2_CEDILLA			184 // ，
#define PS2_SUPERSCRIPT_ONE		185 // 1
#define PS2_MASCULINE_ORDINAL		186 // o
#define PS2_RIGHT_DOUBLE_ANGLE_QUOTE	187 // ≫
#define PS2_FRACTION_ONE_QUARTER	188 // ?
#define PS2_FRACTION_ONE_HALF		189 // ?
#define PS2_FRACTION_THREE_QUARTERS	190 // ?
#define PS2_INVERTED_QUESTION MARK	191 // ?
#define PS2_A_GRAVE			192 // A
#define PS2_A_ACUTE			193 // A
#define PS2_A_CIRCUMFLEX		194 // A
#define PS2_A_TILDE			195 // A
#define PS2_A_DIAERESIS			196 // A
#define PS2_A_RING_ABOVE		197 // A
#define PS2_AE				198 // A
#define PS2_C_CEDILLA			199 // C
#define PS2_E_GRAVE			200 // E
#define PS2_E_ACUTE			201 // E
#define PS2_E_CIRCUMFLEX		202 // E
#define PS2_E_DIAERESIS			203 // E
#define PS2_I_GRAVE			204 // I
#define PS2_I_ACUTE			205 // I
#define PS2_I_CIRCUMFLEX		206 // I
#define PS2_I_DIAERESIS			207 // I
#define PS2_ETH				208 // D
#define PS2_N_TILDE			209 // N
#define PS2_O_GRAVE			210 // O
#define PS2_O_ACUTE			211 // O
#define PS2_O_CIRCUMFLEX		212 // O
#define PS2_O_TILDE			213 // O
#define PS2_O_DIAERESIS			214 // O
#define PS2_MULTIPLICATION		215 // ×
#define PS2_O_STROKE			216 // O
#define PS2_U_GRAVE			217 // U
#define PS2_U_ACUTE			218 // U
#define PS2_U_CIRCUMFLEX		219 // U
#define PS2_U_DIAERESIS			220 // U
#define PS2_Y_ACUTE			221 // Y
#define PS2_THORN			222 // T
#define PS2_SHARP_S			223 // s
#define PS2_a_GRAVE			224 // a
#define PS2_a_ACUTE			225 // a
#define PS2_a_CIRCUMFLEX		226 // a
#define PS2_a_TILDE			227 // a
#define PS2_a_DIAERESIS			228 // a
#define PS2_a_RING_ABOVE		229 // a
#define PS2_ae				230 // a
#define PS2_c_CEDILLA			231 // c
#define PS2_e_GRAVE			232 // e
#define PS2_e_ACUTE			233 // e
#define PS2_e_CIRCUMFLEX		234 // e
#define PS2_e_DIAERESIS			235 // e
#define PS2_i_GRAVE			236 // i
#define PS2_i_ACUTE			237 // i
#define PS2_i_CIRCUMFLEX		238 // i
#define PS2_i_DIAERESIS			239 // i
#define PS2_eth				240 // d
#define PS2_n_TILDE			241 // n
#define PS2_o_GRAVE			242 // o
#define PS2_o_ACUTE			243 // o
#define PS2_o_CIRCUMFLEX		244 // o
#define PS2_o_TILDE			245 // o
#define PS2_o_DIAERESIS			246 // o
#define PS2_DIVISION			247 // ÷
#define PS2_o_STROKE			248 // o
#define PS2_u_GRAVE			249 // u
#define PS2_u_ACUTE			250 // u
#define PS2_u_CIRCUMFLEX		251 // u
#define PS2_u_DIAERESIS			252 // u
#define PS2_y_ACUTE			253 // y
#define PS2_thorn			254 // t
#define PS2_y_DIAERESIS			255 // y


#define PS2_KEYMAP_SIZE 136

typedef struct {
	uint8_t noshift[PS2_KEYMAP_SIZE];
	uint8_t shift[PS2_KEYMAP_SIZE];
	uint8_t uses_altgr;
	uint8_t altgr[PS2_KEYMAP_SIZE];
} PS2Keymap_t;


extern const PROGMEM PS2Keymap_t PS2Keymap_JP;
extern const PROGMEM PS2Keymap_t PS2Keymap_US;


/**
 * Purpose: Provides an easy access to PS2 keyboards
 * Author:  Christian Weichel
 */
class PS2Keyboard {
  public:
  	/**
  	 * This constructor does basically nothing. Please call the begin(int,int)
  	 * method before using any other method of this class.
  	 */
    PS2Keyboard();
    
    /**
     * Starts the keyboard "service" by registering the external interrupt.
     * setting the pin modes correctly and driving those needed to high.
     * The propably best place to call this method is in the setup routine.
     * デフォルトは日本語キーボードとする
     */
    static void begin(uint8_t dataPin, uint8_t irq_pin, const PS2Keymap_t &map = PS2Keymap_JP);
    
    /**
     * Returns true if there is a char to be read, false if not.
     */
    static bool available();
    
    /**
     * Returns the char last read from the keyboard.
     * If there is no char availble, -1 is returned.
     */
    static int read();
	static uint8_t getRcevData();
	static void clrarBuffer();
};

#endif

/*
  PS2Keyboard.cpp - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
  ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **
  ** Modified for Arduino STM32 Tamakichi <tamacat2014@gmail.com>  **

  for more information you can read the original wiki in arduino.cc
  at http://www.arduino.cc/playground/Main/PS2Keyboard
  or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html

  Version 2.4.stm32 (January 2017) by Tamakichi
  - Support Arduino STM32(STM32F1xx)

  Version 2.4.sk001 (May 2015) BY きむ茶工房(きむしげ)
  - 日本語キーボード対応（ [右ALT]+[カナ/かな]キーでカタカナ入力モードに移行）
  - F4 key support from F1. Also change a little key code
  - We have removed German and French keyboard in order to save memory

  Version 2.4 (March 2013)
  - Support Teensy 3.0, Arduino Due, Arduino Leonardo & other boards
  - French keyboard layout, David Chochoi, tchoyyfr at yahoo dot fr

  Version 2.3 (October 2011)
  - Minor bugs fixed

  Version 2.2 (August 2011)
  - Support non-US keyboards - thanks to Rainer Bruch for a German keyboard :)

  Version 2.1 (May 2011)
  - timeout to recover from misaligned input
  - compatibility with Arduino "new-extension" branch
  - TODO: send function, proposed by Scott Penrose, scooterda at me dot com

  Version 2.0 (June 2010)
  - Buffering added, many scan codes can be captured without data loss
    if your sketch is busy doing other work
  - Shift keys supported, completely rewritten scan code to ascii
  - Slow linear search replaced with fast indexed table lookups
  - Support for Teensy, Arduino Mega, and Sanguino added

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
*/

#include "PS2Keyboard_stm32.h"

#define BUFFER_SIZE 45
static volatile uint8_t buffer[BUFFER_SIZE];
static volatile uint8_t head, tail;
static uint8_t DataPin;
static uint8_t CharBuffer=0;
static uint8_t UTF8next=0;
static const PS2Keymap_t *keymap=NULL;
static uint8_t kana_f = 0 ;             // [カナ/かな]キーのON/OFF情報

#if defined (__STM32F1__) // < 2017/01/14 modified by Tamakichi
  #define gpio_read(pin) gpio_read_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit)?HIGH:LOW
#endif


// The ISR for the external interrupt
static uint8_t bitcount=0;
static uint8_t incoming=0;
static uint32_t prev_ms=0;
void ps2interrupt(void)
{
	uint32_t now_ms;
	uint8_t n, val;

	
#if defined (__STM32F1__) 
	val = gpio_read(DataPin);
#else	
	val = digitalRead(DataPin);
#endif
	now_ms = millis();
	if (now_ms - prev_ms > 250) {
		bitcount = 0;
		incoming = 0;
	}
	prev_ms = now_ms;
	n = bitcount - 1;
	if (n <= 7) {
		incoming |= (val << n);
	}
	bitcount++;
	if (bitcount == 11) {
		uint8_t i = head + 1;
		if (i >= BUFFER_SIZE) i = 0;
		if (i != tail) {
			buffer[i] = incoming;
			head = i;
		}
		bitcount = 0;
		incoming = 0;
	}
}

static inline uint8_t get_scan_code(void)
{
	uint8_t c, i;

	i = tail;
	if (i == head) return 0;
	i++;
	if (i >= BUFFER_SIZE) i = 0;
	c = buffer[i];
	tail = i;
	return c;
}

// http://www.quadibloc.com/comp/scan.htm
// http://www.computer-engineering.org/ps2keyboard/scancodes2.html

// These arrays provide a simple key map, to turn scan codes into ISO8859
// output.  If a non-US keyboard is used, these may need to be modified
// for the desired output.
//

// 日本語キーマップのデータ
const PROGMEM PS2Keymap_t PS2Keymap_JP = {
  // without shift
	{0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0 /*半角/漢字*/, 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0 /*カナ/かな*/, 0 /*Lctrl*/, 'q', '1', 0,
	0, 0, 'z', 's', 'a', 'w', '2', 0,
	0, 'c', 'x', 'd', 'e', '4', '3', 0,
	0, ' ', 'v', 'f', 't', 'r', '5', 0,
	0, 'n', 'b', 'h', 'g', 'y', '6', 0,
	0, 0, 'm', 'j', 'u', '7', '8', 0,
	0, ',', 'k', 'i', 'o', '0', '9', 0,
	0, '.', '/', 'l', ';', 'p', '-', 0,
	0, 0x5C, ':', 0, '@', '^', 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER, '[', 0, ']', 0, 0,
	0, 0, 0, 0, 0 /*変換*/, 0, PS2_BACKSPACE, 0 /*無変換*/,
	0, '1', 0x5C, '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 },
  // with shift
	{0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0 /*半角/漢字*/, 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0 /*カナ/かな*/, 0 /*Lctrl*/, 'Q', '!', 0,
	0, 0, 'Z', 'S', 'A', 'W', '"', 0,
	0, 'C', 'X', 'D', 'E', '$', 0x23, 0,
	0, ' ', 'V', 'F', 'T', 'R', '%', 0,
	0, 'N', 'B', 'H', 'G', 'Y', '&', 0,
	0, 0, 'M', 'J', 'U', 0x27, '(', 0,
	0, '<', 'K', 'I', 'O', 0, ')', 0,
	0, '>', '?', 'L', '+', 'P', '=', 0,
	0, '_', '*', 0, '`', '~', 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER, '{', 0, '}', 0, 0,
	0, 0, 0, 0, 0 /*変換*/, 0, PS2_BACKSPACE, 0 /*無変換*/,
	0, '1', '|', '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 },
	0
};

// カタカナキーマップのデータ
const PROGMEM PS2Keymap_t PS2Keymap_KANA = {
  // without shift
	{0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0 /*半角/漢字*/, 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0 /*カナ/かな*/, 0 /*Lctrl*/, 0xC0, 0xC7, 0,
	0, 0, 0xC2, 0xC4, 0xC1, 0xC3, 0xCC, 0,
	0, 0xBF, 0xBB, 0xBC, 0xB2, 0xB3, 0xB1, 0,
	0, ' ', 0xCB, 0xCA, 0xB6, 0xBD, 0xB4, 0,
	0, 0xD0, 0xBA, 0xB8, 0xB7, 0xDD, 0xB5, 0,
	0, 0, 0xD3, 0xCF, 0xC5, 0xD4, 0xD5, 0,
	0, 0xC8, 0xC9, 0xC6, 0xD7, 0xDC, 0xD6, 0,
	0, 0xD9, 0xD2, 0xD8, 0xDA, 0xBE, 0xCE, 0,
	0, 0xDB, 0xB9, 0, 0xDE, 0xCD, 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER, 0xDF, 0, 0xD1, 0, 0,
	0, 0, 0, 0, 0 /*変換*/, 0, PS2_BACKSPACE, 0 /*無変換*/,
	0, '1', 0xB0, '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 },
  // with shift
	{0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0 /*半角/漢字*/, 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0 /*カナ/かな*/, 0 /*Lctrl*/, 0, 0, 0,
	0, 0, 0xAF, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0xA8, 0xA9, 0xA7, 0,
	0, ' ', 0, 0, 0, 0, 0xAA, 0,
	0, 0, 0, 0, 0, 0, 0xAB, 0,
	0, 0, 0, 0, 0, 0xAC, 0xAD, 0,
	0, 0xA4, 0, 0, 0, 0xA6, 0xAE, 0,
	0, 0xA1, 0xA5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER, 0xA2, 0, 0xA3, 0, 0,
	0, 0, 0, 0, 0 /*変換*/, 0, PS2_BACKSPACE, 0 /*無変換*/,
	0, '1', 0, '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 },
	0
};

const PROGMEM PS2Keymap_t PS2Keymap_US = {
  // without shift
	{0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
	0, 0, 'z', 's', 'a', 'w', '2', 0,
	0, 'c', 'x', 'd', 'e', '4', '3', 0,
	0, ' ', 'v', 'f', 't', 'r', '5', 0,
	0, 'n', 'b', 'h', 'g', 'y', '6', 0,
	0, 0, 'm', 'j', 'u', '7', '8', 0,
	0, ',', 'k', 'i', 'o', '0', '9', 0,
	0, '.', '/', 'l', ';', 'p', '-', 0,
	0, 0, '\'', 0, '[', '=', 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
	0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
	0, '1', 0, '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 },
  // with shift
	{0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
	0, 0, 'Z', 'S', 'A', 'W', '@', 0,
	0, 'C', 'X', 'D', 'E', '$', '#', 0,
	0, ' ', 'V', 'F', 'T', 'R', '%', 0,
	0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
	0, 0, 'M', 'J', 'U', '&', '*', 0,
	0, '<', 'K', 'I', 'O', ')', '(', 0,
	0, '>', '?', 'L', ':', 'P', '_', 0,
	0, 0, '"', 0, '{', '+', 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
	0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
	0, '1', 0, '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 },
	0
};



#define BREAK     0x01
#define MODIFIER  0x02
#define SHIFT_L   0x04
#define SHIFT_R   0x08
#define ALTGR     0x10

static char get_iso8859_code(void)
{
	static uint8_t state=0;
	uint8_t s;
	char c;

	while (1) {
		s = get_scan_code();
		if (!s) return 0;
		if (s == 0xF0) {
			state |= BREAK;
		} else if (s == 0xE0) {
			state |= MODIFIER;
		} else {
			if (state & BREAK) {
				if (s == 0x12) {
					state &= ~SHIFT_L;
				} else if (s == 0x59) {
					state &= ~SHIFT_R;
				} else if (s == 0x11 && (state & MODIFIER)) {
					state &= ~ALTGR;
				}
				// CTRL, ALT & WIN keys could be added
				// but is that really worth the overhead?
				state &= ~(BREAK | MODIFIER);
				continue;
			}
			if (s == 0x12) {
				state |= SHIFT_L;
				continue;
			} else if (s == 0x59) {
				state |= SHIFT_R;
				continue;
			} else if (s == 0x11 && (state & MODIFIER)) {
				state |= ALTGR;
			}
			c = 0;
			if (state & MODIFIER) {
				switch (s) {
//				  case 0x70: c = PS2_INSERT;      break;
//				  case 0x6C: c = PS2_HOME;        break;
//				  case 0x7D: c = PS2_PAGEUP;      break;
				  case 0x71: c = PS2_DELETE;      break;
//				  case 0x69: c = PS2_END;         break;
//				  case 0x7A: c = PS2_PAGEDOWN;    break;
				  case 0x75: c = PS2_UPARROW;     break;
				  case 0x6B: c = PS2_LEFTARROW;   break;
				  case 0x72: c = PS2_DOWNARROW;   break;
				  case 0x74: c = PS2_RIGHTARROW;  break;
				  case 0x4A: c = '/';             break;
				  case 0x5A: c = PS2_ENTER;       break;
				  default: break;
				}
			} else if (state & ALTGR) {
				// [右ALT]+[カナ/かな]キーが押された
				if (s == 0x13) {
					kana_f = ~kana_f ;
					if (kana_f == 0) {
						keymap = &PS2Keymap_JP ;      // 日本語キーマップに戻す
					} else { 
						keymap = &PS2Keymap_KANA ;    // カタカナキーマップにする
					}
					c = PS2_KANA ; // この行をコメントにすればキーコードは返りません
				}
			} else if (state & (SHIFT_L | SHIFT_R)) {
				if (s < PS2_KEYMAP_SIZE)
					c = pgm_read_byte(keymap->shift + s);
			} else {
				if (s < PS2_KEYMAP_SIZE)
					c = pgm_read_byte(keymap->noshift + s);
			}
			state &= ~(BREAK | MODIFIER);
			if (c) return c;
		}
	}
}

bool PS2Keyboard::available() {
	if (CharBuffer || UTF8next) return true;
	CharBuffer = get_iso8859_code();
	if (CharBuffer) return true;
	return false;
}

int PS2Keyboard::read() {
	uint8_t result;

	result = UTF8next;
	if (result) {
		UTF8next = 0;
	} else {
		result = CharBuffer;
		if (result) {
			CharBuffer = 0;
		} else {
			result = get_iso8859_code();
		}
		if (result >= 0xE0) {    // 128 -> 0xE0 に変更
			UTF8next = (result & 0x3F) | 0x80;
			result = ((result >> 6) & 0x1F) | 0xC0;
		}
	}
	if (!result) return -1;
	return result;
}

PS2Keyboard::PS2Keyboard() {
  // nothing to do here, begin() does it all
}

void PS2Keyboard::begin(uint8_t data_pin, uint8_t irq_pin, const PS2Keymap_t &map) {
  uint8_t irq_num=255;

  DataPin = data_pin;
  keymap = &map;

  // initialize the pins
#if defined (__STM32F1__) // < 2017/01/14 modified by Tamakichi
  irq_num = irq_pin;
  pinMode(irq_pin, INPUT);
  pinMode(data_pin, INPUT);
#else // end>	
#ifdef INPUT_PULLUP
  pinMode(irq_pin, INPUT_PULLUP);
  pinMode(data_pin, INPUT_PULLUP);
#else
  pinMode(irq_pin, INPUT);
  digitalWrite(irq_pin, HIGH);
  pinMode(data_pin, INPUT);
  digitalWrite(data_pin, HIGH);
#endif

#ifdef CORE_INT_EVERY_PIN
  irq_num = irq_pin;

#else
  switch(irq_pin) {
    #ifdef CORE_INT0_PIN
    case CORE_INT0_PIN:
      irq_num = 0;
      break;
    #endif
    #ifdef CORE_INT1_PIN
    case CORE_INT1_PIN:
      irq_num = 1;
      break;
    #endif
    #ifdef CORE_INT2_PIN
    case CORE_INT2_PIN:
      irq_num = 2;
      break;
    #endif
    #ifdef CORE_INT3_PIN
    case CORE_INT3_PIN:
      irq_num = 3;
      break;
    #endif
    #ifdef CORE_INT4_PIN
    case CORE_INT4_PIN:
      irq_num = 4;
      break;
    #endif
    #ifdef CORE_INT5_PIN
    case CORE_INT5_PIN:
      irq_num = 5;
      break;
    #endif
    #ifdef CORE_INT6_PIN
    case CORE_INT6_PIN:
      irq_num = 6;
      break;
    #endif
    #ifdef CORE_INT7_PIN
    case CORE_INT7_PIN:
      irq_num = 7;
      break;
    #endif
  }
#endif
#endif // __STM32F1__  2017/01/14 modified by Tamakichi

  head = 0;
  tail = 0;
  if (irq_num < 255) {
    attachInterrupt(irq_num, ps2interrupt, FALLING);
  }
}

uint8_t PS2Keyboard::getRcevData() {
  uint8_t us=250;
  uint8_t c = 0;
  while(1) {
    c = ::get_scan_code();
    if (c) break;          
    us--;
    if (!us) break;
    delay(1);
  }
  return c;
}

void PS2Keyboard::clrarBuffer() {
  ::bitcount = 0;
  ::incoming = 0;
  ::head = 0;	
}


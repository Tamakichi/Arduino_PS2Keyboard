/*  PS2Keyboard library, International Keyboard Layout Example
    http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html

    keyboard.begin() accepts an optional 3rd parameter to
    configure the PS2 keyboard layout.  Uncomment the line for
    your keyboard.  If it doesn't exist, you can create it in
    PS2Keyboard.cpp and email paul@pjrc.com to have it included
    in future versions of this library.
*/
   
#include <PS2Keyboard.h>

#if defined (__STM32F1__)   
const int DataPin = PB8 ;  // PS2キーボードのDATAピンに接続
const int IRQpin  = PB7 ;  // PS2キーボードのCLKピンに接続
#else
const int DataPin = 8;
const int IRQpin =  5;
#endif

PS2Keyboard keyboard;

void setup() {
  keyboard.begin(DataPin, IRQpin, PS2Keymap_JP);
  //keyboard.begin(DataPin, IRQpin, PS2Keymap_US);
  //keyboard.begin(DataPin, IRQpin, PS2Keymap_German);
  //keyboard.begin(DataPin, IRQpin, PS2Keymap_French);
  Serial.begin(115200);
#if defined (__STM32F1__)   
  while (!Serial.isConnected()) delay(100);
#endif
  Serial.println("International Keyboard Test:");
}

void loop() {
  if (keyboard.available()) {
    char c = keyboard.read();
    Serial.print(c);
  }
}

// ＰＳ２日本語キーボードのテスト
#include <PS2Keyboard_stm32.h>

#if defined (__STM32F1__)   
const int DataPin = PB8 ;  // PS2キーボードのDATAピンに接続
const int IRQpin  = PB7 ;  // PS2キーボードのCLKピンに接続
#else
const int DataPin = 4 ;  // PS2キーボードのDATAピンに接続
const int IRQpin  = 3 ;  // PS2キーボードのCLKピンに接続
#endif

int kana_flg = 0 ;

PS2Keyboard keyboard ;

void setup()
{
     keyboard.begin(DataPin, IRQpin) ;

     Serial.begin(115200);
#if defined (__STM32F1__)   
  while (!Serial.isConnected()) delay(100);
#endif
     Serial.println("International Keyboard Test:") ;
}
void loop()
{
     int c ;

     if (keyboard.available()) {
          // キーデータが有れば読み込む
          c = keyboard.read() ;
          if (c > 0) {
               if (c == PS2_ENTER) Serial.println() ;  // [Enter]キーが押された改行する
               else {
                    if (c == PS2_KANA) {               // [右ALT]+[カナ/かな]キーが押された
                         kana_flg = ~kana_flg ;
                         if (kana_flg == 0) Serial.println("\r\nKANA OFF") ;
                         else               Serial.println("\r\nKANA ON") ;
                    } else  Serial.write(c) ;          // キーデータを表示する
               }
          }
     }
}


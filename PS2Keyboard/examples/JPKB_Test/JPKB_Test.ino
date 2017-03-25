// ＰＳ２日本語キーボードのテスト
#include <PS2Keyboard.h>

const int DataPin = 4 ;  // PS2キーボードのDATAピンに接続
const int IRQpin  = 3 ;  // PS2キーボードのCLKピンに接続

int kana_flg = 0 ;

PS2Keyboard keyboard ;

void setup()
{
     keyboard.begin(DataPin, IRQpin) ;
     Serial.begin(9600) ;
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


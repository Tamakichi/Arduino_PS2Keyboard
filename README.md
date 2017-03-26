# Arduino_PS2Keyboard
PS2Keyboard 2.4をArduino STM32対応を追加したバージョンです. 
AVR系のArduinoでも利用出来ます.   

以下の修正版公開ライブラリをベースに更に修正を行っています.  

- きむ茶工房ガレージハウス ＰＳ２キーボードを接続してみます 
  日本語キーボード対応版
  http://www.geocities.jp/zattouka/GarageHouse/micon/Arduino/PS2/PS2.htm  
  PS2Keyboard.zip(Version 2.4.sk001) 

- オリジナル版 PS2Keyboard 2.4  
  PJRC - PS2Keyboard Library  
  https://www.pjrc.com/teensy/td_libs_PS2Keyboard.html  

## STM32ボードで利用する際の注意点  
PS/2 I/FのCLK、DATAの信号電圧に5Vを利用する場合は、  
5Vトレラント(5V耐性)のピンを利用して下さい。  
SM32F103C8T6の5Vトレラント対応ピン  
- PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15
- PB3,PB4,PB6,PB7,PB8,PB9,PB10,PB11PB12,PB13,PB14,PB15

## インストール
フォルダ**PS2Keyboard**をライブラリの配置フォルダに置いて下さい.  

本ライブラリはAVR版Arduinoにも対応していますが、  
別バージョンのPS2Keyboardと併用して利用したい場合はライブラリの配置先を  
Arduino STM32用のライブラリに配置して下さい.  
**\hardware\Arduino_STM32\STM32F1\libraries**  

フォルダ名が重複する場合は、PS2Keyboard_stm32等にフォルダ名を変更して配置して下さい.  

## 結線図
PB7をCLK、PB8をDATAとして利用する場合の結線を下記に示します。  
CLK、DATAは10kΩの1/4W抵抗にてプルアップします。  
![USBコネクタ結線図](https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard/blob/master/img/03.jpg)

### ブレッドボード上に実装した例
![USBコネクタ接続例](https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard/blob/master/img/02.jpg)

### PS/2コネクタ接続構成
PS/2コネクタはMini DIN 6pinメスコネクタを利用します。  
![PS/2コネクタ結線図](https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard/blob/master/img/07.jpg)

#### 結線図
PB7(CLK)をDIN 6pin端子の5番端子、PB8(DATA)を1番ピンに接続します。  
3番ピンはGND、4番ピンは+5Vに結線します。  
また、CLK、DATAは10kΩの1/4W抵抗にてプルアップします。  
![PS/2コネクタ結線図](https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard/blob/master/img/04.jpg)

#### ブレッドボード上に実装した例
![PS/2コネクタ接続例](https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard/blob/master//img/06.jpg)

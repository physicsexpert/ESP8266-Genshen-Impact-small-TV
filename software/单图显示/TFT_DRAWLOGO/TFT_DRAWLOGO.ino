//#include <NoSignal.h>
#include <avr/pgmspace.h>
#include <SPI.h>//导入库
#include <TFT_eSPI.h>
#define LCD_BL_PIN 5
int LCD_BL_PWM = 90;//屏幕亮度0-100

extern const uint16_t NoSignal[0x4000] PROGMEM;
extern const uint16_t Pic[10][0x1000] PROGMEM;
TFT_eSPI tft = TFT_eSPI();
int i;
void setup() {
  
  pinMode(4, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(4, HIGH);
  pinMode(LCD_BL_PIN, OUTPUT);
  analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10));
  tft.init();
  tft.setSwapBytes(true);// RGB->BGR 加上这句才能正确显示颜色。
  tft.pushImage(0, 0, 240, 240, NoSignal);
 

}

void loop() {
 

}

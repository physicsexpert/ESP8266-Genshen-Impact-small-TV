//wifi微信配网
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <Ticker.h>
#define HOST_NAME  "bemfa"
char config_flag = 0;
#define MAGIC_NUMBER 0xAA
/**
  结构体，用于存储配网信息
*/
struct config_type
{
  char stassid[32];
  char stapsw[64];
  char cuid[40];
  char ctopic[32];
  uint8_t reboot;
  uint8_t magic;
};
config_type config;
void doSmartconfig();
void saveConfig();
void initWiFi();
void loadConfig();
void restoreFactory();
void waitKey();




//WIFI

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>



#include <NTPClient.h>
#include <WiFiUdp.h>






#include <avr/pgmspace.h>
#include <SPI.h>//导入库
#include <TFT_eSPI.h>


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.aliyun.com"); //NTP地址



//const char* ssid     = "131";       // 连接WiFi名
const char* ssid     = "zhuaidong";       // 连接WiFi名
// 请将您需要连接的WiFi名填入引号中
//const char* password = "chenyp007476";          // 连接WiFi密码
const char* password = "580780616";          // 连接WiFi密码
// 请将您需要连接的WiFi密码填入引号中


const char* host = "api.seniverse.com";     // 将要连接的服务器地址
const int httpPort = 80;                    // 将要连接的服务器端口



// 心知天气HTTP请求所需信息
String reqUserKey = "S4UO4kJtzQ-GyjhO-";   // 私钥
String reqLocation = "Nanjing";            // 城市
String reqUnit = "c";                      // 摄氏/华氏

String reqRes;



#define LCD_BL_PIN 5
#define Long_Press_Time_Judge 250  //超过这个值就表示长按，且注意两次点按的间隔不能超过这个值
#define Long_Press  1
#define Short_Press 2
int LCD_BL_PWM = 30;//屏幕亮度0-100


extern const uint16_t diantong[0x2710] PROGMEM;
extern const uint16_t usb[0x2710] PROGMEM;
extern const uint16_t shezhi[0x2710] PROGMEM;
extern const uint16_t liangdu[0x2710] PROGMEM;
extern const uint16_t tupian[0x2710] PROGMEM;
extern const uint16_t wifi[0x2710] PROGMEM;
extern const uint16_t yuanshen[0xE100] PROGMEM;
extern const uint16_t shijian[0x2710] PROGMEM;
extern const uint16_t tianqi[0x26AC] PROGMEM;
extern const uint16_t  wendi[0xE100]  PROGMEM;
//extern const uint16_t yuanshen2[0xE100] PROGMEM;
extern const uint16_t riqi[0x2710] PROGMEM ;
extern const uint16_t huifu[0x2710] PROGMEM ;

extern const uint16_t   jingdutiao4[0x400] PROGMEM ;
extern const uint16_t   jingdutiao5[0x3C0] PROGMEM ;
extern const uint16_t   jingdutiao6[0x3E0] PROGMEM ;
extern const uint16_t   jingdutiao7[0x3C0] PROGMEM ;
extern const uint16_t   jingdutiao8[0x420] PROGMEM ;
extern const uint16_t   jingdutiao9[0x400] PROGMEM ;
extern const uint16_t   jingdutiao10[0x360] PROGMEM ;

extern const uint16_t paimen1[0x2710] PROGMEM;
extern const uint16_t paimen2[0x2710] PROGMEM;
extern const uint16_t paimen3[0x2710] PROGMEM;
extern const uint16_t paimen4[0x2710] PROGMEM;
extern const uint16_t paimen5[0x2710] PROGMEM;
extern const uint16_t paimen6[0x2710] PROGMEM;

extern const uint16_t peiwang[0x57E4]PROGMEM;


TFT_eSPI tft = TFT_eSPI();

uint16_t torchlight = 10;//手电筒亮度
uint8_t i = 1;
uint16_t a;
uint8_t n ;//tupian
uint8_t buttonState = 0;         // 设置初始状态为0低电平
uint8_t key;
uint8_t nowstate = 0;
uint8_t back = 0;
uint8_t serial = 0;
uint16_t ms;
uint16_t s;
uint16_t m;
uint16_t h;


//WIFI配网函数
/**
  airkiss配网
*/
void doSmartconfig() {
  Serial.print(" Smartconfig begin,Waiting for WeChat Config.....");
  WiFi.mode(WIFI_STA);
  WiFi.stopSmartConfig();
  WiFi.beginSmartConfig();
  int cnt = 0;
  bool flag_ok = false;
  while (!WiFi.smartConfigDone()) {
    delay(300);
    if (flag_ok == true) continue;
    if (WiFi.smartConfigDone()) {
      strcpy(config.stassid, WiFi.SSID().c_str());
      strcpy(config.stapsw, WiFi.psk().c_str());
      config.magic = 0xAA;
      saveConfig();
      flag_ok = true;

    }
    cnt++;
    if (cnt >= 600) {
      delayRestart(0);
    }
  }
}

/**
  存储配网信息
*/
void saveConfig() {
  int rand_key;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  config.reboot = 0;
  EEPROM.begin(512);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    EEPROM.write(i, *(p + i));
  }
  EEPROM.commit();
}
Ticker delayTimer;
void delayRestart(float t) {
  delayTimer.attach(t, []() {
    ESP.restart();
  });
}

/**
  初始化wifi信息，并连接路由器网络
*/
void initWiFi() {
  char temp[32];
  uint8_t mac[6];
  int e = 0;
  WiFi.macAddress(mac);
  sprintf(temp, "%s_%02X%02X%02X", HOST_NAME, mac[3], mac[4], mac[5]);
  WiFi.hostname(temp);
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();//断开连接
    WiFi.mode(WIFI_STA);//STA模式
    WiFi.begin(config.stassid, config.stapsw);//连接路由器
  }
  while (WiFi.status() != WL_CONNECTED)
  { //检查是否连接成功
    delay(500);

    waiting  (e);
    delay(500);
    e++;
    if (n = e)
    {
      tft.fillScreen(TFT_WHITE);
      n = 0;
    }
    Serial.print(".");
  }
  if (config_flag == 1) {
    setConfig();
  }
  tft.fillScreen(TFT_WHITE); //用某一颜色填充屏幕
  tft.setTextSize(2);
  tft.setCursor(10, 20, 1);
  tft.print("wifi config ok");
  Serial.println("wifi config ok");
  WiFi.softAP(temp);
}
/**
  设置SmartConfig
*/
void setConfig() {
  String mac = WiFi.macAddress().substring(8);//取mac地址做主题用
  mac.replace(":", "");//去掉:号
  WiFiClient client_bemfa_WiFiClient;
  HTTPClient http_bemfa_HTTPClient;
  http_bemfa_HTTPClient.begin(client_bemfa_WiFiClient, "http://pro.bemfa.com/vv/setSmartConfig?version=1&user=" + mac);
  int httpCode = http_bemfa_HTTPClient.GET();
  if (httpCode == 200) {
    Serial.println("wifi smartconfig ok");
  }
  http_bemfa_HTTPClient.end();
}

/**
  加载存储的信息，并检查是否进行了反复5次重启恢复出厂信息
*/
uint8_t *p = (uint8_t*)(&config);
void loadConfig()
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  EEPROM.begin(512);
  for (int i = 0; i < sizeof(config); i++)
  {
    *(p + i) = EEPROM.read(i);
  }
  config.reboot = config.reboot + 1;
  if (config.reboot >= 4) {
    restoreFactory();
  }
  if (config.magic != 0xAA) {
    config_flag = 1;
  }
  EEPROM.begin(512);
  for (int i = 0; i < sizeof(config); i++) {
    EEPROM.write(i, *(p + i));
  }
  EEPROM.commit();
  delay(2000);
  EEPROM.begin(512);
  config.reboot = 0;
  for (int i = 0; i < sizeof(config); i++) {
    EEPROM.write(i, *(p + i));
  }
  EEPROM.commit();
  delay(2000);
}
/**
  恢复出厂设置，清除存储的wifi信息
*/
void restoreFactory() {
  Serial.println("Restore Factory....... ");
  tft.fillScreen(TFT_WHITE); //用某一颜色填充屏幕
  tft.setTextSize(2);
  tft.setCursor(10, 20, 1);
  tft.print("Restoring......");
  config.magic = 0x00;
  strcpy(config.stassid, "");
  strcpy(config.stapsw, "");
  config.magic = 0x00;
  saveConfig();
  delayRestart(1);
  delay(2000);

  //  while (1) {
  //    delay(100);
  //  }
}
/**
  检查是否需要airkiss配网
*/
void waitKey()
{
  if (config_flag == 1) {
    doSmartconfig();
  }
}






//WIFI函数

// 向心知天气服务器服务器请求信息并对信息进行解析
void httpRequest(String reqRes) {
  WiFiClient client;

  // 建立http请求信息
  String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Connection: close\r\n\r\n";


  // 尝试连接服务器
  if (client.connect(host, 80)) {


    // 向服务器发送http请求信息
    client.print(httpRequest);


    // 获取并显示服务器响应状态行
    String status_response = client.readStringUntil('\n');


    // 使用find跳过HTTP响应头
    if (client.find("\r\n\r\n")) {
      Serial.println("Found Header End. Start Parsing.");
    }

    // 利用ArduinoJson库解析心知天气响应信息
    parseInfo(client);
  } else {
    Serial.println(" connection failed!");
  }
  //断开客户端与服务器连接工作
  client.stop();
}

// 连接WiFi
void connectWiFi() {
  WiFi.begin(ssid, password);                  // 启动网络连接

  int toptime = 0;
  int i = 0;                                   // 这一段程序语句用于检查WiFi是否连接成功
  while (WiFi.status() != WL_CONNECTED)
  {
    waiting  (i);
    toptime += 1;
    i += 1;

    delay(2600); // 如果WiFi连接成功则返回值为WL_CONNECTED
    if (toptime == 8)
    {
      toptime = 0;
      tft.setTextSize(3);
      tft.setCursor(10, 20, 1);
      tft.print("SB WIFI");//显示文字
    }
  }

  while (i < 7)
  {

    waiting  (i);
    delay(300);
    i++;


  }


}

// 利用ArduinoJson库解析心知天气响应信息
void parseInfo(WiFiClient client) {
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 230;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, client);

  JsonObject results_0 = doc["results"][0];

  JsonObject results_0_now = results_0["now"];
  const char* results_0_now_text = results_0_now["text"]; // "Sunny"
  const char* results_0_now_code = results_0_now["code"]; // "0"
  const char* results_0_now_temperature = results_0_now["temperature"]; // "32"

  const char* results_0_last_update = results_0["last_update"]; // "2020-06-02T14:40:00+08:00"

  // 通过串口监视器显示以上信息
  String results_0_now_text_str = results_0_now["text"].as<String>();
  int results_0_now_code_int = results_0_now["code"].as<int>();
  int results_0_now_temperature_int = results_0_now["temperature"].as<int>();

  String results_0_last_update_str = results_0["last_update"].as<String>();


  tft.fillScreen(TFT_WHITE); //用某一颜色填充屏幕
  tft.setTextColor(TFT_BLUE, TFT_WHITE);
  tft.setCursor(10, 20, 1); //
  tft.setTextSize(4);
  tft.println(reqLocation );//显示文字
  tft.setTextColor(TFT_RED, TFT_WHITE);
  tft.setCursor(10, 100, 1); //
  tft.setTextSize(3);
  tft.println(results_0_now_text_str);//显示文字
  tft.setTextColor(TFT_GREEN, TFT_WHITE);
  tft.setCursor(10, 180, 1);
  tft.setTextSize(4);
  tft.println(results_0_now_temperature_int ); //显示文字
  tft.setTextColor(TFT_GREEN, TFT_WHITE);
  tft.setCursor(100, 180, 1);
  tft.setTextSize(4);
  tft.println("C" ); //显示文字
  tft.setTextColor(TFT_BLACK, TFT_WHITE);


}











uint8_t Key_Scan(void)
{
  uint8_t long_press_flag = 0; //长按标志位
  uint8_t short_press_flag = 0; //点按标志位
  buttonState = digitalRead(15);
  if (buttonState == 1)
  {
    delay(10);     //按键消抖
    buttonState = digitalRead(15);
    if (buttonState == 1)
    {
      short_press_flag = 1;
      delay(250);  //点按的时间不能多于250ms
      buttonState = digitalRead(15);
      if (buttonState == 1)
      {
        long_press_flag = 1;
      } while (buttonState) {
        buttonState = digitalRead(15);
      }
      if (long_press_flag == 1) //先判断是否长按，因为长按中包括了点按
      {
        return Long_Press;
      }
      if (short_press_flag == 1) //后判断是否点按
      {
        return Short_Press;
      }
    }
  }
  return 0;
}


void stopwatch(void)
{
  ms = ms + 10;
  delay(2);

  if (ms == 1000)
  {

    ms = 0;
    s = s + 1;
  }

  if (s == 60)  //如果是分钟计数状态
  {
    s = 0;
    m = m + 1;
    tft.setCursor(90, 90, 1);
    tft.print("  ");//显示文字
  }
  if (m == 60)  //如果是分钟计数状态
  {
    m = 0;
    h = h + 1;
    tft.setCursor(10, 90, 1);
    tft.print("  "); //显示文字
  }
  tft.setTextSize(3);
  tft.setCursor(120, 20, 1);
  tft.print(h ); //显示文字
  tft.setTextSize(5);
  tft.setCursor(10, 90, 1);
  tft.print(m ); //显示文字
  tft.setTextSize(5);
  tft.setCursor(90, 90, 1);
  tft.print(s);//显示文字
  tft.setTextSize(5);
  tft.setCursor(170, 90, 1);
  tft.print(ms / 10); //显示文字




}

void waiting  (int i)
{
  switch (i)
  {
    case  0: tft.pushImage(10, 100 , 32, 32, jingdutiao4); break;
    case  1:  tft.pushImage(40, 100 , 30, 32, jingdutiao5); break;
    case  2: tft.pushImage(70, 100 , 32, 31, jingdutiao6); break;
    case  3: tft.pushImage(105, 100 , 30, 32, jingdutiao7); break;
    case  4:  tft.pushImage(136, 100 , 33, 32, jingdutiao8); break;
    case  5:  tft.pushImage(171, 100 , 32, 32, jingdutiao9); break;
    case  6: tft.pushImage(202, 100 , 27, 32, jingdutiao10); break;


  }





}










void loading  (uint16_t timedelay)
{

  tft.pushImage(10, 100 , 32, 32, jingdutiao4);
  delay(timedelay);
  tft.pushImage(40, 100 , 30, 32, jingdutiao5);
  delay(timedelay);
  tft.pushImage(70, 100 , 32, 31, jingdutiao6);
  delay(timedelay);
  tft.pushImage(105, 100 , 30, 32, jingdutiao7);
  delay(timedelay);
  tft.pushImage(136, 100 , 33, 32, jingdutiao8);
  delay(timedelay);
  tft.pushImage(171, 100 , 32, 32, jingdutiao9);
  delay(timedelay);
  tft.pushImage(202, 100 , 27, 32, jingdutiao10);
  delay(timedelay);

}


void setup() {

  pinMode(4, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);//关灯
  digitalWrite(4, HIGH);//关灯

  pinMode(LCD_BL_PIN, OUTPUT);
  analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10));
  analogWriteFreq(20000);
  tft.init();
  tft.setSwapBytes(true);// RGB->BGR 加上这句才能正确显示颜色
  tft.fillScreen(TFT_WHITE); //用某一颜色填充屏幕
  tft.setTextColor(TFT_BLACK, TFT_WHITE);//黑色汉字，白色背景


  // 初始化D0为信号采集针脚:
  pinMode(15, INPUT);
  loading(200);
  tft.fillScreen(TFT_WHITE);

}

void loop()
{




  key = Key_Scan();
  if (i == 11)
  {
    i = 1;
  }
  if (key == 2)
  {
    i += 1;
  }
  switch (i)
  {
    case 1 :  tft.pushImage(70, 70, 100, 100, diantong); nowstate = 1;  break;
    case 2 : tft.pushImage(70, 70, 100, 100, usb); nowstate = 2;  break;
    case 3:  tft.pushImage(70, 70, 100, 100, shezhi); nowstate = 3;   break;
    case 4 : tft.pushImage(70, 70, 100, 100, liangdu); nowstate = 4;   break;
    case 5 :  tft.pushImage(70, 70, 100, 100, tupian); nowstate = 5;   break;
    case 6 : tft.pushImage(70, 70, 100, 100, wifi); nowstate = 6;   break;
    case 7 : tft.pushImage(70, 70, 100, 100, shijian); nowstate = 7;   break;
    case 8 : tft.pushImage(70, 70, 100, 99, tianqi); nowstate = 8;   break;
    case 9 : tft.pushImage(70, 70, 100, 99, riqi); nowstate = 9;   break;
    case 10 : tft.pushImage(70, 70, 100, 99, huifu); nowstate = 10;   break;
  }


  if (key == 1)
  {
    switch (nowstate)
    {
      case 1 :
        tft.fillScreen(TFT_WHITE);
        while (back != 1)
        {

          key = Key_Scan();
          if (key == 1)
          {
            back = 1;
          }
          if (key == 2)
          {
            torchlight += 10;
          }
          if (torchlight == 100)
          {
            torchlight = 10;
          }
          tft.setTextSize(8);
          tft.setCursor(60, 100, 1);
          tft.print(torchlight  );
          tft.setTextSize(8);
          tft.setCursor(160, 100, 1);
          tft.print("%");
          tft.setTextSize(3);
          tft.setCursor(10, 10, 1);
          tft.print("flashlight");
          analogWrite(12, 1023 - torchlight);
          analogWrite(4, 1023 - torchlight);
          delay(10);
        }

        tft.fillScreen(TFT_WHITE);
        digitalWrite(12, HIGH);
        digitalWrite(4, HIGH);
        back = 0;
        break;

      case 2 :
        tft.fillScreen(TFT_WHITE);
        while (back != 1)
        {

          key = Key_Scan();
          if (key == 1)
          {
            back = 1;
          }


          Serial.begin(9600);
          tft.setTextSize(3);
          tft.setCursor(10, 10, 1);
          tft.print("serial begin 9600 bps");//显示文字
          tft.drawRect(5, 5, 230, 60, TFT_BLACK);
          tft.drawRect(5, 70, 230, 165, TFT_BLACK);
          if (Serial.available()) {                     // 当串口接收到信息后

            tft.setTextSize(3);
            tft.setCursor(10, 10, 1);
            tft.print("serial begin 9600");//显示文字
            String serialData = Serial.readString();    // 将接收到的信息使用readString()存储于serialData变量
            tft.setCursor(5, 90, 1);
            tft.print("                                                                                                          ");//显示文字
            tft.setCursor(10, 90, 1);
            tft.print(serialData);//显示文字

          }
        }


        tft.fillScreen(TFT_WHITE);
        Serial.end();
        back = 0;
        break;

      case 3:
        tft.fillScreen(TFT_WHITE);
        while (back != 1)
        {

          key = Key_Scan();
          if (key == 1)
          {
            back = 1;
          }
          tft.drawRect(5, 5, 230, 65, TFT_BLACK);
          tft.setTextSize(2);
          tft.setCursor(10, 10, 1);
          tft.print("hardware     V4.0");
          tft.setCursor(10, 45, 1);
          tft.print("software     V1.0");
          tft.drawRect(5, 80, 230, 70, TFT_BLACK);
          tft.setCursor(10, 90, 1);
          tft.print(ssid);
          tft.setCursor(10, 125, 1);
          tft.print(password);
          tft.drawRect(5, 160, 230, 35, TFT_BLACK);
          tft.setCursor(10, 170, 1);
          tft.print("Created By XYC");
          tft.drawRect(5, 200, 230, 35, TFT_BLACK);
          tft.setCursor(10, 210, 1);
          tft.print("Genshen Edition");
          delay(10);
        }

        tft.fillScreen(TFT_WHITE);

        back = 0;
        break;

      case 4 :
        tft.fillScreen(TFT_WHITE);
        tft.setTextSize(3);
        tft.setCursor(10, 10, 1);
        tft.print("brightness");//显示文字
        while (back != 1)
        {
          key = Key_Scan();
          if (key == 1) {
            back = 1;
          }
          if (key == 2) {
            LCD_BL_PWM += 5;
            analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10)) ;
            tft.setTextSize(10);
            tft.setCursor(60, 90, 1);
            tft.print("00");//显示文字
            tft.setCursor(60, 90, 1);
            tft.print(LCD_BL_PWM);//显示文字

            tft.setCursor(160, 90, 1);
            tft.print("%");//显示文字
            if (LCD_BL_PWM == 95) {
              LCD_BL_PWM = 5;
            }
          }
          delay(100);
        }
        tft.fillScreen(TFT_WHITE);
        back = 0;
        break;




      case 5 :
        tft.fillScreen(TFT_WHITE);
        while (back != 1)
        {
          key = Key_Scan();


          if (key == 1) {
            back = 1;
          }
          if (key == 2) {
            n += 1;
          }
          if (n == 3) {
            n = 1;
          }
          switch (n) {

            case 1 : tft.pushImage(0, 0, 240, 240, wendi); break;
            case 2 : tft.pushImage(0, 0, 240, 240, yuanshen); break;
              //            case 3 : tft.pushImage(0, 0, 240, 240, yuanshen2); break;




          }





          delay(10);
        }

        tft.fillScreen(TFT_WHITE);
        back = 0;
        break;


      case 6 :
        tft.fillScreen(TFT_WHITE);
        Serial.begin(115200);
        tft.pushImage(40, 40, 150, 150, peiwang);

        loadConfig();
        waitKey();
        initWiFi();
        while (back != 1)
        {
          key = Key_Scan();
          if (key == 1)
          {
            back = 1;
          }


          //          tft.drawRect(5, 5, 230, 65, TFT_BLACK);
          //          tft.setTextSize(2);
          //          tft.setCursor(10, 10, 1);
          //          tft.print("WIFI SSID:");
          //          tft.setCursor(10, 50, 1);
          //          tft.print( ssid );
          //          tft.drawRect(5, 80, 230, 70, TFT_BLACK);
          //          tft.setCursor(10, 90, 1);
          //          tft.print("password:");
          //          tft.setCursor(10, 130, 1);
          //          tft.print( password);

          delay(60);
        }

        tft.fillScreen(TFT_WHITE);

        back = 0;
        break;
      case 7 :
        tft.fillScreen(TFT_WHITE);
        while (back != 1)
        {
          tft.setTextSize(3);
          tft.setCursor(80, 200, 1);
          tft.print("START");//显示文字
          tft.setTextSize(5);
          tft.setCursor(62, 90, 1);
          tft.print(":");//显示文字
          tft.setTextSize(5);
          tft.setCursor(142, 90, 1);
          tft.print(":");//显示文字
          tft.setTextSize(3);
          tft.setCursor(10, 20, 1);
          tft.print("HOUR:");//显示文字
          tft.drawRect(5, 5, 230, 70, TFT_BLACK);
          tft.drawRect(5, 80, 230, 70, TFT_BLACK);
          tft.drawRect(5, 180, 230, 55, TFT_BLACK);

          key = Key_Scan();

          if (key == 1)
          {
            back = 1;

          }
          if (key == 2)
          {

            tft.setTextSize(3);
            tft.setCursor(80, 200, 1);
            tft.print("STOP   ");//显示文字
            key = Key_Scan();
            while (key != 2)
            {
              key = Key_Scan();
              stopwatch();
            }
          }
          delay(10);
        }
        ms = 0;
        m = 0;
        s = 0;
        h = 0;
        tft.fillScreen(TFT_WHITE);
        back = 0;
        break;



      case 8 :
        tft.fillScreen(TFT_WHITE);
        connectWiFi();



        // 建立心知天气API当前天气请求资源地址
        reqRes = "/v3/weather/now.json?key=" + reqUserKey +
                 + "&location=" + reqLocation +
                 "&language=en&unit=" + reqUnit;

        // 向心知天气服务器服务器请求信息并对信息进行解析
        httpRequest(reqRes);

        while (back != 1)
        {

          key = Key_Scan();
          if (key == 1)
          {
            back = 1;
          }
          delay(10);
        }

        tft.fillScreen(TFT_WHITE);

        back = 0;
        break;
      case 9 :
        tft.fillScreen(TFT_WHITE);

        connectWiFi();

        tft.fillScreen(TFT_BLACK);
        timeClient.begin();
        timeClient.setTimeOffset(28800); //+1区，偏移3600，+8区，偏移36
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        while (back != 1)
        {

          key = Key_Scan();

          if (key == 1)
          {
            back = 1;
          }
          timeClient.update();
          unsigned long epochTime = timeClient.getEpochTime();

          tft.setTextSize(4);
          tft.setCursor(22, 40,  1);

          tft.print(timeClient.getFormattedTime());


          //          int weekDay = timeClient.getDay();
          //          tft.setTextSize(2);
          //          tft.setCursor(60, 200, 1);
          //          tft.print("Weekday");
          //          tft.setTextSize(2);
          //          tft.setCursor(150, 200, 1);
          //          tft.print(weekDay);



          //将epochTime换算成年月日
          struct tm *ptm = gmtime((time_t *)&epochTime);
          int monthDay = ptm->tm_mday;
          int currentMonth = ptm->tm_mon + 1;
          key = Key_Scan();
          tft.setTextSize(2);
          tft.setCursor(10, 220, 1);
          tft.print("Month");
          tft.setTextSize(2);
          tft.setCursor(80, 220, 1);
          tft.print(currentMonth);
          tft.setTextSize(2);
          tft.setCursor(160, 220, 1);
          tft.print("Day");
          tft.setTextSize(2);
          tft.setCursor(210, 220, 1);
          tft.print(monthDay);
          key = Key_Scan();
          tft.pushImage(70, 90 , 100, 100, paimen1);
          delay(100);
          tft.pushImage(70, 90 , 100, 100, paimen2);
          delay(100);
          tft.pushImage(70, 90 , 100, 100, paimen3);
          delay(100);
          tft.pushImage(70, 90 , 100, 100, paimen4);
          delay(100);
          tft.pushImage(70, 90 , 100, 100, paimen5);
          delay(100);
          tft.pushImage(70, 90 , 100, 100, paimen6);
          delay(100);





        }

        tft.fillScreen(TFT_WHITE);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        back = 0;
        break;
      case 10 :
        tft.fillScreen(TFT_WHITE);
        Serial.begin(115200);
        while (back != 1)
        {

          key = Key_Scan();
          if (key == 1)
          {
            back = 1;
          }
          restoreFactory();
          delay(10);

        }

        tft.fillScreen(TFT_WHITE);

        back = 0;
        break;






    }
  }




}

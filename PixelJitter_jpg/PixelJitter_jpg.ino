// Example for library:
// https://github.com/Bodmer/TJpg_Decoder

// This example if for processors with LittleFS capability (e.g. RP2040,
// ESP32, ESP8266). It renders a Jpeg file that is stored in a LittleFS
// file.

// The test image is in the sketch "data" folder (press Ctrl+K to see it).
// You must upload the image to LittleFS using the Arduino IDE Tools Data
// Upload menu option (you may need to install extra tools for that).

// Include the jpeg decoder library
#include <TJpg_Decoder.h>
// Include LittleFS
#include "LittleFS.h"
#include "SPI.h"

#include <GxEPD2_BW.h>
//#include <GxEPD2_3C.h>
//#include <GxEPD2_7C.h>
#include <U8g2_for_Adafruit_GFX.h>
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
#define baise  GxEPD_WHITE  //白色
#define heise  GxEPD_BLACK  //黑色
//基类gxepd2u GFX可用于将引用或指针作为参数传递到显示实例，使用~1.2k更多代码
//启用或禁用GxEPD2_GFX基类
#define ENABLE_GxEPD2_GFX 1

GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));                             // GDEM029A01
//GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display(GxEPD2_290_T94(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));   // GDEM029T94
//GxEPD2_BW<GxEPD2_290_T5, GxEPD2_290_T5::HEIGHT> display(GxEPD2_290_T5(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));      // GDEW029T5
//GxEPD2_BW<GxEPD2_290_T5D, GxEPD2_290_T5D::HEIGHT> display(GxEPD2_290_T5D(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));   // GDEW029T5D

//GxEPD2_BW<GxEPD2_266_B72, GxEPD2_266_B72::HEIGHT> display(GxEPD2_266_B72(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0266B72

//GxEPD2_BW<GxEPD2_371, GxEPD2_371::HEIGHT> display(GxEPD2_371(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW0371W7
//GxEPD2_BW<GxEPD2_370_TC1, GxEPD2_370_TC1::HEIGHT> display(GxEPD2_370_TC1(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // ED037TC1, Waveshare 3.7"

//GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW042T2
//GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(GxEPD2_420_M01(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW042M01
//GxEPD2_BW<GxEPD2_420_Z96, GxEPD2_420_Z96::HEIGHT> display(GxEPD2_420_Z96 (/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW042Z96
//GxEPD2_BW<GxEPD2_YC0420R_SN, GxEPD2_YC0420R_SN::HEIGHT> display(GxEPD2_YC0420R_SN(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW042Z98

//GxEPD2_BW<GxEPD2_213, GxEPD2_213::HEIGHT> display(GxEPD2_213(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDE0213B1, phased out

//GxEPD2_BW<GxEPD2_154_M09, GxEPD2_154_M09::HEIGHT> display(GxEPD2_154_M09(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW0154M09 200x200


//将jpeg文件解码到
//将每个块渲染到TFT。如果使用不同的TFT库
//您需要调整此功能以适应。
bool epd_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // 停止进一步解码，因为图像正在从屏幕底部溢出
  if ( y >= display.height()) return 0;

  //Serial.println(*bitmap, HEX);
  Serial.print("x:"); Serial.println(x);
  Serial.print("y:"); Serial.println(y);
  Serial.print("w:"); Serial.println(w);
  Serial.print("h:"); Serial.println(h);
  for (int16_t j = 0; j < h; j++, y++)
  {
    for (int16_t i = 0; i < w; i++)
    {
      //uint32_t num = j * w + i;
      Serial.print((j * w + i));
      Serial.print(":");
      Serial.print(bitmap[j * w + i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("");
  //暂未适配墨水屏
  
  //此函数将在TFT边界自动剪裁图像块渲染
  //tft.pushImage(x, y, w, h, bitmap);
  // 如果您修改草图以使用Adafruit_GFX库，这可能会起作用
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // 返回1以解码下一个块
  return 1;
}


void setup()
{
  Serial.begin(74880);
  Serial.println("\n\n Testing TJpg_Decoder library");

  // Initialise LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  //屏幕初始化
  display.init(0, 0, 10, 1); // 串口使能 初始化完全刷新使能 复位时间 ret上拉使能
  //xiaobian();                // 消除黑边
  u8g2Fonts.begin(display);             // 将u8g2过程连接到Adafruit GFX
  u8g2Fonts.setFontMode(1);             // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);        // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(heise);  // 设置前景色
  u8g2Fonts.setBackgroundColor(baise);  // 设置背景色
  display.setRotation(3);  // 设置方向

  // jpeg图像可以按1、2、4或8的比例缩放
  TJpgDec.setJpgScale(1);

  // 必须为解码器提供上述呈现函数的确切名称
  TJpgDec.setCallback(epd_output);
}

void loop()
{
  // 为测试目的记录的时间
  uint32_t t = millis();

  // 如果愿意，可以获取jpeg的宽度和高度（以像素为单位）
  uint16_t w = 0, h = 0;
  TJpgDec.getFsJpgSize(&w, &h, "/111.jpg", LittleFS); // 注释名称前面带有 "/"
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  // 绘制图像，左上角 0,0
  TJpgDec.drawFsJpg(0, 0, "/111.jpg", LittleFS);

  // 渲染花费了多少时间 (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8位并行 105ms
  t = millis() - t;
  Serial.print(t); Serial.println(" ms");

  ESP.deepSleep(0); //WAKE_RF_DEFAULT  WAKE_RFCAL  WAKE_NO_RFCAL  WAKE_RF_DISABLED RF_DISABLED

}

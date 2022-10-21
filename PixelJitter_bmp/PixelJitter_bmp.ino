// 除了U8g2_for_Adafruit_GFX需要用我提供的库
// 其他库均可在库管理器下载

#include <GxEPD2_BW.h>
//#include <GxEPD2_3C.h>
//#include <GxEPD2_7C.h>
#include <U8g2_for_Adafruit_GFX.h>

//墨水屏端口定义
#define dc 0
#define cs 15
SPISettings spi_settings(10000000, MSBFIRST, SPI_MODE0);

//选择屏幕类型
GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));                         // GDEM029A01
//GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display(GxEPD2_290_T94(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEM029T94
//GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));    // GDEM029T94, Waveshare 2.9" V2 variant
//GxEPD2_BW<GxEPD2_290_I6FD, GxEPD2_290_I6FD::HEIGHT> display(GxEPD2_290_I6FD(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5)); // GDEW029I6FD
//GxEPD2_BW<GxEPD2_290_T5, GxEPD2_290_T5::HEIGHT> display(GxEPD2_290_T5(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));    // GDEW029T5
//GxEPD2_BW<GxEPD2_290_T5D, GxEPD2_290_T5D::HEIGHT> display(GxEPD2_290_T5D(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));   // GDEW029T5D
//GxEPD2_BW<GxEPD2_290_M06, GxEPD2_290_M06::HEIGHT> display(GxEPD2_290_M06(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW029M06

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
#define baise  GxEPD_WHITE  //白色
#define heise  GxEPD_BLACK  //黑色
//声明gb2312.c
#include "gb2312.c"
//声明外部字体常量
extern const uint8_t chinese_gb2312[254049] U8G2_FONT_SECTION("chinese_gb2312");
//图标声明
const unsigned char Bitmap_jc[] PROGMEM = { 0x00, 0xcf, 0xff, 0x14, 0x4e, 0x00, 0x2f, 0x00, 0x7e, 0xf3, 0xc3, 0x63, 0x89, 0x21, 0xe7, 0x7e, 0x42, 0xe8, 0x5c, 0x06, 0x56, 0x25, 0xf7, 0x42, 0x42, 0xe8, 0x5c, 0x06, 0x56, 0x25, 0xf7, 0x42, 0x42, 0xc3, 0x7c, 0x79, 0xce, 0x25, 0xcb, 0x42, 0x42, 0xe0, 0xfb, 0x00, 0x08, 0x38, 0x0f, 0x42, 0x7e, 0xf8, 0x42, 0x13, 0xce, 0xe4, 0x0f, 0x7e, 0x00, 0xd4, 0xa5, 0x6a, 0x56, 0xa5, 0x2b, 0x00, 0xff, 0xf8, 0x1e, 0x1b, 0xd0, 0x46, 0x0b, 0xff, 0xff, 0xf8, 0x1e, 0x1b, 0xd0, 0x46, 0x0b, 0xff, 0xc8, 0xc7, 0xd9, 0x70, 0x17, 0x1e, 0x34, 0xee, 0xf1, 0x14, 0x3c, 0x85, 0x89, 0x99, 0x17, 0x3c, 0x86, 0xc4, 0xe0, 0x1b, 0xc9, 0xc4, 0xf7, 0xe0, 0xc3, 0xdf, 0xf9, 0x7a, 0x46, 0xc6, 0x07, 0xdc, 0x48, 0xd0, 0xc4, 0x08, 0x31, 0x7e, 0xdc, 0x0d, 0x48, 0xd0, 0xc4, 0x08, 0x31, 0x7e, 0xdc, 0x0d, 0x87, 0xec, 0xbf, 0xfc, 0x56, 0x9f, 0xd8, 0x70, 0x8a, 0xe7, 0x9d, 0x6b, 0xe6, 0x63, 0xf7, 0x7c, 0x7b, 0x00, 0x9a, 0x1a, 0x39, 0x06, 0x07, 0x8f, 0x08, 0xf8, 0x7c, 0xf9, 0xb7, 0x79, 0xc7, 0x1c, 0x08, 0xf8, 0x7c, 0xf9, 0xb7, 0x79, 0xc7, 0x1c, 0x4f, 0xe7, 0x05, 0xf0, 0x36, 0x24, 0xf3, 0xc0, 0x82, 0xcb, 0xdb, 0x70, 0x60, 0xc6, 0x18, 0x9c, 0xc5, 0x04, 0xd8, 0x8b, 0x86, 0x1e, 0x2c, 0x9f, 0x82, 0x1f, 0xe5, 0x7d, 0xe8, 0xe6, 0x2c, 0x9e, 0x3b, 0xd0, 0x63, 0x7f, 0xe0, 0xb8, 0xff, 0xfc, 0x3b, 0xd0, 0x63, 0x7f, 0xe0, 0xb8, 0xff, 0xfc, 0xb0, 0x0b, 0x83, 0x79, 0x8e, 0x03, 0x3b, 0xa3, 0xb3, 0x2f, 0x20, 0x0e, 0x40, 0x9e, 0x1c, 0x01, 0x78, 0x1c, 0xc2, 0x88, 0x18, 0x7a, 0xc8, 0x13, 0x43, 0xcf, 0x5d, 0x6b, 0xd6, 0x87, 0x2b, 0xdc, 0x8a, 0xdb, 0x21, 0xea, 0x50, 0x7e, 0xf3, 0x4d, 0x8a, 0xdb, 0x21, 0xea, 0x50, 0x7e, 0xf3, 0x4d, 0x3b, 0xc8, 0xdd, 0x73, 0xc7, 0xba, 0x33, 0xc2, 0x38, 0x13, 0x61, 0x18, 0x1f, 0xfa, 0x30, 0x1d, 0x73, 0x18, 0xdf, 0x8e, 0x56, 0xbf, 0x0f, 0xe2, 0xf8, 0x0f, 0x63, 0x7a, 0x51, 0x07, 0x00, 0x73, 0xcd, 0xf7, 0xa3, 0x74, 0x77, 0x02, 0x08, 0x1d, 0xcd, 0xf7, 0xa3, 0x74, 0x77, 0x02, 0x08, 0x1d, 0x7c, 0x08, 0x5b, 0x0d, 0x9e, 0x98, 0x3b, 0xec, 0x8b, 0x23, 0x60, 0x80, 0x3e, 0x1f, 0x1b, 0xfc, 0xf8, 0x34, 0x01, 0x16, 0x39, 0x62, 0x34, 0x60, 0x3f, 0x10, 0x19, 0x0f, 0x9e, 0x02, 0x28, 0x4f, 0x48, 0x0c, 0x99, 0x6c, 0x77, 0x06, 0x1f, 0xbc, 0x48, 0x0c, 0x99, 0x6c, 0x77, 0x06, 0x1f, 0xbc, 0x43, 0xc0, 0xf8, 0x14, 0x06, 0xbe, 0xf3, 0xfc, 0x80, 0xd0, 0x98, 0x01, 0xb1, 0x06, 0xe8, 0x4c, 0x4d, 0xc7, 0xfe, 0x77, 0xf7, 0xc4, 0x28, 0x7d, 0x0a, 0x3c, 0x9b, 0x90, 0x2f, 0x43, 0x1c, 0x7f, 0x0a, 0x3c, 0x9b, 0x90, 0x2f, 0x43, 0x1c, 0x7f, 0xff, 0x00, 0x39, 0x1e, 0x7f, 0xd9, 0x38, 0xcc, 0x30, 0x13, 0x87, 0xeb, 0xee, 0x80, 0xd4, 0x3c, 0x8f, 0xc0, 0x18, 0xfd, 0xb9, 0x43, 0x2b, 0x2f, 0xf6, 0xe7, 0x02, 0x98, 0x09, 0x03, 0xc8, 0x1c, 0xff, 0xc3, 0x7a, 0xe3, 0xc1, 0x9c, 0x33, 0xd0, 0xff, 0xc3, 0x7a, 0xe3, 0xc1, 0x9c, 0x33, 0xd0, 0x00, 0xe8, 0xf9, 0x9a, 0x57, 0x7e, 0x1b, 0x4c, 0x7e, 0xd8, 0x20, 0x73, 0xc7, 0xa7, 0x13, 0xdd, 0x42, 0xdb, 0x85, 0x60, 0x10, 0xc3, 0xd8, 0x0c, 0x42, 0xe0, 0xa3, 0x8f, 0xf0, 0xf8, 0xdb, 0x90, 0x42, 0xd4, 0x5f, 0x0a, 0x47, 0xb9, 0xfc, 0x70, 0x42, 0xd4, 0x5f, 0x0a, 0x47, 0xb9, 0xfc, 0x70, 0x7e, 0xf3, 0xbf, 0x9a, 0x66, 0xba, 0x30, 0x1d, 0x00, 0xec, 0x79, 0x97, 0xb1, 0x7b, 0x03, 0xfc };
//#include <ESP8266WiFi.h>
#include <SPI.h>
#include <LittleFS.h> // 官方要求的新文件系统库  #include "FS.h"未来将不会得官方支持，已弃用
uint8_t xxx = 0;
void setup()
{

  Serial.begin(74880);

  //屏幕初始化
  //display.init();
  //display.init(0, 1, 10, 1); // 串口使能 初始化完全刷新使能 复位时间 ret上拉使能
  display.init(0, 0, 10, 1);   // 串口使能 初始化完全刷新使能 复位时间 ret上拉使能
  xiaobian();                  // 消除黑边

  display.setRotation(3);//屏幕旋转方向0:90度（1） 1:270度（3） 02竖屏 13横屏

  u8g2Fonts.begin(display);                // 将u8g2过程连接到Adafruit GFX
  u8g2Fonts.setFontMode(1);                // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);           // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(heise);     // 设置前景色
  u8g2Fonts.setBackgroundColor(baise);     // 设置背景色
  display.setTextColor(GxEPD_BLACK);       // 设置字体颜色
  u8g2Fonts.setFont(chinese_gb2312);       // 加载字体

  display.init(0, 0, 10, 1);
  display.setFullWindow();
  display.firstPage();
  display.nextPage();

  bool fsOK;
  fsOK = LittleFS.begin();
  if (!fsOK)
  {
    display_partialLine(0, "LittleFS 启动失败");
    display_partialLine(1, "扫码查看解决教程");

    display.firstPage();
    do
    {
      display.drawInvertedBitmap((display.width() / 2) - (64 / 2), (display.height() / 2) - (64 / 2), Bitmap_jc, 64, 64, heise); //显示图片
    }
    while (display.nextPage());

    Serial.println("LittleFS 未能成功启动，点击链接查看教程");
    Serial.println("https://gitee.com/Lichengjiez/weather-ink-screen/wikis/%E6%8F%90%E7%A4%BAlittle%20fs%E5%90%AF%E5%8A%A8%E5%A4%B1%E8%B4%A5%E7%9A%84%E8%A7%A3%E5%86%B3%E6%96%B9%E6%B3%95");
    esp_sleep(0); //永久休眠
  }

}

void loop()
{
  /*display_partialLine(0, "抖动源码来至");
  display_partialLine(1, "oshwhub.com/ZERO--0/epaper_card");
  for (uint8_t i = 3; i > 0; i--)
  {
    display_partialLine(2, "即将开始抖动显示图片 " + String(i));
    delay(i * 300);
  }
  BW_refresh();*/
  //寻找并显示根目录下所有BMP图片
  bool bmpFileState = 0;
  Dir dir = LittleFS.openDir("/");   // 建立"目录"对象
  while (dir.next())          // dir.next()用于检查目录中是否还有"下一个文件"
  {
    ESP.wdtFeed();// 喂狗
    String name = dir.fileName();
    if (name.endsWith(".bmp"))
    {
      bmpFileState = 1;
      Serial.println(name);
      //文件位置，文件名称，坐标X，坐标Y，颜色，是否局部刷新，是否覆盖刷新
      drawBitmapFromSpiffs_Buffered(&LittleFS, name, 0, 0, 1, 1, 0);
      delay(2000);
      Serial.println("");
    }
  }
  if (bmpFileState == 0)
  {
    display.firstPage();
    do
    {
      display.setPartialWindow(0, 0, display.width(), display.height()); //设置局部刷新窗口
      display.drawInvertedBitmap((display.width() / 2) - (64 / 2), (display.height() / 2) - (64 / 2), Bitmap_jc, 64, 64, heise); //显示图片
    }
    while (display.nextPage());
    display_partialLine(0, "文件系统没有扫描到文件");
    display_partialLine(1, "扫码查看解决教程");
  }
  esp_sleep(0); //永久休眠
}

//系统休眠
void esp_sleep(uint32_t minutes)
{
  display.hibernate(); //关闭电源屏幕并进入深度睡眠 display.powerOff()为仅关闭电源
  ESP.deepSleep(minutes * 1000, WAKE_RF_DEFAULT); //WAKE_RF_DEFAULT  WAKE_RFCAL  WAKE_NO_RFCAL  WAKE_RF_DISABLED RF_DISABLED
}

void EPD_writeCommand(uint8_t c)
{
  SPI.beginTransaction(spi_settings);
  if (dc >= 0) digitalWrite(dc, LOW);  //dc
  if (cs >= 0) digitalWrite(cs, LOW);  //cs
  SPI.transfer(c);
  if (dc >= 0) digitalWrite(dc, HIGH);   //dc
  if (cs >= 0) digitalWrite(cs, HIGH);   //cs
  SPI.endTransaction();
}
void EPD_writeData(uint8_t d)
{
  SPI.beginTransaction(spi_settings);
  if (cs >= 0) digitalWrite(cs, LOW); //cs
  SPI.transfer(d);
  if (cs >= 0) digitalWrite(cs, HIGH); //cs
  SPI.endTransaction();
}
void xiaobian() //消除黑边（四周的边跟随屏幕刷新，仅全局刷新有效）
{
  EPD_writeCommand(0x3c);  // 边界波形控制寄存器
  EPD_writeData(0x33);     // 向里面写入数据
}

// Example for library:
// https://github.com/Bodmer/TJpg_Decoder

// This example if for processors with LittleFS capability (e.g. RP2040,
// ESP32, ESP8266). It renders a Jpeg file that is stored in a LittleFS
// file.

// The test image is in the sketch "data" folder (press Ctrl+K to see it).
// You must upload the image to LittleFS using the Arduino IDE Tools Data
// Upload menu option (you may need to install extra tools for that).

// Include the jpeg decoder library

// 拿去商用可以，但要注明出处，且优化后需开源出来，不要拿着别人的程序说是自己写的，脸都不要了！

// 本程序由****** 甘草酸不酸 ******提供
// 感谢库TJpg_Decoder的作者
// 感谢库http://elm-chan.org/fsw/tjpgd/00index.html
// 像素抖动参考https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
// 对墨水屏进行适配，使用像素抖动方式输出，自动缩放和居中输出

// TJpg_Decoder库需要修改配置文件 tjpgdcnf.h
/* JD_FORMAT      2
   JD_USE_SCALE   1
   JD_TBLCLIP     0
   JD_FASTDECODE  0 */

#include <TJpg_Decoder.h>
#include "SPI.h"
#include "LittleFS.h"
SPISettings spi_settings(10000000, MSBFIRST, SPI_MODE0);

uint16_t jpgWidth, jpgHeight; // 记录当前JPG的宽高
uint8_t (*bmp8)[16 + 1];      // 创建像素抖动缓存二维数组（先不定长度），JPG最大的输出区块16+1行缓存（缓存上一次的最后一行）
uint16_t blockCount_x = 0;    // x轴区块计数
boolean FirstLineJitterStatus = 1;  // 首行抖动状态 1-可以抖动 0-已抖动过
boolean getXYstate = 1;             // 获取绘制像素点XY像素初始坐标
int16_t x_p = 0;                    // 绘制像素点的x轴坐标
int16_t y_p = 0;                    // 绘制像素点的y轴坐标
int16_t x_start;                    // 绘制像素点的x轴坐标初始值记录
int16_t y_start;                    // 绘制像素点的y轴坐标初始值记录
uint8_t ScreenOrientation = 0;      // 屏幕方向

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

//GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));                             // GDEM029A01
//GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display(GxEPD2_290_T94(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));   // GDEM029T94
//GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(/*CS*/ 15, /*DC*/ 0, /*RST*/ 2, /*BUSY*/ 4));      // GDEM029T94, Waveshare 2.9" V2 variant
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

GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=D8*/ 15, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEP015OC1 200x200, IL3829, no longer available
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0154D67 200x200, SSD1681
//GxEPD2_BW<GxEPD2_154_M09, GxEPD2_154_M09::HEIGHT> display(GxEPD2_154_M09(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEW0154M09 200x200, JD79653A

void setup()
{
  Serial.begin(74880);
  Serial.println("\n\n 测试TJpg_Decoder库");
  Serial.println("\n\n 测试墨水屏显示，抖动显示");

  //屏幕初始化
  display.init(0, 0, 10, 1); // 串口使能 初始化完全刷新使能 复位时间 ret上拉使能
  u8g2Fonts.begin(display);             // 将u8g2过程连接到Adafruit GFX
  u8g2Fonts.setFontMode(1);             // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);        // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(heise);  // 设置前景色
  u8g2Fonts.setBackgroundColor(baise);  // 设置背景色
  display.setRotation(ScreenOrientation); // 设置方向
  u8g2Fonts.setFont(u8g2_font_t0_15_tf);
  // jpeg图像可以按1、2、4或8的比例缩放
  //TJpgDec.setJpgScale(1);
  //TJpgDec.setSwapBytes(0);//我们需要交换颜色字节（字节次序 ）

  //提供回调函数，jpg输出流会在回调函数里输出，图像处理也在这里
  TJpgDec.setCallback(epd_output);

  if (!LittleFS.begin())
  {
    clearScreen();//清空屏幕
    display_partialLine(3, "LittleFS failed!");
    Serial.println("LittleFS初始化失败！");
    ESP.deepSleep(0);
  }
  Serial.println("\r\n初始化已完成.");

}

void loop()
{
  File root = LittleFS.open("/", "r");
  while (File file = root.openNextFile())
  {
    String strname = file.name();
    strname = "/" + strname;
    Serial.println(file.name());
    // If it is not a directory and filename ends in .jpg then load it
    if (!file.isDirectory() && (strname.endsWith(".jpg") || strname.endsWith(".jpeg")))
    {
      //clearScreen();//清空屏幕
      uint32_t t = millis();
      loadFile(strname); //绘制
      t = millis() - t;
      Serial.print(t); Serial.println(" ms");
      Serial.println("");
      delay(2000);
    }
  }
  display.hibernate(); //彻底关闭屏幕电源
  Serial.print("\n休眠\n");
  ESP.deepSleep(0);
}

void loadFile(String name)
{
  //数值初始化
  FirstLineJitterStatus = 1;  // 第一行抖动状态
  getXYstate = 1;             // 获取绘制像素点XY像素初始坐标
  blockCount_x = 0;           // X轴区块计数
  jpgWidth = 0;
  jpgHeight = 0;

  //获取jpeg的宽度和高度（以像素为单位）
  TJpgDec.getFsJpgSize(&jpgWidth, &jpgHeight, name, LittleFS);
  Serial.print("jpgWidth = "); Serial.print(jpgWidth); Serial.print(", jpgHeight = "); Serial.println(jpgHeight);

  //设置屏幕方向
  display.setRotation(ScreenOrientation); // 用户方向
  //if (jpgWidth != jpgHeight) display.setRotation(jpgWidth > jpgHeight ? 3 : 0);

  //设置缩放 1-2-4-8
  uint16_t scale = 1;
  for (scale = 1; scale <= 8; scale <<= 1)
  {
    if (jpgWidth <= display.width() * scale && jpgHeight <= display.height() * scale)
    {
      if (scale > 1)
      {
        scale = scale >> 1; // 屏幕太小，缩得比屏幕小就看不清了，回到上一个缩放
      }
      break;
    }
  }
  if (scale > 8) scale = 8; //至多8倍缩放
  TJpgDec.setJpgScale(scale);

  Serial.print("scale:"); Serial.println(scale);

  // 重新计算缩放后的长宽
  jpgWidth = jpgWidth / scale;
  jpgHeight = jpgHeight / scale;

  //创建指定长度的二维数值，必须在drawFsJpg之前,getFsJpgSize和重新计算缩放后的长宽之后
  bmp8 = new uint8_t[jpgWidth][16 + 1];

  //自动居中
  int32_t x_center = (display.width() / 2) - (jpgWidth / 2);
  int32_t y_center = (display.height() / 2) - (jpgHeight / 2);
  Serial.print("x_center:"); Serial.println(x_center);
  Serial.print("y_center:"); Serial.println(y_center);

  display.init(0, 0, 10, 1);
  display.setFullWindow();
  display.firstPage();
  do
  {
    uint8_t error;
    error = TJpgDec.drawFsJpg(x_center, y_center, name, LittleFS); // 发送文件和坐标
    String str = "";
    if (error == 1)      str = "Interrupted by output function";                    // 被输出功能中断
    else if (error == 2) str = "Device error or wrong termination of input stream"; // 设备错误或输入流的错误终止
    else if (error == 3) str = "Insufficient memory pool for the image";            // 映像的内存池不足
    else if (error == 4) str = "Insufficient stream input buffer";                  // 流输入缓冲区不足
    else if (error == 5) str = "Parameter error";                                   // 参数错误
    else if (error == 6) str = "Data format error (may be broken data)";            // 数据格式错误（可能是损坏的数据）
    else if (error == 7) str = "Right format but not supported";                    // 格式正确但不受支持
    else if (error == 8) str = "Not supported JPEG standard";                       // 不支持JPEG标准
    if (error != 0)
    {
      display_partialLine(3, str);
      display_partialLine(5, name);
    }
    Serial.println("error:" + String(error) + " " + str);
    Serial.println("");
  } while (display.nextPage());

  display.powerOff(); //关闭屏幕电源

  delete[] bmp8;//释放内存
}

void clearScreen() //清空屏幕
{
  display.init(0, 0, 10, 1);
  display.setFullWindow();
  display.firstPage();
  display.nextPage();
}
uint16_t getCenterX(String z) //计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z.c_str());  //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);    //计算字符居中的X位置
  return x;
}
uint16_t getCenterX(const char* z) //计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z);        //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);  //计算字符居中的X位置
  return x;
}
uint16_t getCenterX(int32_t z) //计算字符居中的X位置
{
  String zf = String (z);
  uint16_t zf_width = u8g2Fonts.getUTF8Width(zf.c_str());  //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);    //计算字符居中的X位置
  return x;
}

/*
  display_partialLine()
  发送局部刷新的显示信息到屏幕,带居中
  line        行数（0-7）
  zf          字符内容
  lineRefresh 整行刷新 1-是 0-仅刷新字符长度的区域
*/
void display_partialLine(uint8_t line, String zf) //发送局部刷新的显示信息到屏幕,带居中
{
  display.setPartialWindow(0, line * 16, display.width(), 16);   // 整行刷新
  display.firstPage();
  do
  {
    display.fillScreen(baise);
    u8g2Fonts.setCursor(getCenterX(zf), line * 16 + 13);
    u8g2Fonts.print(zf);
  }
  while (display.nextPage());

  display.powerOff(); //关闭屏幕电源
  // display.hibernate();
}

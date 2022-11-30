//请注意，BMP位图是在屏幕物理方向的物理位置绘制的
#define input_buffer_pixels 10 // 可能会影响性能，数值越大越费动态内存
#define max_row_width 500       // 限制最大尺寸 只能为8的整数
#define max_palette_pixels 500  // 限制最大尺寸 只能为8的整数

//文件位置，是否在图片下方显示文字，文件名称，坐标X，坐标Y，颜色，是否局部刷新，是否覆盖刷新
void drawBitmapFromSpiffs_Buffered(FS *fs, String filename, int16_t x, int16_t y, bool with_color, bool partial_update, bool overwrite)
{
  uint8_t input_buffer[3 * input_buffer_pixels];        // 深度不超过24
  uint8_t output_row_mono_buffer[max_row_width / 8];    // 用于至少一行黑白比特的缓冲区
  uint8_t output_row_color_buffer[max_row_width / 8];   // 至少一行颜色位的缓冲区
  uint8_t mono_palette_buffer[max_palette_pixels / 8];  // 调色板缓冲区深度<=8黑白
  uint8_t color_palette_buffer[max_palette_pixels / 8]; // 调色板缓冲区深度<=8 c/w
  uint16_t rgb_palette_buffer[max_palette_pixels];      // 对于缓冲图形，调色板缓冲区的深度<=8，需要7色显示

  File file;          // 创建文件对象file
  bool valid = false; // 要处理的有效格式
  bool flip = true;   // 位图自下而上存储
  //uint32_t startTime = millis();
  //if ((x >= display.width()) || (y >= display.height())) return;
  file = fs->open(filename, "r");
  if (!file)
  {
    display_partialLine(7, filename + "文件不存在");
    esp_sleep(0); //永久休眠
  }
  // 解析BMP标头
  if (read16(file) == 0x4D42) // BMP签名
  {
    uint32_t fileSize = read32(file);     // 文件大小
    uint32_t creatorBytes = read32(file); // 创建者字节
    uint32_t imageOffset = read32(file);  // 图像数据的开始
    uint32_t headerSize = read32(file);   // 标题大小
    int32_t width  = read32(file);       // 图像宽度
    int32_t height = read32(file);       // 图像高度
    uint16_t planes = read16(file);  // 平面
    uint16_t depth = read16(file);   // 每像素位数
    uint32_t format = read32(file);  // 格式

    Serial.print("width0:"); Serial.println(width);
    Serial.print("height0:"); Serial.println(height);

    // 检测图片大小 设置方向
    if (width <= display.width() && height > display.height())
      display.setRotation(0);
    else display.setRotation(3);

    if (width > max_row_width)
    {
      display_partialLine(7, "错误：图片width过大，应小于等于300");
      esp_sleep(0); //永久休眠
    }
    else if (height > max_row_width)
    {
      display_partialLine(7, "错误：图片height过大，应小于等于300");
      esp_sleep(0); //永久休眠
    }

    // 数组指针的内存分配
    uint8_t (*bmp8)[6];
    bmp8 = new uint8_t[width][6];

    boolean ddxhFirst = 1;                   // 抖动循环的首次状态
    uint16_t yrow1 = 0;                      // Y轴移位
    uint16_t yrow_old = 0;                   // 绘制像素点时 初始Y轴存储
    //Serial.print("depth:"); Serial.println(depth);
    if (depth >= 32)
    {
      display_partialLine(7, "不支持32位深度的图片");
      esp_sleep(0); //永久休眠
    }
    if ((planes == 1) && ((format == 0) || (format == 3))) // 处理未压缩，565同样
    {
      // BMP行填充为4字节边界（如果需要）
      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
      if (height < 0)
      {
        height = -height;
        flip = false;
      }
      uint16_t w = width;
      uint16_t h = height;
      if ((x + w - 1) >= display.width())  w = display.width()  - x;
      if ((y + h - 1) >= display.height()) h = display.height() - y;
      if (w <= max_row_width) //直接绘图处理
      {
        valid = true;
        uint8_t bitmask = 0xFF;
        uint8_t bitshift = 8 - depth;
        uint16_t red, green, blue;
        bool whitish, colored;
        if (depth == 1) with_color = false;
        if (depth <= 8) //8位颜色及以下使用调色板,如不使用有些图会翻转颜色
        {
          Serial.print("depth:"); Serial.print(depth);
          if (depth < 8) bitmask >>= depth;
          //file.seek(54); //调色板始终 @ 54
          file.seek(imageOffset - (4 << depth)); // 54表示常规，diff表示颜色重要
          for (uint16_t pn = 0; pn < (1 << depth); pn++)
          {
            blue  = file.read();
            green = file.read();
            red   = file.read();
            file.read();
            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // 红色还是黄色？
            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
            color_palette_buffer[pn / 8] |= colored << pn % 8;
            rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
          }
        }
        display.init(0, 0, 10, 1);
        if (partial_update) display.setPartialWindow(x, y, w, h);
        else display.setFullWindow();

        display.firstPage();
        do
        {
          if (overwrite) display.fillScreen(GxEPD_WHITE);
          uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
          for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // 对于每条线
          {
            uint32_t in_remain = rowSize;
            uint32_t in_idx = 0;
            uint32_t in_bytes = 0;
            uint8_t in_byte = 0; // for depth <= 8
            uint8_t in_bits = 0; // for depth <= 8
            int16_t color = GxEPD_WHITE;
            file.seek(rowPosition);
            for (uint16_t col = 0; col < w; col++) // 对于每个像素 //width 修补 w
            {
              // 是时候读取更多像素数据了？
              if (in_idx >= in_bytes) // 好的，24位也完全匹配（大小是3的倍数）
              {
                in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
                in_remain -= in_bytes;
                in_idx = 0;
              }
              switch (depth) //深度 //gray = (0.114*Blue+0.587*Green+0.299*Red)
              {
                case 24:
                  blue = input_buffer[in_idx++];   // 蓝
                  green = input_buffer[in_idx++];  // 绿
                  red = input_buffer[in_idx++];    // 红
                  // whitish = 发白的
                  // whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80);
                  // colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                // 红色还是黄色？ colored = 有色的
                  // color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3); // color = 颜色
                  // color = 0.114 * float(blue) + 0.587 * float(green) + 0.299 * float(red); //灰度转换
                  color = (114 * blue + 587 * green + 299 * red + 500) / 1000; //灰度转换
                  break;
                case 16:
                  {
                    uint8_t lsb = input_buffer[in_idx++];
                    uint8_t msb = input_buffer[in_idx++];
                    if (format == 0) // 555
                    {
                      blue  = (lsb & 0x1F) << 3;
                      green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                      red   = (msb & 0x7C) << 1;
                      //color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                      color = (114 * blue + 587 * green + 299 * red + 500) / 1000; //灰度转换
                    }
                    else // 565
                    {
                      blue  = (lsb & 0x1F) << 3;
                      green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                      red   = (msb & 0xF8);
                      //color = (msb << 8) | lsb;
                      color = (114 * blue + 587 * green + 299 * red + 500) / 1000; //灰度转换
                    }
                    //whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                    //colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // 微红或微黄?
                  }
                  break;
                case 1:
                case 4:
                  {
                    if (0 == in_bits)
                    {
                      in_byte = input_buffer[in_idx++];
                      in_bits = 8;
                    }
                    uint16_t pn = (in_byte >> bitshift) & bitmask;
                    whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                    colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                    in_byte <<= depth;
                    in_bits -= depth;
                    color = rgb_palette_buffer[pn];
                  }
                  break;
                case 8:
                  color = input_buffer[in_idx++];
                  break;
              }

              uint16_t yrow = y + (flip ? h - row - 1 : row);
              //Serial.print("x + col:" + String(x + col)); Serial.println(" yrow:" + String(yrow));
              if (depth == 1) // 位深为1位，直接绘制
              {
                if (whitish) color = GxEPD_WHITE;
                //else if (colored && with_color) color = GxEPD_COLORED;
                else color = GxEPD_BLACK;
                display.drawPixel(x + col, yrow, color); // 原始
              }
              else // 位深为24，16，8位 使用像素抖动绘制
              {
                //亮度对比度调节
                /*float B = 0.1;
                  float C = -0.1;
                  float K = tan((45 + 44 * C) / 180 * PI);
                  color = (color - 127.5 * (1 - B)) * K + 127.5 * (1 + B);
                  if (color > 255) color = 255;
                  else if (color < 0) color = 0;*/
                //分段抖动，每3行抖动一次
                bmp8[x + col][yrow1] = color;
                if (x + col == (w - 1)) //X轴填满，换行 //width 修补 w
                {
                  yrow1++;       // Y轴进位
                  // 首次需要存入6行数据再抖动 ，中间每次在012后面存入3行
                  if (yrow1 == 6 || (flip == 1 && yrow == 0) || (flip == 0 && yrow == (height - 1)))
                  {
                    int err;
                    uint8_t y_max0 = 4; //首次抖动0-4行 其余抖动1-4行
                    //到最后时将剩余行都一起抖动
                    if (flip == 1 && yrow == 0)                 y_max0 = yrow1;
                    else if (flip == 0 && yrow == (height - 1)) y_max0 = yrow1;

                    //Serial.print("y_max0："); Serial.println(y_max0);
                    yrow1 = 2;   // Y轴进位回到第3行，012

                    for (uint16_t y = 0; y <= y_max0; y++) // height width
                    {
                      for (uint16_t x = 0; x < w; x++) //width 修补 w
                      {
                        if (ddxhFirst == 1 || y != 0) //第一次对01234行抖动处理 后面至抖动1234行
                        {
                          if (bmp8[x][y] > 127) {
                            err = bmp8[x][y] - 255;
                            bmp8[x][y] = 255;
                          } else {
                            err = bmp8[x][y] - 0;
                            bmp8[x][y] = 0;
                          }
                          if (x != w - 1)  bmp8[x + 1][y + 0] = colorThresholdLimit(bmp8[x + 1][y + 0] , (err * 7) / 16);
                          if (x != 0)          bmp8[x - 1][y + 1] = colorThresholdLimit(bmp8[x - 1][y + 1] , (err * 3) / 16);
                          if (1)               bmp8[x + 0][y + 1] = colorThresholdLimit(bmp8[x + 0][y + 1] , (err * 5) / 16);
                          if (x != w - 1 ) bmp8[x + 1][y + 1] = colorThresholdLimit(bmp8[x + 1][y + 1] , (err * 1) / 16);
                        }
                      }
                      ddxhFirst = 0; //首行结束
                    }//像素抖动结束

                    // 绘制像素点 bmp[x][y] x轴绘制需全部完 y轴只绘制前5行
                    // bmp图片Y轴绘制初始位置
                    if (flip == 1 && yrow != 0)                 yrow_old = yrow + 5;
                    else if (flip == 0 && yrow != (height - 1)) yrow_old = yrow - 5;//bmp图片Y轴绘制初始位置
                    uint8_t y_max1 = 5; // 平时绘制5行
                    // 到最后时全部绘制完
                    if (flip == 1 && yrow == 0)                 y_max1 = yrow_old + 1;
                    else if (flip == 0 && yrow == (height - 1)) y_max1 = height - yrow_old;
                    //Serial.print("yrow:"); Serial.println(yrow);
                    for (uint16_t y = 0; y < y_max1; y++)
                    {
                      for (uint16_t x = 0; x < w; x++) //width 修补 w
                      {
                        /*Serial.print("x:" + String(x));
                          Serial.print(" y:" + String(y));
                          Serial.println(" bmp8:" + String(bmp8[x][y]));*/
                        /*if (yrow_old > 110) {
                          Serial.print("yrow_old:"); Serial.println(yrow_old);
                          }*/
                        display.drawPixel(x, yrow_old, bmp8[x][y]);
                      }
                      //Y轴进位
                      if (flip == 1 && yrow_old != 0)            yrow_old--;
                      else if (flip == 0 && yrow_old != (height - 1)) yrow_old++;
                    }
                    //bmp8 4、5行移到开头
                    for (uint16_t x = 0; x < w; x++) //width 修补 w
                    {
                      bmp8[x][0] = bmp8[x][4];
                      bmp8[x][1] = bmp8[x][5];
                    }
                  } //像素抖动6行数据处理结束
                }//像素抖动换行结束
              }
            }// end pixel
          }// end line
          delete[] bmp8; //释放内存
        } while (display.nextPage());
        display.powerOff(); //为仅关闭电源
        Serial.println("图像显示完毕");
      }
    }
  }
  file.close();
  if (!valid)
  {
    display_partialLine(7, filename + "图片格式不能处理");
    esp_sleep(0); //永久休眠
  }
}

uint16_t read16(File& f)
{
  // BMP数据存储在little endian中，与Arduino相同。
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB 最低有效位 最右侧
  ((uint8_t *)&result)[1] = f.read(); // MSB 最高有效位 最左侧
  return result;
}

uint32_t read32(File& f)
{
  // BMP数据存储在little endian中，与Arduino相同。
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

uint8_t colorThresholdLimit(uint8_t val1, int8_t val2) // 颜色阈值限制
{
  int16_t val1_int = val1;
  int16_t val2_int = val2;
  int16_t tmp = val1_int + val2_int;
  int16_t out = 0;
  //Serial.print("val1_int:" + String(val1_int)); Serial.print(" val2_int:" + String(val2_int)); Serial.println(" tmp:" + String(tmp));
  if (tmp > 255) return 255;
  else if (tmp < 0) return 0;
  else return tmp;
  return 0;
}

void display_partialLine(uint8_t line, String zf) //发送局部刷新的显示信息到屏幕,带居中
{
  //u8g2Fonts.setFont(chinese_gb2312);
  const char *character = zf.c_str();                            // String转换char
  uint16_t zf_width = u8g2Fonts.getUTF8Width(character);         // 获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);           // 计算字符居中的X坐标（屏幕宽度/2-字符宽度/2）
  display.setPartialWindow(0, line * 16, display.width(), 16);   // 整行刷新
  display.firstPage();
  do
  {
    u8g2Fonts.setCursor(x, line * 16 + 13);
    u8g2Fonts.print(character);
  }
  while (display.nextPage());
  display.powerOff(); //关闭屏幕电源
  //display.hibernate();
}
void BW_refresh() //黑白刷新一次
{
  display.init(0, 0, 10, 1);
  display.fillScreen(heise);  // 填充屏幕
  display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲
  //display.clearScreen(); //清屏
  display.fillScreen(baise);
  display.display(1);
}

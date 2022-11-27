bool epd_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t* bitmap)
{
  //Serial.print("x:"); Serial.println(x);
  //Serial.print("y:"); Serial.println(y);
  //Serial.print("w:"); Serial.println(w);
  //Serial.print("h:"); Serial.println(h);
  // Serial.println(" ");

  yield();
  // 绘制像素点的 x y从哪里开始
  if (getXYstate)
  {
    getXYstate = 0;
    x_start = x;
    y_start = y;
    x_p = x;
    y_p = y;
  }

  for (int16_t j = 0; j < h; j++, y++) //y轴
  {
    int16_t x1 = abs(x_start - x); //计算bmp8的x坐标
    int16_t y1 = j; //计算bmp8的y坐标
    if (FirstLineJitterStatus == 0) y1 += 1; //第一次之后从1行开始

    for (int16_t i = 0; i < w; i++, x1++) //x轴
    {
      uint32_t xh = j * w + i;
      uint8_t num = bitmap[xh];
      bmp8[x1][y1] = num;
    }
  }

  //y轴区块计数
  blockCount_x += w;
  //**** 区块已达到x轴边界，开始抖动和绘制图像
  if (blockCount_x >= jpgWidth)
  {
    blockCount_x = 0;

    //**** 抖动
    int err;
    uint8_t y_max; //抖动多少行，第一次0123456 之后12345678
    if (FirstLineJitterStatus) y_max = h - 2;  //首次0123456
    else                       y_max = h - 1;  //非首次01234567

    //到了最后一行吧剩余的行也一起抖动
    if (y == jpgHeight + y_start) y_max = h - 1;

    for (uint16_t y = 0; y <= y_max; y++) // height width
    {
      for (uint16_t x = 0; x < jpgWidth; x++)
      {
        if (bmp8[x][y] > 127) {
          err = bmp8[x][y] - 255;
          bmp8[x][y] = 255;
        } else {
          err = bmp8[x][y] - 0;
          bmp8[x][y] = 0;
        }
        if (x != jpgWidth - 1)  bmp8[x + 1][y + 0] = colorThresholdLimit(bmp8[x + 1][y + 0] , (err * 7) / 16);
        if (x != 0)             bmp8[x - 1][y + 1] = colorThresholdLimit(bmp8[x - 1][y + 1] , (err * 3) / 16);
        if (1)                  bmp8[x + 0][y + 1] = colorThresholdLimit(bmp8[x + 0][y + 1] , (err * 5) / 16);
        if (x != jpgWidth - 1)  bmp8[x + 1][y + 1] = colorThresholdLimit(bmp8[x + 1][y + 1] , (err * 1) / 16);
      }
    }//像素抖动结束

    uint16_t y_p_max; // 绘制多少行
    if (FirstLineJitterStatus) y_p_max = h - 2; // 首次只到6行 0123456
    else                       y_p_max = h - 1;  // 8-1=7  01234567

    //到了最后一行吧剩余的行也一起绘制
    if (y == jpgHeight + y_start)
    {
      y_p_max += (jpgHeight + y_start - 1) - (y_p + y_p_max); //127-（119+7）
    }

    for (uint16_t y1 = 0; y1 <= y_p_max; y1++, y_p++)
    {
      x_p = x_start; //回到初始位置
      for (uint16_t x1 = 0; x1 < jpgWidth; x1++, x_p++)
      {
        display.drawPixel(x_p, y_p, bmp8[x1][y1]);
      }
    }

    //倒数第1行移动到第1行
    if (FirstLineJitterStatus) //第一次
    {
      for (uint16_t x = 0; x < jpgWidth; x++)
        bmp8[x][0] = bmp8[x][h - 1];
    }
    else //第一次之后
    {
      for (uint16_t x = 0; x < jpgWidth; x++)
        bmp8[x][0] = bmp8[x][h];
    }
    FirstLineJitterStatus = 0; //第一次抖动结束
  }
  // 返回1以解码下一个块
  return 1;
}

//val1附近的像素，val2误差
uint8_t colorThresholdLimit(uint8_t val1, int8_t val2) // 颜色阈值限制
{
  int16_t val1_int = val1;
  int16_t val2_int = val2;
  int16_t tmp = val1_int + val2_int;
  if (tmp > 255) return 255;
  else if (tmp < 0) return 0;
  else return tmp;
  return 0;
}

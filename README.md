# EPD_PixelJitter_BMP_JPG
基于GXEPD2和TJpg_Decoder的像素抖动例程，可以处理BMP和jpg图片

显示中文需要使用我修改的U8g2_for_Adafruit_GFX库

BMP可以抖动24、16、8位深度的图，单色图直接显示，建议使用296*128像素之内的图片

JPG支持600K以下，太大解码慢导致看门狗超时，带抖动，自动缩放，居中裁剪

![](https://github.com/jie326513988/EPD_PixelJitter_BMP_JPG/blob/main/001.jpg) 

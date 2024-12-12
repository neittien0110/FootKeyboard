
#ifndef SSD1306_H
#define SSD1306_H


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "settings.h"              // Định nghĩa các PIN

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

/**
 * @brief Khởi tạo màn hình oled
 */
void setup_oled();

/**
 * @brief Màn hình chào chính, giới thiệu sản phẩm
 */
void main_screen(void);

/**
 * @brief  Hiển thị số thứ tự pedal được bấm
 * @param no  Số thứ tự pedal. Ví dụ 0, 1, 2, 3, 4
 */
void show_pedal(unsigned char no);

#endif
/**
 * @file configuration.h
 * @author Nguyen Duc Tien
 * @brief Configure and customize features via serial, or oled screen
 * @version 0.1
 * @date 2024-12-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <Arduino.h>
#include <settings.h>

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

    /// Kích thước bộ đệm đọc lệnh từ Serial (thực ra phải nhiều hơn MAX_KEY_CODE một vài kí tự xx=)
    #define SERIAL_BUFFER_SIZE MAX_KEY_CODE + 3
    // Lệnh điều khiển nhận được từ Serial, có dạng userformat
    extern char SerialCommand[SERIAL_BUFFER_SIZE];    

    /// Chuỗi các kí tự cần gửi, ứng với mỗi pedal. Cú pháp đơn kí tự, là bộ mã ASCII nhưng tận dụng mã ascii điều khiển để thành mã phím bấm điều khiển
    extern ASCII_FORMAT button_sendkeys[MAX_BUTTONS][MAX_KEY_CODE];      

    /**
     * @brief Phân tách chuỗi key=value
     * @param Command Chuỗi cần phân tích
     * @param key [out] Chuỗi key đã được trích. Zero-ending.
     * @param value [out] Chuỗi value đã được trích. Zero-ending.
     * @return true   Chuỗi phân tích thành công
     * @return false  Chuỗi sai cú pháp, không có kí tự =.
     */
    bool DetermineKeyValue(char * Command, char ** key, char ** value);

    /**
     * @brief Cấu hình chức năng các phím/pedal qua Serial
     * @param SerialCommand lệnh từ serial, có cấu trúc dạng <key>=<value>
     * @details Phân tích lệnh serial và ghi vào các biến hệ thống
     */
    void SerialConfiguration(char * SerialCommand);

#endif
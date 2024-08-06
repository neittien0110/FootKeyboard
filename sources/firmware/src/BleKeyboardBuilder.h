#pragma once

#include "BleKeyboard.h"

#ifndef ESP32_BLE_SENDKEYS_H
#define ESP32_BLE_KEYBOARD_H


/**
 * @brief Chuỗi kí tự theo format dành cho người dùng để gửi qua serial:  computer -------serial(USER_FORMAT)-----> device
 * @see  cú pháp <https://learn.microsoft.com/en-us/dotnet/api/system.windows.forms.sendkeys>
 * @see  ASCII_FORMAT
 * @example    a   / {ENTER}  /   ^F4    /  +EC
 */
typedef char USER_FORMAT; 


/**
 * @brief Chuỗi mã phím cần gửi trả về HID computer.    device(ASCII Format)-------HID connection(keycode)-----> computer
 * Là bộ mã ASCII nhưng tận dụng mã ascii điều khiển để thành mã phím bấm điều khiển, giúp tăng tốc xử lý, giảm trễ bluetooth.
 * @details Nguyên tắc
 *   - giữ nguyên mã ACSCII (nên không cần định nghĩa lại) của các kí tự thường trên bàn phím. 
 *   - giữ nguyên các mã kí tự được định nghĩa trong lib BleKeyboards như là  KEY_LEFT_ARROW
 *   - thêm 1 mã 0xFF để ám chỉ mã ngay phía sau là release. Ví dụ const char myCmd[] = {KEY_LEFT_SHIFT, 'c', 'H', 'a', ASCII_RELEASE_CODE, KEY_LEFT_SHIFT, 'o', 0}; SendKeys(myCmd); 
 * @note Bắt buộc zero-ending.
 * @see  USER_FORMAT
 * @see  ASCII_RELEASE_CODE
 */
typedef char ASCII_FORMAT; 

/**
 * @brief Ám chỉ mã ngay phía sau là release. 
 * @example  0x80 là phím left_control được bấm, còn 0xFF,0x80 là phím left_control bị nhả
 */
#define ASCII_RELEASE_CODE  0xFF

class BleKeyboardBuilder : public BleKeyboard {
private:
    // Độ trễ giữa 2 phím gửi đi liên tiếp. Đơn vị ms
    uint16_t keytokeytime;
public:
    BleKeyboardBuilder(std::string deviceName = "ESP32 Keyboard", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
    void SendKeys(const ASCII_FORMAT * cmd);
    static int ConvertFormat(const char * USER_FORMAT, char * ASCII_FORMAT);
    void SetKeyPerMinute( uint16_t kpm);  
    uint16_t GetKeyPerMinute( uint16_t kpm);
};

#endif // ESP32_BLE_KEYBOARD_H

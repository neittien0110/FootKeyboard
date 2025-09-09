#include "BleKeyboardBuilder.h"

#ifndef SETTINGS_H
#define SETTINGS_H

/// Nhiều thông tin debug
#define DEBUG_VERBOSE              

/// Debug lỗi cú pháp của bộ phân tích USER_FORMAT
//#define DEBUG_SERIAL_SYNTAX      

#if defined(ADRUINO_BOARD_ESP32_C3_SUPERMINI)
    /// Áp dụng khi MCU thuộc họ ESP32-C3
    #define LED_INDICATOR 8  //Led mặc định sẵn có trên ESP32-Super Mini. Tích cực mức thấp.
    #define LED_ON  HIGH   // Trạng thái bật đèn led, tích cực mức thấp
    #define LED_OFF LOW    // Trạng thái tắt đèn led
    #define BUTTON_BOOT 9  //Nút bấm mặc định sẵn có trên ESP32-Super Mini
    #define PIN_PEDAL00 1  // Kết nối tới pedal số 0
    #define PIN_PEDAL01 2  // Kết nối tới pedal số 1
    #define PIN_PEDAL02 10 // Kết nối tới pedal số 2
    #define PIN_PEDAL03 7  // Kết nối tới pedal số 3
    #define PIN_PEDAL04 3  // Đặt tạm, và cũng có sẵn header XH2.54 rồi
    #define PIN_VAR1 0     // Kết nối tới biến trở / cảm biến lực FSR
    #define PIN_VAR2 3     // Kết nối tới biến trở / cảm biến lực FSR    

    #define PIN_SDA     04 // Kết nối màn hình oled. Chân SDA mặc định
    #define PIN_SCL     05 // Kết nối màn hình oled. Chân SCL mặc định

#elif defined(ADRUINO_BOARD_ESP32_DEV_KIT)
    /// Áp dụng khi MCU thuộc họ ESP32
    #define LED_INDICATOR 2 // Led được thiết kế trên board. Không cần định nghĩa lại
    #define LED_ON  HIGH   // Trạng thái bật đèn led, tích cực mức cao
    #define LED_OFF LOW    // Trạng thái tắt đèn led
    #define BUTTON_BOOT 00 //Nút bấm 2 chức năng, thiết lập trạng thái nạp code và tính năng tùy ý
    #define PIN_PEDAL00 15 // Kết nối tới pedal số 15
    #define PIN_PEDAL01 16 // Kết nối tới pedal số 16
    #define PIN_PEDAL02 17 // Kết nối tới pedal số 17
    #define PIN_PEDAL03 18 // Kết nối tới pedal số 18
    #define PIN_PEDAL04 19 // Kết nối tới pedal số 19
    #define PIN_VAR1 04    // Kết nối tới biến trở 
    #define PIN_VAR2 04    // Chưa sử dụng

    #define PIN_SDA     21 // Kết nối màn hình oled
    #define PIN_SCL     22 // Kết nối màn hình oled

#elif defined(ARDUINO_ARCH_AVR)
    /// Áp dụng khi CPU thuộc họ AVR như Arduino Uno, Mega, Lilypad
#endif   

/**
 * @brief Trạng thái tích cực theo mức. Ví dụ 0, 1.
 * @example Ví dụ  digitalRead(3) == PEDAL_ACTIVE_LOGIC
 */
#define PEDAL_ACTIVE_LOGIC 0
#define PEDAL_DEACTIVE_LOGIC 1

/// Số lượng kí tự bàn phím tối đa của một phím pedal
#define MAX_KEY_CODE 40

/// Số lượng pedal/nút bấm tối đa
#define MAX_BUTTONS 5 

/// Tên mặc định của mạng BLE Bluetooth
#define DEFAULT_BLENAME  "Foot Keyboard"
/// Default setting của các phím pedal
#define DEFAULT_FEATURE_PEDAL_00 "{PGDN}"
#define DEFAULT_FEATURE_PEDAL_01 "{PGUP}"
#define DEFAULT_FEATURE_PEDAL_02 "cos{ENTER}"
#define DEFAULT_FEATURE_PEDAL_03 "vawsng{ENTER}"
//#define DEFAULT_FEATURE_PEDAL_03 "{CTRL}{F4}{~CTRL}"
//#define DEFAULT_FEATURE_PEDAL_03 "Hello"



/// Vùng không gian chứa các thông số cấu hình thiết bị BLE Bluetooth dạng vô hướng, như là thời gian trễ.. và tên
#define MAX_SCALAR_AREA 32

/// Chứa tên mạng BLE Bluetooth. Tối đã 15 kí tự + \0.
#define MAX_BLUETOOTH_NAME 16

// Tốc độ gõ kí tự mặc định trong 1 phút
#define DEFAULT_KEYS_PER_MINUTE 250

// define the number of bytes you want to access. 32 kí tự chứa thông tin phần BLE Name
#define EEPROM_SIZE (MAX_KEY_CODE * MAX_BUTTONS + MAX_SCALAR_AREA)

void ResetFactorySetting();

/**
 * @brief Đọc cấu hình chức năng đã lưu trữ trong Flash
 * @param blename [out] Tên mạng BLE Bluetooth
 * @param time_k2k [out] Tốc độ gõ bàn phím
 * @param area    [out] Địa chỉ Mảng 2 chiều chứa các phím kí tự của các nút bấm Pedal
 */
void GetSettings(char * blename, uint16_t * time_k2k, void * area);

/**
 * @brief Lưu cấu hình chức năng vào bộ nhớ Flash
 * @param blename [in] Tên mạng BLE Bluetooth
 * @param time_k2k [in] Tốc độ gõ bàn phím
 */
void SaveScalarSettings(const char * blename, uint16_t time_k2k);

/**
 * @brief Lưu cấu hình chức năng vào bộ nhớ Flash
 * @param key       Mã của phím chức năng. Ví dụ 0, 1, 2 ,3 tương ứng với các phím pedal 1, 2, 3, 4.
 * @param value     Chuỗi cấu hình
 */
void SaveKeyCodeSetting(int key, char * value);

#endif
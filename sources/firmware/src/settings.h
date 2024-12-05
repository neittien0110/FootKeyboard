#include "BleKeyboardBuilder.h"

#ifndef SETTINGS_H
#define SETTINGS_H

/// Nhiều thông tin debug
#define DEBUG_VERBOSE              

/// Debug lỗi cú pháp của bộ phân tích USER_FORMAT
//#define DEBUG_SERIAL_SYNTAX      


/// Số lượng kí tự bàn phím tối đa của một phím pedal
#define MAX_KEY_CODE 100

/// Số lượng pedal/nút bấm tối đa
#define MAX_BUTTONS 4 

/// Tên mặc định của mạng BLE Bluetooth
#define DEFAULT_BLENAME  "Foot Keyboard"
/// Default setting của các phím pedal
#define DEFAULT_FEATURE_PEDAL_00 "{PGDN}"
#define DEFAULT_FEATURE_PEDAL_01 "{PGUP}"
#define DEFAULT_FEATURE_PEDAL_02 "{ENTER}"
//#define DEFAULT_FEATURE_PEDAL_03 "{CTRL}{F4}{~CTRL}"
#define DEFAULT_FEATURE_PEDAL_03 "Hello"



/// Vùng không gian chứa các thông số cấu hình thiết bị BLE Bluetooth dạng vô hướng, như là thời gian trễ.. và tên
#define MAX_SCALAR_AREA 32

/// Chứa tên mạng BLE Bluetooth. Tối đã 15 kí tự + \0.
#define MAX_BLUETOOTH_NAME 16

// Tốc độ gõ kí tự mặc định trong 1 phút
#define DEFAULT_KEYS_PER_MINUTE 150

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
void SaveScalarSettings(char * blename, uint16_t time_k2k);

/**
 * @brief Lưu cấu hình chức năng vào bộ nhớ Flash
 * @param key       Mã của phím chức năng. Ví dụ 0, 1, 2 ,3 tương ứng với các phím pedal 1, 2, 3, 4.
 * @param value     Chuỗi cấu hình
 */
void SaveKeyCodeSetting(int key, char * value);

#endif
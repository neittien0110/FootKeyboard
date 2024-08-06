#include "settings.h"
#include <EEPROM.h>

/**
 * @brief Khôi phục cấu hình xuất xương
 * @details  Cấu hình xuát xưởng như sau:
 * - Tên mạng Bluetooth BLE: Foot Keyboard
 * - Chức năng Pedal 01: Page Down
 * - Chức năng Pedal 02: Page Up
 * - Chức năng Pedal 03: Enter
 * - Chức năng Pedal 04: Ctrl+F4
  */
void ResetFactorySetting()
{  
  ASCII_FORMAT codes[MAX_KEY_CODE];
  uint8_t len;
  EEPROM.begin(EEPROM_SIZE);
  // Tính địa chỉ dịch chuyển và lưu 
  EEPROM.writeBytes(0                              , DEFAUL_BLENAME         , strlen(DEFAUL_BLENAME)+1);

  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_00, codes) + 1;  // +1 để thêm kí tự 0
  EEPROM.writeBytes(0 * MAX_KEY_CODE + MAX_BLE_NAME, codes, len);     
  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_01, codes) + 1;
  EEPROM.writeBytes(1 * MAX_KEY_CODE + MAX_BLE_NAME, codes, len);
  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_02, codes) + 1;
  EEPROM.writeBytes(2 * MAX_KEY_CODE + MAX_BLE_NAME, codes, len);
  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_03, codes) + 1;
  EEPROM.writeBytes(3 * MAX_KEY_CODE + MAX_BLE_NAME, codes, len);
  
  EEPROM.end();
}

/**
 * @brief Đọc cấu hình chức năng đã lưu trữ trong Flash
 * @param blename [out] Tên mạng BLE Bluetooth
 * @param area    [out] Địa chỉ Mảng 2 chiều chứa các phím kí tự của các nút bấm Pedal
 */
void GetSettings(char * blename, void * area)
{  uint8_t i;
    
    EEPROM.begin(EEPROM_SIZE);

    // Đọc tên mạng BLE
    EEPROM.readString(0, blename, MAX_BLE_NAME);
    
    // Đọc chức năng của các phím, trong toàn khối  button_sendkeys
    EEPROM.readBytes(i * MAX_KEY_CODE + MAX_BLE_NAME, area , MAX_BUTTONS * MAX_KEY_CODE);

    EEPROM.end();
}

/**
 * @brief Lưu cấu hình chức năng vào bộ nhớ Flash
 * @param key       Mã của phím chức năng. Ví dụ 0, 1, 2 ,3 tương ứng với các phím pedal 1, 2, 3, 4.
 * @param value     Chuỗi cấu hình
 */
void SaveSettings(int key, char * value)
{
    EEPROM.begin(EEPROM_SIZE);
    // Tính địa chỉ dịch chuyển và lưu 
    //EEPROM.writeBytes(key * MAX_KEY_CODE + MAX_BLE_NAME, value, MAX_KEY_CODE);
    EEPROM.writeBytes(key * MAX_KEY_CODE + MAX_BLE_NAME, value, strlen(value)+1);//+1 để ghi thêm kí tự \0
    EEPROM.end();
}


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
  
  // Tốc độ gõ bàn phím 
  EEPROM.writeUShort(MAX_BLUETOOTH_NAME + sizeof(uint16_t), DEFAULT_KEYS_PER_MINUTE);

  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_00, codes) + 1;  // +1 để thêm kí tự 0
  EEPROM.writeBytes(0 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);     
  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_01, codes) + 1;
  EEPROM.writeBytes(1 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);
  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_02, codes) + 1;
  EEPROM.writeBytes(2 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);
  len = BleKeyboardBuilder::ConvertFormat(DEFAUL_FEATURE_PEDAL_03, codes) + 1;
  EEPROM.writeBytes(3 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);
  
  EEPROM.end();
}


void GetSettings(char * blename, uint16_t * time_k2k, void * area)
{  uint8_t i;
    
    EEPROM.begin(EEPROM_SIZE);

    // Đọc tên mạng BLE. Dộ dài không quá 15 kí tự.
    EEPROM.readString(0, blename, MAX_BLUETOOTH_NAME);

    // Độ trễ giữa 2 lần gửi phím kí tự HID. Đơn vị ms. 2 byte.
    *time_k2k = EEPROM.readUShort(MAX_BLUETOOTH_NAME + sizeof(uint16_t));

    // Đọc chức năng của các phím, trong toàn khối  button_sendkeys
    EEPROM.readBytes(i * MAX_KEY_CODE + MAX_SCALAR_AREA, area , MAX_BUTTONS * MAX_KEY_CODE);

    EEPROM.end();
}

void SaveKeyCodeSetting(int key, char * value)
{
    EEPROM.begin(EEPROM_SIZE);
    // Tính địa chỉ dịch chuyển và lưu 
    //EEPROM.writeBytes(key * MAX_KEY_CODE + MAX_SCALAR_AREA, value, MAX_KEY_CODE);
    EEPROM.writeBytes(key * MAX_KEY_CODE + MAX_SCALAR_AREA, value, strlen(value)+1);//+1 để ghi thêm kí tự \0
    EEPROM.end();
}

void SaveScalarSettings(char * blename, uint8_t time_k2k)
{
    EEPROM.begin(EEPROM_SIZE);
    // Đọc tên mạng BLE. Dộ dài không quá 15 kí tự.
    EEPROM.writeString(0, blename);

    // Tốc độ gõ bàn phím 
    EEPROM.writeUShort(MAX_BLUETOOTH_NAME + sizeof(uint16_t), time_k2k);

    EEPROM.end();
}


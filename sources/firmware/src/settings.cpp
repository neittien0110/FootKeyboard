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
 * 
 * Cấu trúc dữ liệu vùng nhớ EEPROM
 *
 *    | BLUETOOTH NAME  | 16 bytes   \
 *    | TIME_K2K        |   2 bytes   = Toàn bộ vùng này là MAX_SCALAR_AREA, 32 bytes
 *    | Reverse         |  14 bytes  / 
 *    | KeyCodes of 1st | 100 bytes = MAX_KEY_CODE
 *    | KeyCodes of 2nd | 100 bytes = MAX_KEY_CODE
 *    | KeyCodes of 3rd | 100 bytes = MAX_KEY_CODE
 *    | KeyCodes of 4th | 100 bytes = MAX_KEY_CODE
 */
void ResetFactorySetting()
{  
  ASCII_FORMAT codes[MAX_KEY_CODE];
  uint8_t len;

  EEPROM.begin(EEPROM_SIZE);
  // Tính địa chỉ dịch chuyển và lưu 
#ifdef DEBUG_VERBOSE
    Serial.print("Bluetooth name:");
    Serial.println(DEFAULT_BLENAME);
#endif  
  EEPROM.writeBytes(0                  , DEFAULT_BLENAME     , strlen(DEFAULT_BLENAME)+1);
  
  // Tốc độ gõ bàn phím 
#ifdef DEBUG_VERBOSE
    Serial.printf("Tre phim: %d \n", DEFAULT_KEYS_PER_MINUTE);
#endif      
  EEPROM.writeUShort(MAX_BLUETOOTH_NAME, DEFAULT_KEYS_PER_MINUTE);

  len = BleKeyboardBuilder::ConvertFormat(DEFAULT_FEATURE_PEDAL_00, codes) + 1;  // +1 để thêm kí tự 0
  EEPROM.writeBytes(0 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);     
  len = BleKeyboardBuilder::ConvertFormat(DEFAULT_FEATURE_PEDAL_01, codes) + 1;
  EEPROM.writeBytes(1 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);
  len = BleKeyboardBuilder::ConvertFormat(DEFAULT_FEATURE_PEDAL_02, codes) + 1;
  EEPROM.writeBytes(2 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);
  len = BleKeyboardBuilder::ConvertFormat(DEFAULT_FEATURE_PEDAL_03, codes) + 1;
  EEPROM.writeBytes(3 * MAX_KEY_CODE + MAX_SCALAR_AREA, codes, len);
#ifdef DEBUG_VERBOSE
    Serial.println((char *)codes);
#endif    

// Ghi vào eeprom  
  EEPROM.commit();
  EEPROM.end();
}


/**
 * @brief Đọc thông tin cấu hình lưu trữ trong bộ nhớ vĩnh viễn 
 * @param blename  Tên mạng Bluetooth
 * @param time_k2k Thời gian trễ giữa 2 lần gửi phím 
 * @param area     Mảng 2D chứa các phím gõ tương tứng với từng nút bấm.
 */   
void GetSettings(char * blename, uint16_t * time_k2k, void * area)
{  uint8_t i;
    
    
    EEPROM.begin(EEPROM_SIZE);

    // Đọc tên mạng BLE. Dộ dài không quá 15 kí tự. Ở vị trí đầu tiên
    EEPROM.readString(0, blename, MAX_BLUETOOTH_NAME);
#ifdef DEBUG_VERBOSE
    Serial.print("Bluetooth name:");
    Serial.println(blename);
#endif

    // Độ trễ giữa 2 lần gửi phím kí tự HID. Đơn vị ms. 2 byte. Vị trí ngay sau BLUETOOTH_NAME
    *time_k2k = EEPROM.readUShort(MAX_BLUETOOTH_NAME);
#ifdef DEBUG_VERBOSE
    Serial.printf("Tre phim: %d \n", *time_k2k);
#endif    

    // Đọc chức năng của các phím, trong toàn khối  button_sendkeys
    EEPROM.readBytes(MAX_SCALAR_AREA, area , MAX_BUTTONS * MAX_KEY_CODE);
#ifdef DEBUG_VERBOSE
    Serial.println((char *)(&area + (MAX_BUTTONS-1)*MAX_KEY_CODE));
#endif    

#ifdef DEBUG_VERBOSE
    Serial.println("Doc xong");
#endif 
    EEPROM.end();

#ifdef DEBUG_VERBOSE
    Serial.println("Dong EEPROM");
#endif 
}

void SaveKeyCodeSetting(int key, char * value)
{
    EEPROM.begin(EEPROM_SIZE);
    // Tính địa chỉ dịch chuyển và lưu 
    //EEPROM.writeBytes(key * MAX_KEY_CODE + MAX_SCALAR_AREA, value, MAX_KEY_CODE);
    EEPROM.writeBytes(key * MAX_KEY_CODE + MAX_SCALAR_AREA, value, strlen(value)+1);//+1 để ghi thêm kí tự \0
    EEPROM.end();
}

/**
 * @brief Lưu thông tin cấu hình vào EEPROM
 * @param blename  Tên mạng Bluetooth
 * @param time_k2k Thời gian trễ giữa 2 lần gửi phím 
 * @see  Xem Cấu trúc dữ liệu vùng EEPROM
 */
void SaveScalarSettings(char * blename, uint16_t time_k2k)
{
    EEPROM.begin(EEPROM_SIZE);
    // Đọc tên mạng BLE. Dộ dài không quá 15 kí tự. Điểm bắt đầu 0.
    EEPROM.writeString(0, blename);

    // Tốc độ gõ bàn phím. Điểm bắt đầu ngay sau phần thông tin Bluetooth name
    EEPROM.writeInt(MAX_BLUETOOTH_NAME, time_k2k);

    EEPROM.end();
}


#include "BleKeyboardBuilder.h"

#ifndef SETTINGS_H
#define SETTINGS_H

/// Số lượng kí tự bàn phím tối đa của một phím pedal
#define MAX_KEY_CODE 100

/// Số lượng pedal/nút bấm tối đa
#define MAX_BUTTONS 4 

/// Tên mặc định của mạng BLE Bluetooth
#define DEFAUL_BLENAME  "Foot Keyboard"
/// Default setting của các phím pedal
#define DEFAUL_FEATURE_PEDAL_00 "{PGDN}"
#define DEFAUL_FEATURE_PEDAL_01 "{PGUP}"
#define DEFAUL_FEATURE_PEDAL_02 "{ENTER}"
#define DEFAUL_FEATURE_PEDAL_03 "{CTRL}{F4}{~CTRL}"


/// Chứa tên mạng BLE Bluetooth
#define MAX_BLE_NAME 32

// define the number of bytes you want to access. 32 kí tự chứa thông tin phần BLE Name
#define EEPROM_SIZE (MAX_KEY_CODE * MAX_BUTTONS + MAX_BLE_NAME )

void ResetFactorySetting();
void GetSettings(char * blename, void * button_sendkeys);
void SaveSettings(int key, char * value);

#endif
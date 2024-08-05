#ifndef SETTINGS_H
#define SETTINGS_H

/// Số lượng kí tự bàn phím tối đa của một phím pedal
#define MAX_KEY_CODE 255

/// Số lượng pedal/nút bấm tối đa
#define MAX_BUTTONS 4 

// define the number of bytes you want to access. 20 kí tự chứa thông tin phần BLE Name
#define EEPROM_SIZE (MAX_KEY_CODE * MAX_BUTTONS) + 20

void ResetFactorySetting();
void GetSettings();
void SaveSettings(int key, char * value);

#endif
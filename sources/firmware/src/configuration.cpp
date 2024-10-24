#include <Arduino.h>
#include <settings.h>
#include <configuration.h>



const char TIME_KEY_TO_KEY[] = "TK";   // ma cấu hình tương ứng với khoảng thời gian trễ giữa các phím kí tự gửi đi
const char PRINT_ALL[] = "A?";   // mã xem toàn bộ thông tin cáu hình
const char KEY00[] = "00";   // mã cấu hình tương ứng với chân PIN_PEDAL01
const char KEY01[] = "01";   // mã cấu hình tương ứng với chân PIN_PEDAL02
const char KEY02[] = "02";   // mã cấu hình tương ứng với chân PIN_PEDAL03
const char KEY03[] = "03";   // mã cấu hình tương ứng với chân PIN_PEDAL04


// Phân tách mỗi dòng lệnh điều khiển thành 2 vùng. key=value
USER_FORMAT * cmdvalue = NULL;    

// Lệnh điều khiển nhận được từ Serial, có dạng userformat
char SerialCommand[SERIAL_BUFFER_SIZE];    

/// Chuỗi các kí tự cần gửi, ứng với mỗi pedal. Cú pháp đơn 
ASCII_FORMAT button_sendkeys[MAX_BUTTONS][MAX_KEY_CODE];

// Điều khiển gửi kí tự
extern BleKeyboardBuilder bleKeyboardBuilder;

/**
 * @brief Phân tách chuỗi key=value
 * @param Command Chuỗi cần phân tích
 * @param key [out] Chuỗi key đã được trích. Zero-ending.
 * @param value [out] Chuỗi value đã được trích. Zero-ending.
 * @return true   Chuỗi phân tích thành công
 * @return false  Chuỗi sai cú pháp, không có kí tự =.
 */
bool DetermineKeyValue(char * Command, char ** key, char ** value) {
    char ch;
    uint8_t i;
    *key = Command;
    *value = NULL;
    
    /// Phân tích parsing
    i = 0;
    do {
        ch = Command[i];
        if (ch == '=') {
            Command[i]=0; // Đánh dấu Zero kết thúc chuỗi
            *value = & Command[i+1];
            break;
        }
        i++;
    } while (ch != 0);
    
    // Nếu không tìm thấy kí tự = không hợp lệ.
    if (ch == 0) {
        Serial.println("Error 01: delimiter = not found.");
        return false;
    };
    return true;
}

/**
 * @brief Cấu hình chức năng các phím/pedal qua Serial
 * @param SerialCommand lệnh từ serial, có cấu trúc dạng <key>=<value>
 * @details Phân tích lệnh serial và ghi vào các biến hệ thống
 */
void SerialConfiguration(char * SerialCommand)
{
    /// Phân tách mỗi dòng lệnh điều khiển thành 2 vùng. key=value.
    /// Key ở điểm bắt đầu của commandline, nên luôn trùng với điểm đầu của lệnh
    char * cmdkey = SerialCommand;

    uint8_t i, j;
    int16_t res;
    unsigned char ch;
   
    // Phân tích thành cặp key=value. Không phù hợp thì dừng luôn
    if (!DetermineKeyValue(SerialCommand, & cmdkey, & cmdvalue)) return;

#pragma region ASSIGN_COMMAND
    /// Gán lệnh cho các nút bấm/pedal tương ứng
    if (strcasecmp(cmdkey, KEY00) == 0) {
        i = 0;
    } else if (strcasecmp(cmdkey, KEY01) == 0) {
        i = 1;
    } else if (strcasecmp(cmdkey, KEY02) == 0) {
        i = 2;
    } else if (strcasecmp(cmdkey, KEY03) == 0) {
        i = 3;
    } else if (strcasecmp(cmdkey, TIME_KEY_TO_KEY) == 0) {
        // Thiết lập tốc độ gửi phím
        res = atoi(cmdvalue);
        if (res == 0) {
            Serial.println("Error: toc do do phim phai la so nguyen duong.");    
        }
        bleKeyboardBuilder.SetKeyPerMinute(res);
        SaveScalarSettings(DEFAUL_BLENAME ,res);
        Serial.print("Info: thiet lap toc do go phim ");
        Serial.print(res);
        Serial.println(" key/phut.");
        Serial.print("Info: OK");
        return;
    } else if (strcasecmp(cmdkey, PRINT_ALL) == 0) { 
        /// Hiện ra các thông số cấu hình
        ///  1. Hiển thị tốc độ gửi phím
        res = bleKeyboardBuilder.GetKeyPerMinute();
        Serial.print("Info: toc do go phim ");
        Serial.print(res);
        Serial.println(" key/phut.");
        ///  2. Hiển thị thông tin phím
        for( i = 0; i < MAX_BUTTONS; i++ )
        {
            Serial.print("Info: Button ");
            Serial.print(i);
            Serial.print(": ");
            res = BleKeyboardBuilder::RevertFormat(button_sendkeys[i], SerialCommand);
            Serial.println(SerialCommand);
            Serial.print("     length=");
            Serial.println(res);
        }
        Serial.print("Info: OK");
        return;
    }else {
        // mã phím bấm không phù hợp, kết thúc luôn
        Serial.println("Error 02: button key is invalid.");
        return;
    }
    // Debug
    Serial.print("Info: Button="); Serial.print(i);
    Serial.print(", UserFormat="); Serial.println(cmdvalue);

    // Chuyển đổi từ USER_FORMAT về thành ASCII_FORMAT. Tận dụng lại mảng SerialComamnd để tiết kiệm bộ nhớ, vì cmdValue chắc chắn chứa các cụm từ mô tả dài hơn.
    res = BleKeyboardBuilder::ConvertFormat(cmdvalue, SerialCommand);
    if (res < 0) {
        Serial.println("Error: UserFormat wrong at the character "); Serial.print(-res);
        return;
    } else {
        Serial.print("cmd ");
        Serial.print(res);
        Serial.print(" chars: ");
        Serial.println(SerialCommand);
    }
    // Áp dụng thành các phím kí tự của phím pedal
    for (j = 0; j <= res ; j++) {   // <=res để copy cả kí tự 0
        button_sendkeys[i][j] = SerialCommand[j];    
    }
    Serial.print("Infor: ap dung ");
    Serial.println(button_sendkeys[i]);    
    // Debug
    Serial.print("Info: OK");
    // Lưu cấu hình
    SaveKeyCodeSetting(i , button_sendkeys[i]);
#pragma endregion ASSIGN_COMMAND
}

#include <Arduino.h>
#include "BleKeyboardBuilder.h"
#include <cstdint>

BleKeyboardBuilder::BleKeyboardBuilder(std::string deviceName , std::string deviceManufacturer , uint8_t batteryLevel ) :
    BleKeyboard(deviceName , deviceManufacturer , batteryLevel )
{
    SetKeyPerMinute(1000); //nghỉ 60ms giữa 2 lần gửi mã phím, để tránh tràn bộ đệm bàn phím
};

/**
 * @brief Gửi kí tự ra giao tiếp HID 
 * @param ASCII_FORMAT cmd Chuôi kí tự cần gửi theo cú pháp ASCII_FORMAT. Bắt buộc zero-ending.
 * @example     const char myCmd[] = {KEY_F4, 0};  SendKeys(myCmd);   
 *              const char myCmd[] = {KEY_LEFT_SHIFT, 'c', 'H', 'a', ASCII_RELEASE_CODE, KEY_LEFT_SHIFT, 'o',' ', 'T', 0}; SendKeys(myCmd); 
  */
void BleKeyboardBuilder::SendKeys(const ASCII_FORMAT * cmd) {
    static unsigned char ch;
    static uint8_t i;

    i=0;
    while (true){
        ch = cmd[i];
        // đợi giữa 2 kí tự liên tiếp, tránh bị tràn bộ đêm bàn phím
        delay(keytokeytime);

        if (ch == 0) { // Nếu là kí tự kết thúc chuỗi thì nhả tất cả các phím và dừng lại
            releaseAll();    
            return;
        } else if (ch < 0x80) { // Nếu là kí tự thường thì hiện thị
            press(ch);      
            delay(keytokeytime);    // Phải có khoảng trễ nhất định giữa press và release, nếu không sẽ bị mất phím        
            release(ch); //Phải có release. Nếu không có thì mặc dù biểu hiện không khác biệt, nhưng sẽ chỉ được 6 kí tự.
        } else { 
            // Nếu là kí tự đặc biệt chính là vai trò của mã ASCII_FORMAT đây
            if (ch != ASCII_RELEASE_CODE) {
                press(ch);
            } else {
                i++;
                ch = cmd[i];
                release(ch);
            }
        }
        i++; // Chuyển sang kí tự tiếp theo
    };
}

/**
 * @brief Tốc độ gõ phím. Key per minute. 
 * @detail Nếu số quá nhỏ, và lượng kí tự gửi đi khi bấm phím pedal quá lớn, trên 16 kí tự, có thể sẽ gây tràn bộ đệm bàn phím, gửi thiếu phím
 * 
 */
void BleKeyboardBuilder::SetKeyPerMinute(uint16_t kpm)
{
    keytokeytime = 60000 / kpm;  // = 60s / key per minute;
}

/**
  * @brief Tốc độ gõ phím. Key per minute. Mặc định là 200 kpm
 */  
uint16_t BleKeyboardBuilder::GetKeyPerMinute(uint16_t kpm)
{
    return 60000/keytokeytime;
}

typedef struct {
    uint16_t checksum;
    uint8_t code;
} CheckSUM2CODE;

/** Bảng qui đổi từ checksum của USER_FORMAT ==> KeyCode */
CheckSUM2CODE checksum2codes[] = { 
    {631, KEY_LEFT_CTRL},          // {~CTRL}
    {498, KEY_LEFT_ALT},          // {~ALT}
    {793, KEY_LEFT_SHIFT},          // {~SHIFT}
    {784, KEY_RIGHT_CTRL},          // {~RCTRL}
    {649, KEY_RIGHT_ALT},          // {~RALT}
    {947, KEY_RIGHT_SHIFT},          // {~RSHIFT}
    {477, KEY_LEFT_CTRL},          // {CTRL}
    {344, KEY_LEFT_ALT},          // {ALT}
    {637, KEY_LEFT_SHIFT},          // {SHIFT}
    {628, KEY_RIGHT_CTRL},          // {RCTRL}
    {492, KEY_RIGHT_ALT},          // {RALT}
    {791, KEY_RIGHT_SHIFT},          // {RSHIFT}
    {1102, KEY_BACKSPACE},          // {BACKSPACE}
    {581, },          // {BREAK}
    {1031, KEY_CAPS_LOCK},   // {CAPSLOCK}
    {732, KEY_DELETE},       // {DELETE}
    {490, KEY_DOWN_ARROW},   // {DOWN}
    {299, KEY_END},          // {END}
    {634, KEY_RETURN},       // {ENTER}
    {301, KEY_ESC},          // {ESC}
    {440, KEY_HOME},         // {HOME}
    {821, KEY_INSERT},       // {INSERT}
    {485, KEY_LEFT_ARROW},   // {LEFT}
    {883, 0xDB},             // {NUMLOCK}
    {459, KEY_PAGE_DOWN},    // {PGDN}
    {493, KEY_PAGE_UP},      // {PGUP}
    {589, KEY_PRTSC},        // {PRTSC}
    {640, KEY_RIGHT_ARROW},  // {RIGHT}
    {1323, 0xCF},            // {SCROLLLOCK}
    {284, KEY_TAB},          // {TAB}
    {202, KEY_UP_ARROW},     // {UP}
    {133, KEY_F1},           // {F1}
    {135, KEY_F2},           // {F2}
    {137, KEY_F3},           // {F3}
    {139, KEY_F4},           // {F4}
    {141, KEY_F5},           // {F5}
    {143, KEY_F6},           // {F6}
    {145, KEY_F7},           // {F7}
    {147, KEY_F8},           // {F8}
    {149, KEY_F9},           // {F9}
    {210, KEY_F10},          // {F10}
    {213, KEY_F11},          // {F11}
    {216, KEY_F12},          // {F12}
    {219, KEY_F13},          // {F13}
    {222, KEY_F14},          // {F14}
    {225, KEY_F15},          // {F15}
    {228, KEY_F16},          // {F16}
    {199, KEY_BACKSPACE},          // {BS}
    {314, KEY_DELETE},          // {DEL}
    {345, KEY_INSERT},          // {INS}
    {123, '{'},          // {{}
    {125, '}'},          // {}}
};



/**
 * @brief Chuyển đổi định dạng dành cho USER thành định dạng RAW
 * 
 * @param USER_FORMAT *    request . Xem cú pháp https://learn.microsoft.com/en-us/dotnet/api/system.windows.forms.sendkeys
 * @param ASCII_FORMAT *   command
 * @return int      Độ dài chuỗi kết quả command 
 */
int BleKeyboardBuilder::ConvertFormat(const USER_FORMAT * request, ASCII_FORMAT *  command){
    uint8_t i, oi,j;
    int16_t start_brace_i, checksum;
    char ch;
   
    i = 0;
    oi = 0;
    start_brace_i = -1; // <0 tức là chưa tìm thấy kí tự {

    if (request == NULL) {
        Serial.println("Debug: USER_FORMAT is null!");
        return 0;
    }

    while (true) {
        // Lấy ra kí tự
        ch = request[i];  
        if (ch == 0) {
            command[oi] = 0;    // Đánh dấu điểm cuối cùng
            break;
        } else if (ch == '{') {
            // Đã tìm thấy điểm bắt đầu ngoặc
            start_brace_i = i;
            // Tìm điểm cuối của ngoặc
            i = i+1;
            // Tính checksum là tổng mã ascii của tất cả các kí tự x vị trí, áp dụng dể tìm chuỗi cho nhanh. bản chất là đánh indexing chuỗi
            checksum = 0; 
            while (request[i] != '}' and (request[i] != 0)) { 
                checksum = (checksum >> 1) + request[i] * (i - start_brace_i);               
                i++;
            }
            // Nếu không tìm thấy dấu đóng }, tức là lỗi cú pháp rồi, loại.
            if (request[i] == 0) {
                oi = -start_brace_i;        // Lỗi ở vị trí dấu ngoặc
                Serial.print("Debug: } not found.");
                break;
            }

            // Lúc này command[start_brace_i:i] chứa chuỗi như {ENTER},  {PGUP}
            // Xác định mã của các phím đặc biệt và gán vào command.      
            for (j=0;j < 52; j++) {
                if (checksum2codes[j].checksum == checksum) {
                    // Đã tìm thấy mã của điều khiển
                    if (j < 0x6) {      // là dạng release của 3 phím Ctrl, Alt, Shift
                        command[oi] = ASCII_RELEASE_CODE;
                        oi++;
                    }
                    command[oi] = checksum2codes[j].code ;   
                    oi++;
                    // Đánh dấu đã xử lý xong ngoặc
                    start_brace_i = -1; // Shift, Ctrl, Alt                    
                    break;
                }
            }
            
            // Nếu không tìm thấy cụm từ khóa, tức là lỗi cú pháp rồi, loại.
            if (start_brace_i != -1) {
                oi = -start_brace_i-1;        // Lỗi ở vị trí dấu ngoặc
                Serial.print("Debug: checksum not found, ");
                Serial.println(checksum);
                break;
            }
        }  else {
            command[oi] = request[i];
            oi++;
        }
        i++; // CHuyển sang kí tự tiếp theo
    }
    
    return oi;
}

/* Tóm tắt:
      - việc press() một kí tự thường sẽ làm release() kí tự trước đó.
            bleKeyboardBuilder.press('a');                              bleKeyboardBuilder.press('a');
            bleKeyboardBuilder.release('a');    ====tương đương==>      bleKeyboardBuilder.press('b');
            bleKeyboardBuilder.press('b');
      - việc press() hoặc release() một kí tự thường sẽ KHÔNG release() kí tự điều khiển trước đó. 
        Đoạn code sau sẽ làm phím a được gõ vô hạn và phím shift giữ vô hạn.
            bleKeyboardBuilder.press('a');              
            bleKeyboardBuilder.press(KEY_RIGHT_SHIFT);  
      - việc press() hoặc release() một kí tự điểu khiển sẽ KHÔNG release() kí tự thường trươc đó
            bleKeyboardBuilder.press(KEY_RIGHT_SHIFT);  
            bleKeyboardBuilder.press('a');   
      - việc press kí tự chữ hoa sẽ được hiểu là kèm theo cả press và release phím SHIFT
                                                                bleKeyboardBuilder.press(KEY_RIGHT_SHIFT);
            bleKeyboardBuilder.press('A');     ====tương đương==>      bleKeyboardBuilder.press('a');
                                                                bleKeyboardBuilder.release('a');        
                                                                bleKeyboardBuilder.release(KEY_RIGHT_SHIFT);    
*/
#include "BleKeyboardBuilder.h"
#include <cstdint>

BleKeyboardBuilder::BleKeyboardBuilder(std::string deviceName , std::string deviceManufacturer , uint8_t batteryLevel ) :
    BleKeyboard(deviceName , deviceManufacturer , batteryLevel )
{
};

/**
 * @brief Gửi kí tự ra giao tiếp HID 
 * @param ASCII_FORMAT cmd Chuôi kí tự cần gửi theo cú pháp ASCII_FORMAT. Bắt buộc zero-ending.
 * @example     const char myCmd[] = {KEY_F4, 0};  SendKeys(myCmd);   
 *              const char myCmd[] = {KEY_LEFT_SHIFT, 'c', 'H', 'a', ASCII_RELEASE_CODE, KEY_LEFT_SHIFT, 'o',' ', 'T', 0}; SendKeys(myCmd); 
  */
void BleKeyboardBuilder::SendKeys(const ASCII_FORMAT * cmd) {
    static char ch;
    static uint8_t i;

    i=0;
    while (true){
        ch = cmd[i];

        if (ch == 0) { // Nếu là kí tự kết thúc chuỗi thì nhả tất cả các phím và dừng lại
            releaseAll();    
            return;
        } else if (ch < 0x80) { // Nếu là kí tự thường thì hiện thị
            press(ch);              
            //bleSendKeys.release(ch);
            //Không cần release, vì press kí tự sau sẽ trở thành release kí tự trước (ngoại trừ kí tự diều khiển)
            // Tuy nhiên, hết sức lưu ý phản ứng của thư viện trong 2 trường hợp. ví dụ với cùng doạn mã sau
            //     const char myCmd[] = {ASCII_CODE_PRESS_LEFT_SHIFT, 'c', 'H', 'a', 'o', ASCII_CODE_RELEASE_LEFT_SHIFT, 0};
            //      SendKeys(myCmd);    
            // - CÓ    bleSendKeys.release(ch);   THÌ KẾT QUẢ TRẢ VỀ LÀ    CHao
            // - KHÔNG bleSendKeys.release(ch);   THÌ KẾT QUẢ TRẢ VỀ LÀ    CHAO
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
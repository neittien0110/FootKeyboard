/**
 * Bộ bàn phím dùng chân điều khiển, phù hợp với các hoạt động nhập số liệu hoặc chơi game.
 * Thiết bị giao tiếp máy tính dưới dạng bàn phím bluetooth, không cần driver điều khiển.
 * Sử dụng: 
 *   - như bàn phím thường với các nút bấm to đạp bằng chân
 *   - có thêm 2 socket dạng XH2.54 làm đầu chờ cho dạng nút vặn như volumn.
 *   - Khi mới khởi động, tiếp điện, bấm và giữ nút BOOT để đưa mạch về trạng thái kiểm tra xuất xưởng Self_Test
 * Cấu hình
 *   - Sử dụng máy tính gửi lệnh serial ở tốc độ 115200, 8bit, 0 stop bit.
 *   - Cú pháp lệnh:  <button>=<userformat>
 *     Lưu ý rằng cú pháp này cho giao tiếp máy-máy, đòi hỏi chuỗi phải chính xác.
 *     <userformat> theo cú pháp <https://learn.microsoft.com/en-us/dotnet/api/system.windows.forms.sendkeys>
 *   - Ví dụ lệnh: PEDAL1=a
 *   - Ví dụ lệnh: PEDAL2={ENTER}
 *   - Ví dụ lệnh: PEDAL3=^F4
 *   - Ví dụ lệnh: PEDAL3=+EC
 * Thuật toán mã kĩ tự
 *           USER_FORMAT  -------------------------> ASCII_FORMAT -----------------------------------> KEYCODE
 *     (clean, clear for user)     (simple, fast for cpu, encryp press|release keycode)               (standard)
 */
#include <Arduino.h>
#include "BleKeyboardBuilder.h"

#define LED_BUILTIN 8   //Led mặc định sẵn có trên ESP32-Super Mini
#define BUTTON_BOOT 9   //Nút bấm mặc định sẵn có trên ESP32-Super Mini

#define MAX_BUTTONS 4 // Số lượng pedal/nút bấm tối đa

#define PIN_PEDAL01 1  // Kết nối tới pedal số 1
#define PIN_PEDAL02 2  // Kết nối tới pedal số 2
#define PIN_PEDAL03 10 // Kết nối tới pedal số 3
#define PIN_PEDAL04 7  // Kết nối tới pedal số 4
#define PIN_VAR1 0     // Kết nối tới biến trở / cảm biến lực FSR
#define PIN_VAR2 3     // Kết nối tới biến trở / cảm biến lực FSR

/**
 * @brief Trạng thái tích cực theo mức. Ví dụ 0, 1.
 * @example Ví dụ  digitalRead(3) == PEDAL_ACTIVE_LOGIC
 */
#define PEDAL_ACTIVE_LOGIC 0
#define PEDAL_DEACTIVE_LOGIC 1

/**
 * @brief 4 trạng thái của phím bấm, tạo thành chu trình 4 bước gồm sườn, mức bấm. sườn, mức nhả.
 */
enum KEYSTATUS
{
    KEYFREE = 0,    // phím không được bấm
    KEYDOWN = 1,    // phím vừa được bấm
    KEYPRESS = 2,   // phím bấm giữ
    KEYUP = 3,      // phím vừa được nhả 
};


const uint button_pins[MAX_BUTTONS] = {PIN_PEDAL01, PIN_PEDAL02, PIN_PEDAL03, PIN_PEDAL04};
uint button_prevalues[MAX_BUTTONS];   /// Giá trị trước đó, ở dạng digital 0/1 của nút bấm
uint button_curvalues[MAX_BUTTONS];   /// Giá trị hiện thời, ở dạng digital 0/1 của nút bấm
KEYSTATUS button_status[MAX_BUTTONS]; /// Trạng thái hiện thời của các nút bấm

/// @brief Thời điểm của lần lặp trước đó
unsigned long TimeOfPreLoop;

/**
 * @brief  Handler điều khiển giao tiếp HID Keyboard BLE
 */
BleKeyboardBuilder bleKeyboardBuilder("Foot keyboard", "Bluetooth Device Manufacturer", 100);

#define SERIAL_CONFIG // Cấu hình nút bấm qua serial


#ifdef  SERIAL_CONFIG
    /// Kích thước bộ đệm đọc lệnh từ Serial
    #define SERIAL_BUFFER_SIZE 10
    // Lệnh điều khiển nhận được từ Serial, có dạng userformat
    USER_FORMAT SerialCommand[SERIAL_BUFFER_SIZE];
    /// Phân tách mỗi dòng lệnh điều khiển thành 2 vùng. key=value.
    /// Key ở điểm bắt đầu của commandline, nên luôn trùng với điểm đầu của lệnh
    char * cmdkey = SerialCommand;
    const char KEY01[] = "01";   // mã cấu hình tương ứng với chân PIN_PEDAL01
    const char KEY02[] = "02";   // mã cấu hình tương ứng với chân PIN_PEDAL02
    const char KEY03[] = "03";   // mã cấu hình tương ứng với chân PIN_PEDAL03
    const char KEY04[] = "04";   // mã cấu hình tương ứng với chân PIN_PEDAL04

    // Phân tách mỗi dòng lệnh điều khiển thành 2 vùng. key=value
    USER_FORMAT * cmdvalue = NULL;    
    /// Chuỗi các kí tự cần gửi, ứng với mỗi pedal. Cú pháp đơn kí tự, là bộ mã ASCII nhưng tận dụng mã ascii điều khiển để thành mã phím bấm điều khiển
    ASCII_FORMAT button_sendkeys[MAX_BUTTONS][SERIAL_BUFFER_SIZE];  
#endif

#ifdef SERIAL_CONFIG
/**
 * @brief Cấu hình chức năng các phím/pedal qua Serial
 *
 */
void SerialConfiguration()
{
    uint8_t i;
    /// Nếu không có kết nối serial thì kết thúc luôn
    if (Serial.available() <= 0)
    {
        return;
    }
    
    /// Đọc lệnh từ Serial
    int rlen = Serial.readBytesUntil('\n',SerialCommand, SERIAL_BUFFER_SIZE);
    SerialCommand[rlen] = 0; // đánh dấu kết thúc chuỗi, nếu không sẽ dính với phần còn lại của lệnh trước.

#pragma region PARSING_COMMAND
    cmdvalue = NULL;
    /// Phân tích parsing
    for (i = 0; i < rlen; i++) {// Đọc từng kí tự
        if (SerialCommand[i] == '=') {
            SerialCommand[i]=0; // Đánh dấu Zero kết thúc chuỗi
            cmdvalue = & SerialCommand[i+1];
            break;
        }
    }

    // Nếu độ dài của key, value dưới 2 kí tự là không hợp lệ. Quá ngắn
    if ((i < 2) || ((rlen - i) < 2)) {
        return;
    };
#pragma endregion PARSING_COMMAND

#pragma region ASSIGN_COMMAND
    /// Gán lệnh cho các nút bấm/pedal tương ứng
    if (strcasecmp(cmdkey, KEY01) == 0) {
        i = 0;
    } else if (strcasecmp(cmdkey, KEY02) == 0) {
        i = 1;
    } else if (strcasecmp(cmdkey, KEY03) == 0) {
        i = 2;
    } else if (strcasecmp(cmdkey, KEY04) == 0) {
        i = 3;
    } else {
        // mã phím bấm không phù hợp, kết thúc luôn
        return;
    }
    strcpy(button_sendkeys[i], cmdvalue);

    // Debug
    Serial.print("Key="); Serial.print(i);
    Serial.print(", Value="); Serial.print(button_sendkeys[i]);

#pragma endregion ASSIGN_COMMAND
}

#endif

/**
 * @brief Chế độ tự kiểm tra các chức năng hoạt động, dành cho DEV và QA
 * 
 */
void Self_Test(){
    static uint8_t i;
    // Kiểm tra led với chu ki 0.5 giây và độ rộng xung tăng dần
    for (i=0;i<5;i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(250 + i*50);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250 - i*50);    
    }

    // Gửi các kí tự về 
    const char myCmd[] = {KEY_LEFT_SHIFT, 'c', 'H', 'a', ASCII_RELEASE_CODE, KEY_LEFT_SHIFT, 'o',' ', 'T', 0};
    bleKeyboardBuilder.SendKeys(myCmd);    

    // Treo thiết bị với đèn báo
    while (true){
        digitalWrite(LED_BUILTIN, LOW);
        delay(80);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(400);    
    }
}

/** Kết nối với thiết bị */
bool TryToConnect()
{
    uint8_t led_blink = HIGH;

    // Nếu đã kết nối rồi thì hủy kết nối
    if (bleKeyboardBuilder.isConnected())
    {
        bleKeyboardBuilder.end();
    }

    // Bắt đầu phiên
    bleKeyboardBuilder.begin();
    // Led nháy báo hiệu đang tìm thiết bị kết nôi
    while (!bleKeyboardBuilder.isConnected())
    {
        led_blink = !led_blink;
        digitalWrite(LED_BUILTIN, led_blink);
        delay(100);
    }
    return true;
}

void setup()
{   
    uint8_t i;
    // Cấu hình cho chân pin led mặc định
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef SERIAL_CONFIG
    // Thiết lập kếnh cấu hình phím và debug
    Serial.begin(115200);
#endif

    // Cấu hình cho nút bấm chế độ
    pinMode(BUTTON_BOOT, INPUT);    

    // Kết nối thiết bị liên tục cho tới khi thành công.
    TryToConnect();

    // Led sáng,  báo hiệu kết nối bluetooth thành công
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);

    // Khởi tạo trạng thái cho các chân pedal có chức năng đóng/cắt:
    // - nối vào các chân pin dạng INPUT với điện trở kéo lên bên trong
    // - trạng thái ban đầu là nhả phím
    for (i = 0; i < MAX_BUTTONS; i++)
    {
        pinMode(button_pins[i], INPUT_PULLUP);
        button_status[i] = KEYFREE;
    }

    // 2 nối vào các chân pin dạng INPUT với điện trở kéo xuống ở ngoài board
    pinMode(PIN_VAR1, INPUT);
    pinMode(PIN_VAR2, INPUT);

    // Thiết lập chức năng mặc định cho 4 pedal
    i = 0;
    button_sendkeys[i][0] = KEY_PAGE_DOWN;
    button_sendkeys[i][1] = 0;
    i = 1;
    button_sendkeys[i][0] = KEY_PAGE_UP;
    button_sendkeys[i][1] = 0;
    i = 2;
    button_sendkeys[i][0] = KEY_RETURN;
    button_sendkeys[i][1] = 0;
    i = 3;
    button_sendkeys[i][0] = KEY_LEFT_CTRL;
    button_sendkeys[i][1] = KEY_F4;
    button_sendkeys[i][2] = 0;


    // Đánh dấu thời điểm bắt đầu chạy vòng lặp.
    TimeOfPreLoop = millis();

    // Đèn báo hiệu sẵn sàng
    digitalWrite(LED_BUILTIN, LOW);
    sleep(1);
    digitalWrite(LED_BUILTIN, HIGH);

    //Self_Test();
}


/** Xác định trạng thái phim bấm trong chu trình DOWN, PRESS, UP, FREE */
KEYSTATUS DetectKeyWordflow(uint8_t pre, uint8_t cur)
{
    if (pre == PEDAL_DEACTIVE_LOGIC && cur == PEDAL_ACTIVE_LOGIC)
        return KEYDOWN;
    else if (pre == PEDAL_ACTIVE_LOGIC && cur == PEDAL_ACTIVE_LOGIC)
        return KEYPRESS;
    else if (pre == PEDAL_ACTIVE_LOGIC && cur == PEDAL_DEACTIVE_LOGIC)
        return KEYUP;
    else
        return KEYFREE;
}

void loop()
{
    static int8_t i;
    static uint8_t tmp;
    static uint8_t isDown; // = true nếu có 1 nút bấm/pedal nào đó được bấm

    // Lấy thời điểm hiện tại
    unsigned long currentMillis = millis();

    if ((isDown && (currentMillis - TimeOfPreLoop < 180))     // Nếu có phím bấm thì phải 180ms mới chạy lại.
        || (!isDown && (currentMillis - TimeOfPreLoop < 15))) // Nếu không bấm phím thì cứ 15ms lại kiểm tra lại
    {
        return;
    }

    // đọc trạng thái của cả 4 nút
    for (i = 0; i < MAX_BUTTONS; i++)
    {
        // Lấy giá trị phím bấm mới
        tmp = digitalRead(button_pins[i]);
        // Nếu 3 lần đọc liên tiếp có dạng 010 hoặc 101 thì chuyển về 000 hoặc 111 tương ứng
        if ((tmp == button_prevalues[i]) && (tmp != button_curvalues[i]))
        {
            button_curvalues[i] = tmp;
        };
        // Lưu lại lịch sử, chóng nhảy phím
        button_prevalues[i] = button_curvalues[i];
        button_curvalues[i] = tmp;

        // Xác định trạng thái phím
        button_status[i] = DetectKeyWordflow(button_prevalues[i], button_curvalues[i]);


    //--------------PHASE 2: hành động
    isDown = false;
    if (button_status[0] == KEYDOWN)
    {
        
        bleKeyboardBuilder.SendKeys(button_sendkeys[0]);       //bleKeyboardBuilder.write(KEY_PAGE_DOWN);
        isDown = true;
    }
    if (button_status[1] == KEYDOWN)
    {
        bleKeyboardBuilder.SendKeys(button_sendkeys[1]);       //bleKeyboardBuilder.write(KEY_PAGE_UP);
        isDown = true;
    }
    if (button_status[2] == KEYDOWN)
    {
        bleKeyboardBuilder.SendKeys(button_sendkeys[2]);       // bleKeyboardBuilder.write(KEY_RETURN);
        isDown = true;
    }
    if (button_status[3] == KEYDOWN)
    {
        bleKeyboardBuilder.SendKeys(button_sendkeys[3]);       // bleKeyboardBuilder.press(KEY_LEFT_CTRL);  bleKeyboardBuilder.press(KEY_F4);  delay(10);  bleKeyboardBuilder.releaseAll();
        isDown = true;
    }

    // Nếu có phím được bấm thì mới cần delay để tránh xử lý 2 phím liên tiếp, còn không thì bỏ qua
    // Có cần thiết không nhỉ?
    if (isDown)
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    
    
#ifdef SERIAL_CONFIG
    // Cấu hình chức năng các phím/pedal qua Serial
    SerialConfiguration();
#endif    
}
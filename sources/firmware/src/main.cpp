/**
 * Bộ bàn phím dùng chân điều khiển, phù hợp với các hoạt động nhập số liệu hoặc chơi game. 
 * Thiết bị giao tiếp máy tính dưới dạng bàn phím bluetooth, không cần driver điều khiển.
 */
#include <Arduino.h>
#include <BleKeyboard.h>

//#define DEBUG

#define LED_BUILTIN 8
#define BUTTON_BOOT 9

#define MAX_BUTTONS 4       // Số lượng pedal/nút bấm tối đa

#define PIN_PEDAL01 1       // Kết nối tới pedal số 1
#define PIN_PEDAL02 2       // Kết nối tới pedal số 2
#define PIN_PEDAL03 10      // Kết nối tới pedal số 3
#define PIN_PEDAL04 7       // Kết nối tới pedal số 4
#define PIN_VAR1 0          // Kết nối tới biến trở / cảm biến lực FSR
#define PIN_VAR2 3          // Kết nối tới biến trở / cảm biến lực FSR

/**
 * @brief Trạng thái tích cực theo mức. Ví dụ 0, 1. 
 * @example Ví dụ  digitalRead(3) == PEDAL_ACTIVE_LOGIC
 */
#define PEDAL_ACTIVE_LOGIC 0
#define PEDAL_DEACTIVE_LOGIC 1

enum KEYSTATUS
{
    KEYFREE = 0,
    KEYDOWN = 1,
    KEYPRESS = 2,
    KEYUP = 3,
};

const uint button_pins[MAX_BUTTONS] = {PIN_PEDAL01, PIN_PEDAL02, PIN_PEDAL03, PIN_PEDAL04};
uint button_prevalues[MAX_BUTTONS];  /// Giá trị trước đó, ở dạng digital 0/1 của nút bấm
uint button_curvalues[MAX_BUTTONS];  /// Giá trị hiện thời, ở dạng digital 0/1 của nút bấm
KEYSTATUS button_status[MAX_BUTTONS];/// Trạng thái hiện thời của các nút bấm

/// @brief Thời điểm của lần lặp trước đó
unsigned long TimeOfPreLoop;

int i;

/**
 * @brief  Handler điều khiển giao tiếp HID Keyboard BLE
 */
BleKeyboard bleKeyboard("Foot keyboard", "Bluetooth Device Manufacturer", 100);



/**
 * @brief Trạng thái hoạt động của 4 pedal
 */
KEYSTATUS pedal01_status = KEYFREE;
KEYSTATUS pedal02_status = KEYFREE;
KEYSTATUS pedal03_status = KEYFREE;
KEYSTATUS pedal04_status = KEYFREE;

/** Kết nối với thiết bị */
bool TryToConnect(){
    uint8_t led_blink = HIGH;

    // Nếu đã kết nối rồi thì hủy kết nối
    if (bleKeyboard.isConnected()) {
        bleKeyboard.end();
    }

    //Bắt đầu phiên
    bleKeyboard.begin();
    // Led nháy báo hiệu đang tìm thiết bị kết nôi
    while (!bleKeyboard.isConnected())
    {
        led_blink = ! led_blink;
        digitalWrite(LED_BUILTIN, led_blink);
        delay(100);        
    }
    return true;
}


void setup()
{
    // Cấu hình cho chân pin led mặc định
    pinMode(LED_BUILTIN, OUTPUT);

    // Thiết lập kếnh cấu hình phím và debug
    Serial.begin(115200);

    // Kết nối thiết bị liên tục cho tới khi thành công.    
    TryToConnect(); 
#ifdef DEBUG    
    Serial.println("BLE: connected");
#endif    

    // Led sáng,  báo hiệu kết nối bluetooth thành công
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);

    // Khởi tạo trạng thái cho các chân pedal có chức năng đóng/cắt:
    // - nối vào các chân pin dạng INPUT với điện trở kéo lên bên trong
    // - trạng thái ban đầu là nhả phím
    for (i = 0 ; i < MAX_BUTTONS; i++) {
        pinMode(button_pins[i], INPUT_PULLUP);
        button_status[i] = KEYFREE;
    }
    
    // 2 nối vào các chân pin dạng INPUT với điện trở kéo xuống ở ngoài board
    pinMode(PIN_VAR1, INPUT);
    pinMode(PIN_VAR2, INPUT);

    // Đánh dấu thời điểm bắt đầu chạy vòng lặp. 
    TimeOfPreLoop = millis();

    // Đèn báo hiệu sẵn sàng
    digitalWrite(LED_BUILTIN, LOW);
    sleep(1);
    digitalWrite(LED_BUILTIN, HIGH);
#ifdef DEBUG       
    Serial.println("in loop key press..");
#endif    
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
    static uint8_t tmp;
    static uint8_t isDown;      // = true nếu có 1 nút bấm/pedal nào đó được bấm

    // Lấy thời điểm hiện tại
    unsigned long currentMillis = millis();

    if ( (isDown && (currentMillis - TimeOfPreLoop < 200))   // Nếu có phím bấm thì phải 200ms mới chạy lại
        || (!isDown && (currentMillis - TimeOfPreLoop < 20))) // Nếu không bấm phím thì cứ 20ms lại kiểm tra lại
    {    
        return;
    }

    // đọc trạng thái của cả 4 nút
    for (i = 0 ; i < MAX_BUTTONS; i++) {
        // Lấy giá trị phím bấm mới
        tmp = digitalRead(button_pins[i]);
        // Nếu 3 lần đọc liên tiếp có dạng 010 hoặc 101 thì chuyển về 000 hoặc 111 tương ứng
        if ((tmp == button_prevalues[i]) && (tmp != button_curvalues[i]) ) {
            button_curvalues[i] = tmp;
        };
        //Lưu lại lịch sử, chóng nhảy phím
        button_prevalues[i] = button_curvalues[i];
        button_curvalues[i] = tmp;

        // Xác định trạng thái phím
        button_status[i] = DetectKeyWordflow(button_prevalues[i], button_curvalues[i]);

#ifdef DEBUG
        Serial.print(button_status[i]);
        Serial.print(",");
#endif        
    }    

#ifdef DEBUG    
    Serial.println("");
#endif    

    isDown = false;
    if (button_status[0] == KEYDOWN)
    {
        bleKeyboard.write(KEY_PAGE_DOWN); 
        isDown = true;
    }
    if (button_status[1] == KEYDOWN)
    {
        bleKeyboard.write(KEY_PAGE_UP); 
        isDown = true;        
    }
    if (button_status[2] == KEYDOWN)
    {
        bleKeyboard.write(KEY_RETURN);  
        isDown = true;        
    }
    if (button_status[3] == KEYDOWN)
    {
        bleKeyboard.press(KEY_LEFT_CTRL);
        bleKeyboard.press(KEY_F4);
        delay(10);
        bleKeyboard.releaseAll();
        isDown = true;        
    }

    // Nếu có phím được bấm thì mới cần delay để tránh xử lý 2 phím liên tiếp, còn không thì bỏ qua
    // Có cần thiết không nhỉ?
    if (isDown) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }        
    else {
        digitalWrite(LED_BUILTIN, HIGH);    
        delay(15);        
    }
}

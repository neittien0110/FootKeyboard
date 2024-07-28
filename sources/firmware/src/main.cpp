/**
 * Bộ bàn phím dùng chân điều khiển, phù hợp với các hoạt động nhập số liệu hoặc chơi game. 
 * Thiết bị giao tiếp máy tính dưới dạng bàn phím bluetooth, không cần driver điều khiển.
 */
#include <Arduino.h>
#include <BleKeyboard.h>

//#define DEBUGME

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

#ifdef DEBUGME
    // Thiết lập thông tin debug
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
#endif

    // Kết nối thiết bị liên tục cho tới khi thành công.
    TryToConnect();    

    // Led sáng,  báo hiệu kết nối bluetooth thành công
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);

    // 4 chân pedal có chức năng đóng/cắt, nối vào các chân pin dạng INPUT với điện trở kéo lên bên trong
    for (i = 0 ; i < MAX_BUTTONS; i++) {
        pinMode(button_pins[i], INPUT_PULLUP);
    }
    
    // 2 nối vào các chân pin dạng INPUT với điện trở kéo xuống ở ngoài board
    pinMode(PIN_VAR1, INPUT);
    pinMode(PIN_VAR2, INPUT);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    sleep(1);
    digitalWrite(LED_BUILTIN, HIGH);
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

/**
 * @brief Đọc giá trị từ pedal
 * @details hàm cấp thấp chống nhảy phím với 2 trạng thái bấm, nhả
 * @param Pin_Pedal Chân pin của Pedal. Ví dụ 1, 2, 3
 * @return unsigned byte  Giá trị đọc được. 0, 1
 */
int ReadPedal(uint8_t Pin_Pedal)
{
    static uint8_t i;
    int tmp;
    int current_status;
    // chống nhảy phím
    current_status = digitalRead(Pin_Pedal);
    // nếu phím chưa bấm thì bỏ qua
    //if (current_status != PEDAL_ACTIVE_LOGIC)  {
    //    return !PEDAL_ACTIVE_LOGIC;
    //}
    // bảo đảm phím đã được bấm và duy trì đủ lâu    
    for (i = 0; i < 5; i++)
    {
        tmp = digitalRead(Pin_Pedal);
        if (tmp != current_status)
        {
            i = 0;
            current_status = tmp;
        }
        delay(5);
    }
    return current_status;
}



/**
 * @brief Đọc giá trị từ pedal
 * @details hàm cấp cao 4 trạng thái bấm, nhả, giữ
 * @param Pin_Pedal Chân pin của Pedal. Ví dụ 1, 2, 3
 * @return int  Giá trị đọc được. 0, 1
 */
void GetKeyPad01(KEYSTATUS & pedal )
{
    static uint8_t current_status; // Trạng thái mới của phim 
    static uint8_t previous_status; // Trạng thái cũ của phim 

    previous_status = current_status;    
    current_status = ReadPedal(PIN_PEDAL01);
    pedal = DetectKeyWordflow(previous_status, current_status);
}

/**
 * @brief Đọc giá trị từ pedal
 * @details hàm cấp cao 4 trạng thái bấm, nhả, giữ
 * @param Pin_Pedal Chân pin của Pedal. Ví dụ 1, 2, 3
 * @return int  Giá trị đọc được. 0, 1
 */
void GetKeyPad02(KEYSTATUS & pedal )
{
    static uint8_t current_status; // Trạng thái mới của phim 
    static uint8_t previous_status; // Trạng thái cũ của phim 

    previous_status = current_status;  
    current_status = ReadPedal(PIN_PEDAL02);
    pedal = DetectKeyWordflow(previous_status, current_status);     
}

void GetKeyPad03(KEYSTATUS & pedal )
{
    static uint8_t current_status; // Trạng thái mới của phim 
    static uint8_t previous_status; // Trạng thái cũ của phim 

    previous_status = current_status;
    current_status = ReadPedal(PIN_PEDAL03);  
    pedal = DetectKeyWordflow(previous_status, current_status);

}

/**
 * @brief Đọc giá trị từ pedal
 * @details hàm cấp cao 4 trạng thái bấm, nhả, giữ
 * @param Pin_Pedal Chân pin của Pedal. Ví dụ 1, 2, 3
 * @return int  Giá trị đọc được. 0, 1
 */
void GetKeyPad04(KEYSTATUS & pedal )
{
    static uint8_t current_status; // Trạng thái mới của phim 
    static uint8_t previous_status; // Trạng thái cũ của phim 

    previous_status = current_status;     
    current_status = ReadPedal(PIN_PEDAL04);
    pedal = DetectKeyWordflow(previous_status, current_status);
}

void loop()
{
    static uint8_t tmp;
    static uint8_t isDown;

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
    }    

#ifdef DEBUGME
    Serial.print(pedal01_status);   Serial.print(",");
    Serial.print(pedal02_status);   Serial.print(",");
    Serial.print(pedal03_status);   Serial.print(",");
    Serial.print(pedal04_status);   Serial.println(",");
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

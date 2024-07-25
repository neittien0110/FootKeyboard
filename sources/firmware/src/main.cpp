/**
 * Bộ bàn phím dùng chân điều khiển, phù hợp với các hoạt động nhập số liệu hoặc chơi game. 
 * Thiết bị giao tiếp máy tính dưới dạng bàn phím bluetooth, không cần driver điều khiển.
 */
#include <Arduino.h>
#include <BleKeyboard.h>

//#define DEBUGME

#define LED_BUILTIN 8
#define BUTTON_BOOT 9

#define PIN_PEDAL01 1       // Kết nối tới pedal số 1
#define PIN_PEDAL02 2       // Kết nối tới pedal số 2
#define PIN_PEDAL03 10      // Kết nối tới pedal số 3
#define PIN_PEDAL04 7       // Kết nối tới pedal số 4
#define PIN_VAR1 0          // Kết nối tới biến trở / cảm biến lực FSR
#define PIN_VAR2 3          // Kết nối tới biến trở / cảm biến lực FSR

/**
 * @brief  Handler điều khiển giao tiếp HID Keyboard BLE
 */
BleKeyboard bleKeyboard("Foot keyboard", "Bluetooth Device Manufacturer", 100);

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
    pinMode(PIN_PEDAL01, INPUT_PULLUP);
    pinMode(PIN_PEDAL02, INPUT_PULLUP);
    pinMode(PIN_PEDAL03, INPUT_PULLUP);
    pinMode(PIN_PEDAL04, INPUT_PULLUP);

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

    // Lấy trạng thái pedal
    GetKeyPad01(pedal01_status);
    // Lấy trạng thái pedal
    GetKeyPad02(pedal02_status);
    // Lấy trạng thái pedal
    GetKeyPad03(pedal03_status);
    // Lấy trạng thái pedal
    GetKeyPad04(pedal04_status);            

#ifdef DEBUGME
    Serial.print(pedal01_status);   Serial.print(",");
    Serial.print(pedal02_status);   Serial.print(",");
    Serial.print(pedal03_status);   Serial.print(",");
    Serial.print(pedal04_status);   Serial.println(",");
#endif

    // Đèn hiệu báo
    if ((pedal01_status == KEYPRESS) 
        || (pedal02_status == KEYPRESS) 
       // || (pedal03_status == KEYPRESS) 
       // || (pedal04_status == KEYPRESS)
    )
        digitalWrite(LED_BUILTIN, LOW);
    else
        digitalWrite(LED_BUILTIN, HIGH);

    if (bleKeyboard.isConnected())
    {
        if (pedal01_status == KEYDOWN)
        {
            bleKeyboard.write(KEY_PAGE_DOWN); 
            // bleKeyboard.print("Hello world");
        }
        if (pedal02_status == KEYDOWN)
        {
            bleKeyboard.write(KEY_PAGE_UP); 
        }
        if (pedal03_status == KEYDOWN)
        {
            bleKeyboard.write(KEY_RETURN);  
        }
        if (pedal04_status == KEYDOWN)
        {
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_F4);
            delay(100);
            bleKeyboard.releaseAll();
        }                        
        delay(100);
    }
}

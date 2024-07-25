/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <BleKeyboard.h>

#define LED_BUILDIN 8
#define PIN_PEDAL01 1
#define PIN_PEDAL02 2
#define PIN_PEDAL03 10
#define PIN_PEDAL04 7
#define PIN_VAR1 0
#define PIN_VAR2 3

BleKeyboard bleKeyboard("Foot keyboard", "Bluetooth Device Manufacturer", 100);

enum KEYSTATUS
{
    KEYFREE = 0,
    KEYDOWN = 1,
    KEYPRESS = 2,
    KEYUP = 3,
};

KEYSTATUS pedal01_status = KEYFREE;
KEYSTATUS pedal02_status = KEYFREE;
KEYSTATUS pedal03_status = KEYFREE;
KEYSTATUS pedal04_status = KEYFREE;

int i;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
    bleKeyboard.begin();

    // 4 chân pedal có chức năng đóng/cắt, nối vào các chân pin dạng INPUT với điện trở kéo lên bên trong
    pinMode(PIN_PEDAL01, INPUT_PULLUP);
    pinMode(PIN_PEDAL02, INPUT_PULLUP);
    pinMode(PIN_PEDAL03, INPUT_PULLUP);
    pinMode(PIN_PEDAL04, INPUT_PULLUP);

    // 2 nối vào các chân pin dạng INPUT với điện trở kéo xuống ở ngoài board
    pinMode(PIN_VAR1, INPUT);
    pinMode(PIN_VAR2, INPUT);

    pinMode(LED_BUILDIN, OUTPUT);
    digitalWrite(LED_BUILDIN, LOW);
    sleep(1);
    digitalWrite(LED_BUILDIN, HIGH);
}

/** Xác định trạng thái phim bấm trong chu trình DOWN, PRESS, UP, FREE */
KEYSTATUS DetectKeyWordflow(uint8_t pre, uint8_t cur)
{
    if (pre == 1 && cur == 0)
        return KEYDOWN;
    else if (pre== 0 && cur == 0)
        return KEYPRESS;
    else if (pre== 0 && cur == 1)
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
    int tmp;
    int current_status;
    // chống nhảy phím
    current_status = digitalRead(Pin_Pedal);
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

    // Đèn hiệu báo
    if ((pedal01_status == KEYPRESS) || (pedal02_status == KEYPRESS) || (pedal03_status == KEYPRESS) || (pedal04_status == KEYPRESS))
        digitalWrite(LED_BUILDIN, LOW);
    else
        digitalWrite(LED_BUILDIN, HIGH);

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


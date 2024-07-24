/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <BleKeyboard.h>

#define LED_BUILDIN 8
#define PIN_PEDAL01 1

BleKeyboard bleKeyboard("Foot keyboard", "Bluetooth Device Manufacturer", 100);

enum KEYSTATUS
{
    KEYFREE = 0,
    KEYDOWN = 1,
    KEYPRESS = 2,
    KEYUP = 3,
};

KEYSTATUS pedal01_status = KEYFREE;
KEYSTATUS pedal01_next_status = KEYFREE;
int i;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
    bleKeyboard.begin();

    attachInterrupt(PIN_PEDAL01, Event01, FALLING);

    pinMode(PIN_PEDAL01, INPUT_PULLUP);
    pinMode(LED_BUILDIN, OUTPUT);
    digitalWrite(LED_BUILDIN, LOW);
    sleep(1);
    digitalWrite(LED_BUILDIN, HIGH);
}

/**
 * @brief Đọc giá trị từ pedal
 * @details hàm cấp thấp chống nhảy phím với 2 trạng thái bấm, nhả
 * @param Pin_Pedal Chân pin của Pedal. Ví dụ 1, 2, 3
 * @return int  Giá trị đọc được. 0, 1
 */
int ReadPedal(int Pin_Pedal)
{
    int tmp;
    int current_status;
    // chống nhảy phím
    current_status = digitalRead(Pin_Pedal);
    for (i = 0; i < 10; i++)
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
 * @brief Hàm sự kiện khi ngắt ở xảy ra
 * 
 */
void IRAM_ATTR Event01() {
    if (bleKeyboard.isConnected())
    {
        bleKeyboard.write(KEY_PAGE_DOWN);
    }
}

/**
 * @brief Đọc giá trị từ pedal
 * @details hàm cấp cao 4 trạng thái bấm, nhả, giữ
 * @param Pin_Pedal Chân pin của Pedal. Ví dụ 1, 2, 3
 * @return int  Giá trị đọc được. 0, 1
 */
void ReadPedal01(KEYSTATUS & pedal )
{
    static int current_status;
    static int previous_status;

    previous_status = current_status;
    // chống nhảy phím
    current_status = ReadPedal(PIN_PEDAL01);
    if (previous_status == 1 && current_status == 0)
        pedal = KEYDOWN;
    else if (previous_status == 0 && current_status == 0)
        pedal = KEYPRESS;
    else if (previous_status == 0 && current_status == 1)
        pedal = KEYUP;
    else
        pedal = KEYFREE;
}

void loop()
{

#if false
    // Lấy trạng thái pedal
    ReadPedal01(pedal01_status);

    // Đèn hiệu báo
    if (pedal01_status == KEYPRESS)
        digitalWrite(LED_BUILDIN, LOW);
    else
        digitalWrite(LED_BUILDIN, HIGH);

    if (bleKeyboard.isConnected())
    {
        if (pedal01_status == KEYPRESS)
        {
            delay(100);
            // Lấy trạng thái pedal tiếp theo để xác định click đúp
            ReadPedal01(pedal01_next_status);
            if (pedal01_next_status == KEYFREE or pedal01_next_status == KEYUP) {
                delay(200);
                ReadPedal01(pedal01_next_status);
                if (pedal01_next_status == KEYDOWN) {
                    bleKeyboard.write(KEY_PAGE_UP);
                    delay(600);
                    return;
                }
            }
            bleKeyboard.write(KEY_PAGE_DOWN);
            delay(1200);

            // bleKeyboard.print("Hello world");
            // bleKeyboard.write(KEY_RETURN);
            // bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
            //
            //  Below is an example of pressing multiple keyboard modifiers
            //  which by default is commented out.
            /*
            Serial.println("Sending Ctrl+Alt+Delete...");
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.press(KEY_DELETE);
            delay(100);
            bleKeyboard.releaseAll();
            */
        }
    }
#endif    
}
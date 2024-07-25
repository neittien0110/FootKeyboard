/**
 * 
 * Lưu ý:  Không sử dụng hàm Delay trong chương trình con ngắt. Nếu không sẽ làm watchdog khởi động lại esp
 */
#include <Arduino.h>
#include <BleKeyboard.h>

#define LED_BUILTIN 8
#define BUTTON_BOOT 9

#define PIN_PEDAL01 1       // Kết nối tới pedal số 1
#define PIN_PEDAL02 2       // Kết nối tới pedal số 2
#define PIN_PEDAL03 10      // Kết nối tới pedal số 3
#define PIN_PEDAL04 7       // Kết nối tới pedal số 4
#define PIN_VAR1 0          // Kết nối tới biến trở / cảm biến lực FSR
#define PIN_VAR2 3          // Kết nối tới biến trở / cảm biến lực FSR

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

void Pepal01_Event(){
    if (bleKeyboard.isConnected())
    {
        bleKeyboard.print('1');
    }
}

void Pepal02_Event(){
    if (bleKeyboard.isConnected())
    {
        bleKeyboard.print('2');
    }
}
void Pepal03_Event(){
    if (bleKeyboard.isConnected())
    {
        bleKeyboard.print('3');
    }
}
void Pepal04_Event(){
    if (bleKeyboard.isConnected())
    {
        bleKeyboard.print('4');
    }
}

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

    // Thiết lập thông tin debug
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

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

    // Kích hoạt hàm sự kiện ngắt
    attachInterrupt(PIN_PEDAL01, Pepal01_Event, FALLING);
    attachInterrupt(PIN_PEDAL02, Pepal02_Event, FALLING);
    attachInterrupt(PIN_PEDAL03, Pepal03_Event, FALLING);
    attachInterrupt(PIN_PEDAL04, Pepal04_Event, FALLING);

    // 2 nối vào các chân pin dạng INPUT với điện trở kéo xuống ở ngoài board
    pinMode(PIN_VAR1, INPUT);
    pinMode(PIN_VAR2, INPUT);

    delay(200);
    // Tắt đèn, báo hiệu sẵn sàng
    digitalWrite(LED_BUILTIN, HIGH);        
}

void loop()
{
    delay(1000);
}


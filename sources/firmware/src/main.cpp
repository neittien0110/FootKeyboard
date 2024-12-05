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
 *     <userformat> theo cú pháp <https://github.com/neittien0110/FootKeyboard/tree/master/sources/firmware>
 *   - Ví dụ lệnh: PEDAL1=Chao
 *   - Ví dụ lệnh: PEDAL2={ENTER}
 *   - Ví dụ lệnh: PEDAL3={CTRL}{F4}{~CTRL}
 *   - Ví dụ lệnh: PEDAL3={ATL}{SHIFT}EC{~ATL}{~SHIFT}
*   - Ví dụ lệnh: PEDAL3={SHIFT}{HOME}{~SHIFT}{CTRL}C{~CTRL}{SHIFT}{TAB}{TAB}{~SHIFT}{CTRL}V{~CTRL}
 * Thuật toán mã kĩ tự
 *           USER_FORMAT  -------------------------> ASCII_FORMAT -----------------------------------> KEYCODE
 *     (clean, clear for user)     (simple, fast for cpu, encryp press|release keycode)               (standard)
 */
#include <Arduino.h>
#include "BleKeyboardBuilder.h"
#include "settings.h"
#include "ledeffects.h"
#include "configuration.h"

#if defined(ADRUINO_BOARD_ESP32_C3_SUPERMINI)
    /// Áp dụng khi MCU thuộc họ ESP32-C3
    #define LED_BUILTIN 8   //Led mặc định sẵn có trên ESP32-Super Mini. Tích cực mức thấp.
    #define BUTTON_BOOT 9   //Nút bấm mặc định sẵn có trên ESP32-Super Mini
    #define PIN_PEDAL00 1  // Kết nối tới pedal số 0
    #define PIN_PEDAL01 2  // Kết nối tới pedal số 1
    #define PIN_PEDAL02 10 // Kết nối tới pedal số 2
    #define PIN_PEDAL03 7  // Kết nối tới pedal số 3
    #define PIN_VAR1 0     // Kết nối tới biến trở / cảm biến lực FSR
    #define PIN_VAR2 3     // Kết nối tới biến trở / cảm biến lực FSR    
#elif defined(ADRUINO_BOARD_ESP32_DEV_KIT)
    /// Áp dụng khi MCU thuộc họ ESP32
    #define LED_BUILTIN 02 // Led được thiết kế trên board
    #define BUTTON_BOOT 00   //Nút bấm 2 chức năng, thiết lập trạng thái nạp code và tính năng tùy ý
    #define PIN_PEDAL00 15 // Kết nối tới pedal số 15
    #define PIN_PEDAL01 16 // Kết nối tới pedal số 16
    #define PIN_PEDAL02 17 // Kết nối tới pedal số 17
    #define PIN_PEDAL03 18 // Kết nối tới pedal số 18
    #define PIN_VAR1 04    // Kết nối tới biến trở 
    #define PIN_VAR2 04    // Chưa sử dụng
#elif defined(ARDUINO_ARCH_AVR)
    /// Áp dụng khi CPU thuộc họ AVR như Arduino Uno, Mega, Lilypad
#endif   



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


const uint button_pins[MAX_BUTTONS] = {PIN_PEDAL00, PIN_PEDAL01, PIN_PEDAL02, PIN_PEDAL03};
uint button_prevalues[MAX_BUTTONS];   /// Giá trị trước đó, ở dạng digital 0/1 của nút bấm
uint button_curvalues[MAX_BUTTONS];   /// Giá trị hiện thời, ở dạng digital 0/1 của nút bấm
KEYSTATUS button_status[MAX_BUTTONS]; /// Trạng thái hiện thời của các nút bấm

/// @brief Thời điểm của lần lặp trước đó
unsigned long TimeOfPreLoop;

/**
 * @brief  Handler điều khiển giao tiếp HID Keyboard BLE
 */
BleKeyboardBuilder bleKeyboardBuilder(DEFAULT_BLENAME, "NDT Device Manufacturer", 100);


/**
 * @brief Chế độ tự kiểm tra các chức năng hoạt động, dành cho DEV và QA
 * 
 */
void Self_Test(){
    // Gửi các kí tự về 
    const char myCmd[] = {KEY_LEFT_SHIFT, 'c', 'H', 'a', ASCII_RELEASE_CODE, KEY_LEFT_SHIFT, 'o','.', ' '
    , 'T','o',' ', 'l','a',' ', 'F', 'o', 'o', 't', ' ', 'K', 'e', 'y', 'b', 'o', 'a', 'r', 'd', '.',0};
    bleKeyboardBuilder.SendKeys(myCmd);    

    BleKeyboardBuilder::ConvertFormat("{ENTER}", button_sendkeys[0]);
    Serial.print(button_sendkeys[0]);
    bleKeyboardBuilder.SendKeys(button_sendkeys[0]);

    BleKeyboardBuilder::ConvertFormat("Hello world. I'm FootKeyboard.", button_sendkeys[0]);
    Serial.print(button_sendkeys[0]);
    bleKeyboardBuilder.SendKeys(button_sendkeys[0]);
}

/** Kết nối với thiết bị */
bool TryToConnect()
{
#ifdef DEBUG_VERBOSE
    Serial.println("Khoi tao BLE HID...");
#endif 

    uint8_t led_blink = HIGH;

    // Nếu đã kết nối rồi thì hủy kết nối
    if (bleKeyboardBuilder.isConnected())
    {
#ifdef DEBUG_VERBOSE
        Serial.println("  - Huy ket noi da co");
#endif         
        bleKeyboardBuilder.end();
    }

    // Bắt đầu phiên
#ifdef DEBUG_VERBOSE
    Serial.println("  - Tao ket noi BLE");
#endif         
    bleKeyboardBuilder.begin();

    // Led nháy báo hiệu đang tìm thiết bị kết nôi
#ifdef DEBUG_VERBOSE
    Serial.println("  - Doi thiet bi...");
#endif         
    while (!bleKeyboardBuilder.isConnected())
    {
        led_blink = !led_blink;
        digitalWrite(LED_BUILTIN, led_blink);
        delayMicroseconds(100*1000);
    }
#ifdef DEBUG_VERBOSE
    Serial.println("  - Da ket noi.");
#endif       
    return true;
}

void setup()
{   
    uint8_t i;

    // Cấu hình cho chân pin led mặc định
    pinMode(LED_BUILTIN, OUTPUT);

    // Thiết lập kếnh cấu hình phím và debug
    Serial.begin(115200);

    // Thông báo
#ifdef DEBUG_VERBOSE
    Serial.println("Cau hinh nut bam...");
#endif

    // Cấu hình cho nút bấm chế độ
    pinMode(BUTTON_BOOT, INPUT);    


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

    // Tắt đèn báo
    digitalWrite(LED_BUILTIN, HIGH);

    //TODO : Cần yêu cầu xuất xưởng ở lần chạy code đầu ti

    // Ché độ khôi phục cấu hình xuất xưởng
    if ((digitalRead(BUTTON_BOOT) == 0 ) && (digitalRead(PIN_PEDAL02) == PEDAL_ACTIVE_LOGIC) ) {
#ifdef DEBUG_VERBOSE
        Serial.println("Khoi phuc cau hinh xuat xuong...");
#endif        
        // Khôi phục cấu hình xuất xưởng
        delayMicroseconds(500*1000);
        ResetFactorySetting();

#ifdef DEBUG_VERBOSE
        Serial.println("Khoi phuc xong... Can reset lai board");
#endif                
        // Báo hiệu đã khôi phục xong
        while (true)
        {
            Flash(LED_BUILTIN,3,255);
        }
    }

#if defined(DEBUG_SERIAL_SYNTAX)

    //strcpy(SerialCommand, "01=Xin {SHIFT}chao{~SHIFT} ban!");
    strcpy(SerialCommand, "01={SHIFT}{HOME}{~SHIFT}{CTRL}C{~CTRL}{SHIFT}{TAB}{TAB}{~SHIFT}{CTRL}V{~CTRL}");
    Serial.println(SerialCommand);

    char * key;
    char * myRequest;
    char  myCommand[100];
    int res;
    
    delay(5000);
    Serial.println("--1-");    
    if (!DetermineKeyValue(SerialCommand, & key, & myRequest)) {
        Serial.print("Error: key=value is wrong "); 
    }
    Serial.print("key=");Serial.println(key);    
    Serial.println("--2-");    


    while (true) {    
        // Chuyển đổi chuỗi        
        res = BleKeyboardBuilder::ConvertFormat(myRequest, button_sendkeys[0]);
        res = BleKeyboardBuilder::RevertFormat(button_sendkeys[0], myCommand);

        // Đèn báo hiệu sẵn sàng
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);        
        Serial.println(myRequest);  
        Serial.println("     ");  
        Serial.println(myCommand);
        Serial.println("     ");  
        Serial.println("-----");                          
        delay(2000);
    }
#else
    /// Thời gian trễ giữa 2 lần gửi phím 
    uint16_t k2k;
#ifdef DEBUG_VERBOSE
    Serial.println("Doc cau hinh phim cua cac nut bam");
#endif    
    GetSettings(SerialCommand, &k2k , (void *) button_sendkeys);
#ifdef DEBUG_VERBOSE
    Serial.print("So luong keycode cua moi phim: ");
    Serial.print(strlen(button_sendkeys[0]));  Serial.print(",");
    Serial.print(strlen(button_sendkeys[1]));  Serial.print(",");
    Serial.print(strlen(button_sendkeys[2]));  Serial.print(",");
    Serial.println(strlen(button_sendkeys[3]));
#endif     

#ifdef DEBUG_VERBOSE
    Serial.println("Khoi tao tham so cho BLE Bluetooth");
#endif 
    // Đặt lại tên cho mạng BLE 
    bleKeyboardBuilder.setName(SerialCommand);

    // Thiết lập tốc độ gõ 
    bleKeyboardBuilder.SetKeyPerMinute(k2k);

    // Kết nối thiết bị liên tục cho tới khi thành công.
    TryToConnect();
   
    // Led sáng,  báo hiệu kết nối bluetooth thành công
    digitalWrite(LED_BUILTIN, LOW);

    // Đánh dấu thời điểm bắt đầu chạy vòng lặp.
#ifdef DEBUG_VERBOSE
    Serial.println("Kiem tra chuc nang Selt_Test co duoc kich hoat khong?");
#endif    
    TimeOfPreLoop = millis();
    if ((digitalRead(BUTTON_BOOT) == 0 ) && (digitalRead(PIN_PEDAL01) == PEDAL_ACTIVE_LOGIC)) {
#ifdef DEBUG_VERBOSE
        Serial.println("  - Co. Thuc hien Self_Test");
#endif    
        Self_Test();  //blocking
        // Treo thiết bị với đèn báo 2 lần chớp
#ifdef DEBUG_VERBOSE
        Serial.println("  - Da xong Self_Test");
#endif    
        while (true){
            Flash(LED_BUILTIN,2,255);   
        }
    }
    // Đèn báo hiệu sẵn sàng
    digitalWrite(LED_BUILTIN, LOW);
    sleep(1);
    digitalWrite(LED_BUILTIN, HIGH);

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

/**
 * @brief Chu kì quét đọc giá trị của tất cả các phím/pedal. Đơn vi ms.
 * @details Mỗi lượt quét toàn bộ bàn phím, không delay. Sau đó lượng thời gian này dùng để dợi,
 *          trước khi thực hiện lượt quét bàn phím tiếp theo
 * @example 50 (mất phím) 
 *          30 (thỉnh thoảng mất) 
 *          20 (khả dĩ nhưng lúc mới kết nối, hoặc lâu lâu mới bấm lại thì mất các phím đầu)
 *          15 có vẻ ôn nhất
 *          10 dễ mất phím
 */
#define PERIOD_OF_BUTTON_SCAN_LOOP  15

void loop()
{
    static int8_t i;
    static uint8_t tmp;
    static uint8_t isDown; // = true nếu có 1 nút bấm/pedal nào đó được bấm

    // Lấy thời điểm hiện tại
    unsigned long currentMillis = millis();

    if ((isDown && (currentMillis - TimeOfPreLoop < 180))     // Nếu có phím bấm thì phải 180ms mới chạy lại.
        || (!isDown && (currentMillis - TimeOfPreLoop < PERIOD_OF_BUTTON_SCAN_LOOP))) // Nếu không bấm phím thì cứ 15ms lại kiểm tra lại
    {
        return;
    }
    TimeOfPreLoop = currentMillis;// Đã vượt qua none-blocking delay
    //--------------PHASE 1: đọc trạng thái các nút bấm/pedal
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
    }

    //--------------PHASE 2: hành động
    isDown = false;
    // đọc trạng thái của cả 4 nút
    for (i = 0; i < MAX_BUTTONS; i++)
    {    
        if (button_status[i] == KEYDOWN)
        {
            
            bleKeyboardBuilder.SendKeys(button_sendkeys[i]);       //bleKeyboardBuilder.write(KEY_PAGE_DOWN);
            isDown = true;
        }
    }

    //--------------PHASE 3: hậu xử lý nếu cần
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

    // Cấu hình chức năng các phím/pedal qua Serial
    /// Nếu không có kết nối serial thì kết thúc luôn
    if (Serial.available() > 0)
    {   
        /// Đọc lệnh từ Serial
        int tmp = Serial.readBytesUntil('\n',SerialCommand, SERIAL_BUFFER_SIZE);
        SerialCommand[tmp] = 0; // đánh dấu kết thúc chuỗi, nếu không sẽ dính với phần còn lại của lệnh trước.
        SerialConfiguration(SerialCommand);
    }
}
#include <Arduino.h>
#include <settings.h>

void Flash(uint8_t pin, uint8_t hitcombo, uint8_t loop) {
    static uint8_t i, j ;
    for (i = 0; i < loop; i++) {
       for (j = 0; j < hitcombo; j++) {     
            digitalWrite(pin, LED_ON);
            delayMicroseconds(80* 1000);
            digitalWrite(pin, LED_OFF);
            delayMicroseconds(250 * 1000);            
       }
       delayMicroseconds(80*1000);
       delayMicroseconds(1000*1000);
    }
}

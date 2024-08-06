#include <Arduino.h>

void Flash(uint8_t pin, uint8_t hitcombo, uint8_t loop) {
    static uint8_t i, j ;
    for (i = 0; i < loop; i++) {
       for (j = 0; j < hitcombo; j++) {     
            digitalWrite(pin, LOW);
            delay(80);
            digitalWrite(pin, HIGH);
            delay(250);            
       }
       delay(1000);
    }
}

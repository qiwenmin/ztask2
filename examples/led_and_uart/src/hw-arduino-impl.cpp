#if defined(ARDUINO)

#include <Arduino.h>
#include "config.h"

#if defined(ARDUINO_ESP32_DEV) && !defined(LED_BUILTIN)
#define LED_BUILTIN (2)
#endif

extern "C" void init_hardware() {
    Serial.begin(UART_BAUD_RATE);

    pinMode(LED_BUILTIN, OUTPUT);
}

extern "C" void print_str(const char *msg) {
    Serial.print(msg);
}

extern "C" void print_uint32(uint32_t v) {
    Serial.print(v);
}

extern "C" void print_int32(int32_t v) {
    Serial.print(v);
}

extern "C" void toggle_led() {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

#endif // defined(ARDUINO)

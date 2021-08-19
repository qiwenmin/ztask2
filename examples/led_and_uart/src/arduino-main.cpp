#if defined(ARDUINO)

#include <Arduino.h>
#include "ztask2.h"

extern "C" void init_hardware();
extern "C" void init_tasks();

void setup() {
    init_hardware();

    init_tasks();
}

void loop() {
    zt2_poll();
}

#endif // defined(ARDUINO)

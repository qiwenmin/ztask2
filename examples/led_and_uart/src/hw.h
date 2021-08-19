#ifndef _HW_H
#define _HW_H

#include <stdint.h>

void init_hardware();

uint32_t millis();
void toggle_led();

void print_str(const char *msg);
void print_uint32(uint32_t v);
void print_int32(int32_t v);

#if defined(__SDCC_stm8)
    #include "hw-stm8-def.inc"
#elif defined(PIO_NATIVE_ENV)
    // nothing to include
#elif defined(ARDUINO)
    // nothing to include
#else
    #error Unsupported hardware
#endif

#endif // _HW_H

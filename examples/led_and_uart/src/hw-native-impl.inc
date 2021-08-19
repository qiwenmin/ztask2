#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

void toggle_led() {
}

static uint32_t now_ms() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static uint32_t init_at;

uint32_t millis() {
    return now_ms() - init_at;
}

void print_uint32(uint32_t v) {
    printf("%d", v);
}

void print_int32(int32_t v) {
    printf("%d", v);
}

void print_str(const char *msg) {
    printf("%s", msg);
}

void init_hardware() {
    init_at = now_ms();
}

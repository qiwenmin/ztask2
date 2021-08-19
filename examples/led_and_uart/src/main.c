#include <stdint.h>

#include "hw.h"
#include "ztask2.h"

// memory for ztask2
#define TASK_COUNT (3)
static char task_mem[ZT2_TASK_T_SIZE * TASK_COUNT];

// toogle led task
static uint32_t toggle_cnt = 0;

static void toggle_led_task(void *data) {
    uint32_t *cnt = data;

    print_uint32(millis());
    print_str(" - toggle led [");
    print_uint32((*cnt)++);
    print_str("]\n");

    toggle_led();
}

// uart output task
typedef struct {
    const char *msg;
    int task_id;
    int pal_task_id;
    int counter;
} uot_data_t;

static void uart_output_task(void *data) {
    uot_data_t *p = data;
    print_uint32(millis());
    print_str(" - ");
    print_str(p->msg);
    print_int32(p->task_id);
    print_str(" [");
    print_int32(p->counter++);
    print_str("]\n");

    if ((p->counter & 0x07) == 0) {
        print_str("    >>>> switch task <<<<\n");
        zt2_resume(p->pal_task_id);
        zt2_stop(p->task_id);
    }
}

// ticks function for ztask2
static zt2_ticks_t ticks_func() {
    return millis();
}

void init_tasks() {
    // prepare ztask2
    zt2_config_t cfg = {
        .mem = task_mem,
        .size = TASK_COUNT,
        .tick_f = ticks_func,
    };

    zt2_init(&cfg);

    // toggle led task
    zt2_bind(toggle_led_task, 500, ZT2_TM_REPEAT, &toggle_cnt);

    // uart output task with d1
    static uot_data_t d1 = {
        .msg = "             Task#",
        .counter = 0,
    };
    d1.task_id = zt2_bind(uart_output_task, 700, ZT2_TM_REPEAT, &d1);

    // uart output task with d2
    static uot_data_t d2 = {
        .msg = "                        Task#",
        .counter = 0,
    };
    d2.task_id = zt2_bind(uart_output_task, 1100, ZT2_TM_REPEAT, &d2);

    d1.pal_task_id = d2.task_id;
    d2.pal_task_id = d1.task_id;

    zt2_stop(d2.task_id);
}

#if defined(ARDUINO)

void setup() {
    init_hardware();

    init_tasks();
}

void loop() {
    zt2_poll();
}

#else

void main(void) {
    init_hardware();

    init_tasks();

    // main loop
    while (1) {
        zt2_poll();
    }
}

#endif // defined(ARDUINO)

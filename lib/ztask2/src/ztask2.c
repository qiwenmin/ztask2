#include "ztask2.h"

#define mode_is_running(x) (x & 0b10000000)
#define mode_set_stopped(x) (x &= 0b01111111)
#define mode_set_running(x) (x |= 0b10000000)
#define mode_get(x) (x & 0b00000001)
#define mode_set(x, m) (x = (x & 0b11111110) | m)

static struct {
    int task_count;
    zt2_task_t *tasks;
    int size;
    zt2_tick_func_t tick_f;
} g;

void zt2_init(zt2_config_t *cfg) {
    g.task_count = 0;
    g.tasks = cfg->mem;
    g.size = cfg->size;
    g.tick_f = cfg->tick_f;
}

void zt2_rebind(zt2_task t, zt2_task_func_t task_f, zt2_ticks_t interval, zt2_task_mode mode, void *data) {
    if (t) {
        zt2_task_t *p = t;
        p->task_f = task_f;
        p->interval = interval;
        mode_set(p->mode, mode);
        p->data = data;
    }
}

zt2_task zt2_bind(zt2_task_func_t task_f, zt2_ticks_t interval, zt2_task_mode mode, void *data) {
    if (g.task_count < g.size) {
        zt2_task t = g.tasks + (g.task_count++);
        zt2_rebind(t, task_f, interval, mode, data);
        zt2_resume(t);
        return t;
    }
    else
        return 0;
}

void zt2_update_mode(zt2_task t, zt2_task_mode mode) {
    if (t) {
        mode_set(((zt2_task_t *)t)->mode, mode);
    }
}

void zt2_update_interval(zt2_task t, zt2_ticks_t interval) {
    if (t) {
        ((zt2_task_t *)t)->interval = interval;
    }
}

void zt2_resume(zt2_task t) {
    if (t) {
        zt2_task_t *p = t;
        if (!mode_is_running(p->mode)) {
            p->scheduled_at = g.tick_f();
            mode_set_running(p->mode);
        }
    }
}

void zt2_stop(zt2_task t) {
    if (t) {
        mode_set_stopped(((zt2_task_t *)t)->mode);
    }
}

void zt2_poll() {
    for (zt2_task_t *p = g.tasks; p != g.tasks + g.task_count; p++) {
        zt2_ticks_t curr = g.tick_f();
        if (mode_is_running(p->mode)
            && (curr - p->scheduled_at >= p->interval)) {
            p->scheduled_at = p->interval ? p->scheduled_at + p->interval : curr;
            p->task_f(p->data);

            if (mode_get(p->mode) == ZT2_TM_ONESHOT) {
                mode_set_stopped(p->mode);
            }
        }
    }
}

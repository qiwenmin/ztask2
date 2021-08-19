#include "ztask2.h"

#define mode_is_running(x) (x & 0b10000000)
#define mode_set_stopped(x) (x &= 0b01111111)
#define mode_set_running(x) (x |= 0b10000000)
#define mode_get(x) (x & 0b00000001)
#define mode_set(x, m) (x &= (0b11111110 | m))

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

void zt2_rebind(int id, zt2_task_func_t task_f, zt2_ticks_t interval, zt2_task_mode mode, void *data) {
    if (id < g.task_count) {
        zt2_task_t *p = g.tasks + id;
        p->task_f = task_f;
        p->interval = interval;
        mode_set(p->mode, mode);
        p->data = data;
    }
}

int zt2_bind(zt2_task_func_t task_f, zt2_ticks_t interval, zt2_task_mode mode, void *data) {
    if (g.task_count < g.size) {
        int id = g.task_count++;
        zt2_rebind(id, task_f, interval, mode, data);
        zt2_resume(id);
        return id;
    }
    else
        return -1;
}

void zt2_update_mode(int id, zt2_task_mode mode) {
    if (id < g.task_count)
        mode_set(g.tasks[id].mode, mode);
}

void zt2_update_interval(int id, zt2_ticks_t interval) {
    if (id < g.task_count)
        g.tasks[id].interval = interval;
}

void zt2_resume(int id) {
    if (id < g.task_count) {
        zt2_task_t *p = g.tasks + id;
        if (!mode_is_running(p->mode)) {
            p->scheduled_at = g.tick_f();
            mode_set_running(p->mode);
        }
    }
}

void zt2_stop(int id) {
    if (id < g.task_count) {
        mode_set_stopped(g.tasks[id].mode);
    }
}

void zt2_poll() {
    for (zt2_task_t *p = g.tasks; p != g.tasks + g.task_count; p++) {
        zt2_ticks_t curr = g.tick_f();
        if (mode_is_running(p->mode)
            && (curr - p->scheduled_at >= p->interval)) {
            p->scheduled_at += p->interval;
            p->task_f(p->data);

            if (mode_get(p->mode) == ZT2_TM_ONESHOT) {
                mode_set_stopped(p->mode);
            }
        }
    }
}

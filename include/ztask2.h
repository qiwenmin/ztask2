#ifndef _ZTASK2_H
#define _ZTASK2_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(ZT2_UINT16_TICKS)
    typedef uint16_t zt2_ticks_t;
#else
    typedef uint32_t zt2_ticks_t;
#endif // ZT2_UINT16_TICKS

typedef zt2_ticks_t (*zt2_tick_func_t)(void);

typedef struct {
    void *mem;
    int size;
    zt2_tick_func_t tick_f;
} zt2_config_t;

void zt2_init(zt2_config_t *cfg);

typedef void (*zt2_task_func_t)(void *);

typedef enum {
    ZT2_TM_REPEAT = 0,
    ZT2_TM_ONESHOT = 1
} zt2_task_mode;

int zt2_bind(zt2_task_func_t task_f, zt2_ticks_t interval, zt2_task_mode mode, void *data);
void zt2_rebind(int id, zt2_task_func_t task_f, zt2_ticks_t interval, zt2_task_mode mode, void *data);

void zt2_update_mode(int id, zt2_task_mode mode);
void zt2_update_interval(int id, zt2_ticks_t interval);

void zt2_stop(int id);
void zt2_resume(int id);

void zt2_poll();

typedef struct {
    zt2_task_func_t task_f;
    zt2_ticks_t scheduled_at, interval;
    zt2_task_mode mode;
    void *data;
} zt2_task_t;

#define ZT2_TASK_T_SIZE (sizeof(zt2_task_t))

#ifdef __cplusplus
}
#endif

#endif // _ZTASK2_H

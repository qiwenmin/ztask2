#include "hw.h"
#include "config.h"

void toggle_led() {
    LED_PORT.ODR.byte ^= LED_PIN;
}

static volatile uint32_t __g_ms = 0;

uint32_t millis() {
    uint32_t ret;

    __asm__("push cc");
    __asm__("sim");

    ret = __g_ms;

    __asm__("pop cc");

    return ret;
}

// timer
static void init_timer() {
    // for low-power device activate TIM4 clock
#if defined(FAMILY_STM8L101)
    sfr_CLK.PCKENR.PCKEN |= 0x04;
#endif

    // stop the timer
    sfr_TIM4.CR1.CEN = 0;

    // clear counter
    sfr_TIM4.CNTR.byte = 0x00;
    // auto-reload value buffered
    sfr_TIM4.CR1.ARPE = 1;
    // clear pending events
    sfr_TIM4.EGR.byte  = 0x00;
    // set clock to 16Mhz/2^6 = 250kHz -> 4us period
    sfr_TIM4.PSCR.PSC = 6;
    // set autoreload value for 1ms (=250*4us)
    sfr_TIM4.ARR.byte  = 250;

    // enable timer 4 interrupt
    sfr_TIM4.IER.UIE = 1;
    // start the timer
    sfr_TIM4.CR1.CEN = 1;
}

#if defined(_TIM4_OVR_UIF_VECTOR_)
    ISR_HANDLER(TIM4_UPD_ISR, _TIM4_OVR_UIF_VECTOR_)
#elif defined(_TIM4_UIF_VECTOR_)
    ISR_HANDLER(TIM4_UPD_ISR, _TIM4_UIF_VECTOR_)
#else
    #error TIM4 vector undefined
#endif
{
    // clear timer 4 interrupt flag
    #if defined(FAMILY_STM8S)
    sfr_TIM4.SR.UIF = 0;
    #else
    sfr_TIM4.SR1.UIF = 0;
    #endif

    __g_ms ++;
}

// uart
#define UART_available()   ( sfr_UART.SR.RXNE )
#define UART_read()        ( sfr_UART.DR.byte )
#define UART_write(x)      { while (!(sfr_UART.SR.TXE)); sfr_UART.DR.byte = x; }
#define UART_flush()       { while (!(sfr_UART.SR.TC)); }

volatile uint8_t g_key;

void UART_begin()
{
// for low-power device enable clock gating to USART1
#if defined(FAMILY_STM8L101)
    sfr_CLK.PCKENR.PCKEN |= 0x20;
#endif
#if defined(FAMILY_STM8L)
    sfr_CLK.PCKENR1.PCKEN15 = 1;
#endif

    // reset UART
    sfr_UART.CR1.byte = 0x00;
    sfr_UART.CR2.byte = 0x00;
    sfr_UART.CR3.byte = 0x00;

    // set baudrate (note: BRR2 must be written before BRR1!)
    uint16_t val16 = (uint16_t)(((uint32_t)F_CPU) / UART_BAUD_RATE);
    sfr_UART.BRR2.byte = (uint8_t)(((val16 & 0xF000) >> 8) | (val16 & 0x000F));
    sfr_UART.BRR1.byte = (uint8_t)((val16 & 0x0FF0) >> 4);

    // enable transmission, no transmission interrupt
    sfr_UART.CR2.REN = 1; // enable receiver
    sfr_UART.CR2.TEN = 1; // enable sender
    //sfr_UART.CR2.TIEN = 1;  // enable transmit interrupt
    sfr_UART.CR2.RIEN = 1; // enable receive interrupt

}

ISR_HANDLER(UART_RXNE_ISR, _UART_RXNE_VECTOR_)
{
    // clean UART2 receive flag
    sfr_UART.SR.RXNE = 0;

    // save received byte
    g_key = sfr_UART.DR.byte;

    return;

}

void print_str(const char *s) {
    for (const char *p = s; *p != 0; p++) {
        UART_write(*p);
    }
}

void print_uint32(uint32_t v) {
    if (v == 0) {
        UART_write('0');
        return;
    }

    char buf[11];
    buf[10] = 0;
    char *p = buf + 10;

    while (v != 0) {
        uint32_t d = v % 10;
        v = v / 10;
        p--;
        *p = '0' + d;
    }

    print_str(p);
}

void print_int32(int32_t v) {
    if (v >= 0) {
        print_uint32(v);
    } else {
        UART_write('-');
        print_uint32(-v);
    }
}

void init_hardware() {
    DISABLE_INTERRUPTS();

    // switch to 16MHz (default is 2MHz)
    sfr_CLK.CKDIVR.byte = 0x00;

    UART_begin();

    init_timer();

    // configure LED pin as output
    LED_PORT.DDR.byte = LED_PIN;    // input(=0) or output(=1)
    LED_PORT.CR1.byte = LED_PIN;    // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
    LED_PORT.CR2.byte = LED_PIN;    // input: 0=no exint, 1=exint; output: 0=2MHz slope, 1=10MHz slope

    ENABLE_INTERRUPTS();
}

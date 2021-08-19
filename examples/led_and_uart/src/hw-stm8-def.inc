#if defined(STM8S_BLACK)
    #include "STM8S105C6.h"

    #define LED_PORT   sfr_PORTE
    #define LED_PIN    PIN5

    #define sfr_UART             sfr_UART2
    #define _UART_RXNE_VECTOR_   _UART2_R_RXNE_VECTOR_
#elif defined(STM8S_BLUE)
    #include "STM8S103F3.h"

    #define LED_PORT sfr_PORTB
    #define LED_PIN PIN5

    #define sfr_UART             sfr_UART1
    #define _UART_RXNE_VECTOR_   _UART1_R_RXNE_VECTOR_
#elif defined(STM8S_STM8L101DEVBRD01)
    #include "STM8L101F3.h"

    #define LED_PORT sfr_PORTB
    #define LED_PIN PIN5

    #define sfr_UART             sfr_USART
    #define _UART_RXNE_VECTOR_   _USART_R_RXNE_VECTOR_
#else
    #error board is unsupported
#endif

#if defined(_TIM4_OVR_UIF_VECTOR_)
    ISR_HANDLER(TIM4_UPD_ISR, _TIM4_OVR_UIF_VECTOR_);
#elif defined(_TIM4_UIF_VECTOR_)
    ISR_HANDLER(TIM4_UPD_ISR, _TIM4_UIF_VECTOR_);
#else
    #error TIM4 vector undefined
#endif

ISR_HANDLER(UART_RXNE_ISR, _UART_RXNE_VECTOR_);

#if defined(__SDCC_stm8)
    #include "hw-stm8-impl.inc"
#elif defined(PIO_NATIVE_ENV)
    #include "hw-native-impl.inc"
#else
    #error Unsupported hardware
#endif

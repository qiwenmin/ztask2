#if defined(__SDCC_stm8)
    #include "hw-stm8-impl.inc"
#elif defined(PIO_NATIVE_ENV)
    #include "hw-native-impl.inc"
#elif defined(ARDUINO)
    // nothing to include
#else
    #error Unsupported hardware
#endif

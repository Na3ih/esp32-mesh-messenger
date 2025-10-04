#pragma once

#ifdef LED_BUILTIN
#define LED_PIN LED_BUILTIN
#else
#define LED_PIN (2)
#endif

#define DEBUG_PIN (0)

#define   BLINK_PERIOD_MS    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION_MS  100  // milliseconds LED is on for
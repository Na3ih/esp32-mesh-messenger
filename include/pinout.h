#pragma once

#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else
#define LED (2)
#endif

#define   BLINK_PERIOD_MS    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION_MS  100  // milliseconds LED is on for
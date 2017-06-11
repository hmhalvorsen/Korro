#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define PIN_CHANNEL_1 11
#define PIN_CHANNEL_2 12
#define PIN_CHANNEL_3 13
#define PIN_CHANNEL_4 14

typedef struct
{
  uint32_t high_flank_time[4];
  uint32_t low_flank_time[4];

  uint32_t channel_value[4];
}channel_time_t;

// Prototypes for functions to interface with rc_pwm

void rc_init(void);
void rc_get_values(float* fetch_array);

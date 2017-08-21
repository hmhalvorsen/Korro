#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define TIME_PER_ROUND_MS 2000
#define PATTERN_MS TIME_PER_ROUND_MS/NUMBER_OF_LEDS

#define DEFAULT_PATTERN 1
#define DEFAULT_GREEN_VALUE 0
#define DEFAULT_RED_VALUE 255
#define DEFAULT_BLUE_VALUE 255
#define DEFAULT_LENGTH 5
#define DEFAULT_SPEED PATTERN_MS
#define NUMBER_OF_LOOPS 5
#define NUMBER_OF_PATTERNS 5

typedef struct {
	uint8_t current_pattern;
	uint8_t green;
	uint8_t red;
	uint8_t blue;
	uint8_t length;
	uint8_t speed;
}Pattern_t;

void adr_led_init(void);
void change_pattern(Pattern_t *pattern);
void reset_all_led(void);
void led_event_handler(void);

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define PWM_TOP_VALUE 20 // Counting to 20, PWM period 1.25µs, frequency 800kHz.
#define PRINT_PWM 0

#define LED_OUTPUT_PIN  3
#define NUMBER_OF_LEDS  24
#define DATA_BIT_LENGTH 24

#define PWM_VALUE_ZERO 14 //12.8f
#define PWM_VALUE_ONE	 9 //9.6f

// Prototypes for functions used in source file:

void led_pwm_init(void);
void led_start_pwm(void);
void update_pwm_sequence_multiple_led(uint8_t *green_array, uint8_t *red_array, uint8_t *blue_array);
void update_pwm_sequence_single_led(uint8_t green, uint8_t red, uint8_t blue, uint8_t led);
void set_led(uint8_t *green, uint8_t *red, uint8_t *blue);

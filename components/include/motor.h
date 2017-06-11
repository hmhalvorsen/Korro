#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MOTOR_A_PWM_OUTPUT_PIN 18
#define MOTOR_B_PWM_OUTPUT_PIN 17
#define MOTOR_A_DIRECTION			 16
#define MOTOR_B_DIRECTION      15

#define PWM_TOP_VALUE 1000 // Counting to 1000, 16kHz
#define MOTOR_CONSTANT 10.0f

typedef struct
{
	uint32_t output_motor_a;
  uint32_t output_motor_b;

  bool direction_motor_a; // TRUE = CW, FALSE = CCW
  bool direction_motor_b; // TRUE = CW, FALSE = CCW
}motor_t;

// Prototypes for functions used in source file:

void motor_pwm_init(void);
void set_motor(motor_t *motor);
void motor_logic(motor_t *motor);

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_drv_pwm.h"
#include "nrf_gpio.h"
#include "nrf_log.h"

#include "printing.h"
#include "motor.h"

void motor_start_pwm(void);

static nrf_drv_pwm_t motor_pwm = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_individual_t pwm_values;

static void pwm_event_handler(nrf_drv_pwm_evt_type_t event_type)
{

}

void motor_pwm_init(void)
{
  ret_code_t err_code;
  nrf_drv_pwm_config_t const pwm_config =
  {
    .output_pins =
      {
        MOTOR_A_PWM_OUTPUT_PIN, // Motor A PWM output
        MOTOR_B_PWM_OUTPUT_PIN, // Motor B PWM output
        MOTOR_C_PWM_OUTPUT_PIN,
        MOTOR_D_PWM_OUTPUT_PIN,
      },
      .base_clock  = NRF_PWM_CLK_16MHz,
      .count_mode  = NRF_PWM_MODE_UP,
      .top_value   = PWM_TOP_VALUE,
      .load_mode   = NRF_PWM_LOAD_INDIVIDUAL,
      .step_mode   = NRF_PWM_STEP_AUTO
  };

  nrf_gpio_cfg_output(MOTOR_A_DIRECTION);
  nrf_gpio_cfg_output(MOTOR_B_DIRECTION);
  nrf_gpio_cfg_output(MOTOR_C_DIRECTION);
  nrf_gpio_cfg_output(MOTOR_D_DIRECTION);

  err_code = nrf_drv_pwm_init(&motor_pwm, &pwm_config, pwm_event_handler);
  APP_ERROR_CHECK(err_code);

  pwm_values.channel_0 = 0; // Initial values for the PWM channels
  pwm_values.channel_1 = 0;
  pwm_values.channel_2 = 0;
  pwm_values.channel_3 = 0;

	NRF_LOG_RAW_INFO("Motor PWM initiated. \n");
}

void motor_start_pwm(void)
{
  nrf_pwm_sequence_t const seq =
  {
    .values.p_individual = &pwm_values,
    .length          		 = NRF_PWM_VALUES_LENGTH(pwm_values),
    .repeats         		 = 0,
    .end_delay       		 = 0,
  };

	#if PRINT_PWM
		NRF_LOG_RAW_INFO("PWM values updated. PWMA: %d - PWMB: %d \n", pwm_values.channel_0, pwm_values.channel_1);
  #endif
	nrf_drv_pwm_simple_playback(&motor_pwm, &seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

void motor_logic(motor_t *motor)
{
  if(motor->direction_motor_left)
  {
    nrf_gpio_pin_set(MOTOR_A_DIRECTION);
    nrf_gpio_pin_set(MOTOR_B_DIRECTION);
  }
  else
  {
    nrf_gpio_pin_clear(MOTOR_A_DIRECTION);
    nrf_gpio_pin_clear(MOTOR_B_DIRECTION);
  }

  if(motor->direction_motor_right)
  {
    nrf_gpio_pin_set(MOTOR_C_DIRECTION);
    nrf_gpio_pin_set(MOTOR_D_DIRECTION);
  }
  else
  {
    nrf_gpio_pin_clear(MOTOR_C_DIRECTION);
    nrf_gpio_pin_clear(MOTOR_D_DIRECTION);
  }
}

void set_motor(motor_t *motor)
{
  motor_logic(motor);

  // Update outputted pwm values

  pwm_values.channel_0 = PWM_TOP_VALUE - (uint16_t)motor->output_motor_left;
  pwm_values.channel_1 = PWM_TOP_VALUE - (uint16_t)motor->output_motor_left;
  pwm_values.channel_2 = PWM_TOP_VALUE - (uint16_t)motor->output_motor_right;
  pwm_values.channel_3 = PWM_TOP_VALUE - (uint16_t)motor->output_motor_right;

	#if PRINT_MOTOR
	NRF_LOG_RAW_INFO("Motor left:: power: %d - direction: %d \t Motor right:: power: %d - direction: %d \n", (uint16_t)motor->output_motor_left, motor->direction_motor_left, (uint16_t)motor->output_motor_right, motor->direction_motor_right);
	#endif

  motor_start_pwm();
}

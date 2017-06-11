//Woodbot #1 Korro

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "boards.h"
#include "bsp.h"
#include "nrf_drv_timer.h"
#include "nrf_log.h"

#include "rc_pwm.h"
#include "motor.h"

const nrf_drv_timer_t application_timer = NRF_DRV_TIMER_INSTANCE(0);

void compute_trajectory(float *channel_values, motor_t *motor)
{
  motor->output_motor_a = ((uint32_t)channel_values[0] * PWM_TOP_VALUE)/100;
  motor->output_motor_b = ((uint32_t)channel_values[1] * PWM_TOP_VALUE)/100;
	
	if(motor->output_motor_a <= 0)
		motor->output_motor_a = 0;
	else if(motor->output_motor_a >= PWM_TOP_VALUE)
		motor->output_motor_a = PWM_TOP_VALUE;
	
	if(motor->output_motor_b <= 0)
		motor->output_motor_b = 0;
	else if(motor->output_motor_b >= PWM_TOP_VALUE)
		motor->output_motor_b = PWM_TOP_VALUE;
}


void timer_event_handler(nrf_timer_event_t event_type, void* p_context)
{
  static float channel_values[4] = {0};
  static motor_t motor;
	
  nrf_gpio_pin_toggle(19);
  rc_get_values(channel_values);
  compute_trajectory(channel_values, &motor);
  set_motor(&motor);
}

//Init function for initiation of the main timer.
void timer_init()
{
  uint32_t err_code;

	uint32_t time_ms = 50; //How often the timer event will be called.
  uint32_t time_ticks;

  static nrf_drv_timer_config_t timer_cfg =
  {
      .frequency = NRF_TIMER_FREQ_16MHz,
      .mode      = NRF_TIMER_MODE_TIMER,
      .bit_width = NRF_TIMER_BIT_WIDTH_32,
  };

  err_code = nrf_drv_timer_init(&application_timer, &timer_cfg, timer_event_handler);
  APP_ERROR_CHECK(err_code);

	time_ticks = nrf_drv_timer_ms_to_ticks(&application_timer, time_ms);

  nrf_drv_timer_extended_compare(&application_timer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

  nrf_drv_timer_enable(&application_timer);

	NRF_LOG_RAW_INFO("Main timer enabled. \n \n Commencing control algorithm. \n");
}

int main(void)
{
  //Extra
  nrf_log_init();
  NRF_LOG_INFO("\n\nInitiating all systems. \n");
	
	nrf_gpio_cfg_output(19);
	
  // Radio and motor initiation
	
  rc_init(); // Init driver for radio controller.
  motor_pwm_init(); // Init motor PWM.

  timer_init(); // Init timer to run main algorithm.
    while (true)
    {

    }
}

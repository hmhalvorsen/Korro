#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_drv_pwm.h"
#include "nrf_gpio.h"
#include "nrf_log.h"

#include "adr_led.h"

static nrf_drv_pwm_t led_pwm = NRF_DRV_PWM_INSTANCE(1);
static nrf_pwm_values_common_t pwm_values[DATA_BIT_LENGTH*NUMBER_OF_LEDS];

static void pwm_event_handler(nrf_drv_pwm_evt_type_t event_type)
{
	if (event_type == NRF_DRV_PWM_EVT_FINISHED)
	{
			nrf_gpio_pin_clear(LED_OUTPUT_PIN);
	}
}

uint8_t check_bit(uint8_t byte, uint8_t bit)
{
  static uint8_t bit_mask = 0x01;

  return ((byte >> bit) & bit_mask);
}

uint8_t led_check_bit(uint8_t byte)
{
	static int i = 7;
	uint8_t return_value = check_bit(byte, i);

	i -= 1;

	if(i < 0)
		i = 7;

	return(return_value);
}

void update_pwm_sequence_single_led(uint8_t green, uint8_t red, uint8_t blue, uint8_t led)
{
  for(int i = 0; i < DATA_BIT_LENGTH; i++)
  {
    if(i <= 7)
    {
      if(led_check_bit(green))
        pwm_values[i + DATA_BIT_LENGTH*led] = PWM_VALUE_ONE;
      else
        pwm_values[i + DATA_BIT_LENGTH*led] = PWM_VALUE_ZERO;
    }
    else if(i >= 8 && i <= 15)
    {
      if(led_check_bit(red))
        pwm_values[i + DATA_BIT_LENGTH*led] = PWM_VALUE_ONE;
      else
        pwm_values[i + DATA_BIT_LENGTH*led] = PWM_VALUE_ZERO;
    }
    else if(i >= 16 && i <= 23)
    {
      if(led_check_bit(blue))
        pwm_values[i + DATA_BIT_LENGTH*led] = PWM_VALUE_ONE;
      else
        pwm_values[i + DATA_BIT_LENGTH*led] = PWM_VALUE_ZERO;
    }
  }
}

void update_pwm_sequence_multiple_led(uint8_t *green_array, uint8_t *red_array, uint8_t *blue_array)
{
	for(uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
	{
		update_pwm_sequence_single_led(green_array[i], red_array[i], blue_array[i], i);
	}
}

void led_pwm_init(void)
{
  ret_code_t err_code;
  nrf_drv_pwm_config_t const pwm_config =
  {
    .output_pins =
      {
        LED_OUTPUT_PIN | NRF_DRV_PWM_PIN_INVERTED, // LED PWM output
        NRF_DRV_PWM_PIN_NOT_USED,
        NRF_DRV_PWM_PIN_NOT_USED,
        NRF_DRV_PWM_PIN_NOT_USED,
      },
      .base_clock  = NRF_PWM_CLK_16MHz,
      .count_mode  = NRF_PWM_MODE_UP,
      .top_value   = PWM_TOP_VALUE,
      .load_mode   = NRF_PWM_LOAD_COMMON,
      .step_mode   = NRF_PWM_STEP_AUTO
  };

	nrf_gpio_cfg_output(LED_OUTPUT_PIN);

  err_code = nrf_drv_pwm_init(&led_pwm, &pwm_config, pwm_event_handler);
  APP_ERROR_CHECK(err_code);

	NRF_LOG_RAW_INFO("LED PWM initiated. \n");
}

void led_start_pwm(void)
{
  nrf_pwm_sequence_t const seq =
  {
    .values.p_common     = pwm_values,
    .length          		 = NRF_PWM_VALUES_LENGTH(pwm_values),
    .repeats         		 = 0,
    .end_delay       		 = 0,
  };

	#if PRINT_PWM
		NRF_LOG_RAW_INFO("PWM values updated. PWMA: %d \n", pwm_values.channel_0);
  #endif

	nrf_drv_pwm_simple_playback(&led_pwm, &seq, 1, NRF_DRV_PWM_FLAG_STOP);
}

void set_led(uint8_t *green_array, uint8_t *red_array, uint8_t *blue_array)
{
	update_pwm_sequence_multiple_led(green_array, red_array, blue_array); //TODO: Update this
	led_start_pwm();
}

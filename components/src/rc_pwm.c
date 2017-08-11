#include <stdbool.h>
#include <stdint.h>

#include "nrf_drv_gpiote.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"
#include "nrf_log.h"

#include "printing.h"
#include "rc_pwm.h"

static channel_time_t channel;

nrf_drv_timer_t rc_timer_1 = NRF_DRV_TIMER_INSTANCE(1);
nrf_drv_timer_t rc_timer_2 = NRF_DRV_TIMER_INSTANCE(2);
nrf_drv_timer_t rc_timer_3 = NRF_DRV_TIMER_INSTANCE(3);
nrf_drv_timer_t rc_timer_4 = NRF_DRV_TIMER_INSTANCE(4);

// Extracts rc values in percent, making it a coefficient to be multiplied

void rc_get_values(float* fetch_array)
{
  for(uint8_t i = 0; i<4; i++)
	{
		fetch_array[i] = (channel.channel_value[i] - 1000.0f)/1000.0f;
	}
}

void rc_get_high_flank(uint8_t pin)
{
  static float us_per_tick = (1.0f/16000000.0f)*1000000.0f;

  switch(pin){
    case 0:
      channel.high_flank_time[0] = NRF_TIMER1->CC[1]*us_per_tick;
      break;
    case 1:
      channel.high_flank_time[1] = NRF_TIMER2->CC[2]*us_per_tick;
      break;
    case 2:
      channel.high_flank_time[2] = NRF_TIMER3->CC[3]*us_per_tick;
      break;
    case 3:
      channel.high_flank_time[3] = NRF_TIMER4->CC[4]*us_per_tick;
      break;
  }
}

void rc_get_low_flank(uint8_t pin)
{
  static float us_per_tick = (1.0f/16000000.0f)*1000000.0f;

  switch(pin){
    case 0:
      channel.low_flank_time[0] = NRF_TIMER1->CC[1]*us_per_tick;
      break;
    case 1:
      channel.low_flank_time[1] = NRF_TIMER2->CC[2]*us_per_tick;
      break;
    case 2:
      channel.low_flank_time[2] = NRF_TIMER3->CC[3]*us_per_tick;
      break;
    case 3:
      channel.low_flank_time[3] = NRF_TIMER4->CC[4]*us_per_tick;
      break;
  }
  channel.channel_value[pin] = channel.low_flank_time[pin] - channel.high_flank_time[pin];
}

void rc_pin_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	static bool state1 = 0, state2 = 0, state3 = 0, state4 = 0;
	uint8_t state;
  uint8_t channel_number;

  switch(pin){
    case PIN_CHANNEL_1:
      channel_number = 0;
				if(state1 == 0)
						state1 = 1;
				else
						state1 = 0;

				state = state1;
      break;
    case PIN_CHANNEL_2:
      channel_number = 1;
				if(state2 == 0)
						state2 = 1;
				else
						state2 = 0;

				state = state2;
      break;
    case PIN_CHANNEL_3:
        channel_number = 2;
				if(state3 == 0)
						state3 = 1;
				else
						state3 = 0;

				state = state3;
        break;
    case PIN_CHANNEL_4:
        channel_number = 3;
				if(state4 == 0)
						state4 = 1;
				else
						state4 = 0;

				state = state4;
        break;
  }

  if(state == 1)
  {
    rc_get_high_flank(channel_number);
  }
  else if(state == 0)
  {
    rc_get_low_flank(channel_number);

    #if PRINT_RC_CHANNEL
      NRF_LOG_RAW_INFO("Channel values: %d - %d - %d - %d \n", channel.channel_value[0], channel.channel_value[1], channel.channel_value[2], channel.channel_value[3]);
    #endif
  }
}

void rct_event_handler(nrf_timer_event_t event_type, void* p_context)
{

}

void rc_timer_init(void)
{
  uint32_t err_code;

  static nrf_drv_timer_config_t rct_cfg =
  {
      .frequency = NRF_TIMER_FREQ_16MHz,
      .mode      = NRF_TIMER_MODE_TIMER,
      .bit_width = NRF_TIMER_BIT_WIDTH_32,
  };

  err_code = nrf_drv_timer_init(&rc_timer_1, &rct_cfg, rct_event_handler);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_timer_init(&rc_timer_2, &rct_cfg, rct_event_handler);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_timer_init(&rc_timer_3, &rct_cfg, rct_event_handler);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_timer_init(&rc_timer_4, &rct_cfg, rct_event_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_timer_enable(&rc_timer_1);
  nrf_drv_timer_enable(&rc_timer_2);
  nrf_drv_timer_enable(&rc_timer_3);
  nrf_drv_timer_enable(&rc_timer_4);

  NRF_LOG_RAW_INFO("Radio controller timers enabled. \n");
}

void rc_gpiote_init(void)
{
  ret_code_t err_code;

  if(!nrf_drv_gpiote_is_init())
	{
    err_code = nrf_drv_gpiote_init();
	}
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
  config.pull = NRF_GPIO_PIN_PULLDOWN;

  err_code = nrf_drv_gpiote_in_init(PIN_CHANNEL_1, &config, rc_pin_event_handler);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_gpiote_in_init(PIN_CHANNEL_2, &config, rc_pin_event_handler);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_gpiote_in_init(PIN_CHANNEL_3, &config, rc_pin_event_handler);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_gpiote_in_init(PIN_CHANNEL_4, &config, rc_pin_event_handler);
	APP_ERROR_CHECK(err_code);

  NRF_LOG_RAW_INFO("Radio controller GPIOTE enabled. \n");
}

void rc_ppi_init(void)
{
  ret_code_t err_code;

	nrf_ppi_channel_t ppi_channel_1, ppi_channel_2, ppi_channel_3, ppi_channel_4;

  uint32_t gpiote_evt_addr_1, gpiote_evt_addr_2, gpiote_evt_addr_3, gpiote_evt_addr_4;
  uint32_t capture_task_addr_1, capture_task_addr_2, capture_task_addr_3, capture_task_addr_4;

  err_code = nrf_drv_ppi_init();
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_alloc(&ppi_channel_1);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_alloc(&ppi_channel_2);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_alloc(&ppi_channel_3);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_alloc(&ppi_channel_4);
  APP_ERROR_CHECK(err_code);

  gpiote_evt_addr_1 = nrf_drv_gpiote_in_event_addr_get(PIN_CHANNEL_1);
  gpiote_evt_addr_2 = nrf_drv_gpiote_in_event_addr_get(PIN_CHANNEL_2);
  gpiote_evt_addr_3 = nrf_drv_gpiote_in_event_addr_get(PIN_CHANNEL_3);
  gpiote_evt_addr_4 = nrf_drv_gpiote_in_event_addr_get(PIN_CHANNEL_4);

  capture_task_addr_1 = nrf_drv_timer_task_address_get(&rc_timer_1, NRF_TIMER_TASK_CAPTURE1);
  capture_task_addr_2 = nrf_drv_timer_task_address_get(&rc_timer_2, NRF_TIMER_TASK_CAPTURE2);
  capture_task_addr_3 = nrf_drv_timer_task_address_get(&rc_timer_3, NRF_TIMER_TASK_CAPTURE3);
  capture_task_addr_4 = nrf_drv_timer_task_address_get(&rc_timer_4, NRF_TIMER_TASK_CAPTURE4);

  err_code = nrf_drv_ppi_channel_assign(ppi_channel_1, gpiote_evt_addr_1, capture_task_addr_1);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_assign(ppi_channel_2, gpiote_evt_addr_2, capture_task_addr_2);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_assign(ppi_channel_3, gpiote_evt_addr_3, capture_task_addr_3);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_assign(ppi_channel_4, gpiote_evt_addr_4, capture_task_addr_4);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_enable(ppi_channel_1);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_enable(ppi_channel_2);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_enable(ppi_channel_3);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_ppi_channel_enable(ppi_channel_4);
  APP_ERROR_CHECK(err_code);

	NRF_LOG_RAW_INFO("PPI initiated on four channels. \n");
}

void rc_init(void)
{
  rc_timer_init();
  rc_gpiote_init();
  rc_ppi_init();

	nrf_drv_gpiote_in_event_enable(PIN_CHANNEL_1, true);
	nrf_drv_gpiote_in_event_enable(PIN_CHANNEL_2, true);
//	nrf_drv_gpiote_in_event_enable(PIN_CHANNEL_3, true);
//	nrf_drv_gpiote_in_event_enable(PIN_CHANNEL_4, true);

  NRF_LOG_RAW_INFO("Radio controller initiated. \n");
}

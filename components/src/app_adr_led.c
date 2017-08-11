#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "app_adr_led.h"
#include "adr_led.h"
#include "nrf_drv_timer.h"
#include "nrf_log.h"

static Pattern_t pattern;

// Makes a wave of one LED transcend the led strip.

void pattern_1(uint8_t green, uint8_t red, uint8_t blue, uint8_t length)
{
  static uint8_t i = 0;
  uint8_t k = i - 1;

  if(i > (NUMBER_OF_LEDS-1))
    i = 0;

  if(k > (NUMBER_OF_LEDS-1))
    k = 23; //This means we are on the first LED and want the last to be clear.

  // Set the previous LED low.

  update_pwm_sequence_single_led(0, 0, 0, k);

  // Set the LED train high.
  for(uint8_t l = 0; l < length; l++){
		if(i+l < NUMBER_OF_LEDS)
			update_pwm_sequence_single_led(green, red, blue, i+l);
		else
			update_pwm_sequence_single_led(green, red, blue, (length-l)-1);
  }

  led_start_pwm();

  i++;
}

// Fills the whole strip with a color and then clears it gradually

void pattern_2(uint8_t green, uint8_t red, uint8_t blue)
{
  static bool fill = true;
  static uint8_t i = 0;

  if(i > (NUMBER_OF_LEDS-1) && fill == true)
  {
    i = 0;
    fill = false;
  }
  else if (i > (NUMBER_OF_LEDS-1) && fill == false)
  {
    i = 0;
    fill = true;
  }

  if(fill == true)
      update_pwm_sequence_single_led(green, red, blue, i);
  else
      update_pwm_sequence_single_led(0, 0, 0, i);

  led_start_pwm();

  i++;
}

// Fills all LED in a seemingly random pattern, then clears them. Makes a sort of breathing rythm.

void pattern_3(uint8_t green, uint8_t red, uint8_t blue)
{
  static uint8_t fill_array[24] = {0, 21, 8, 3, 18, 13, 7, 9, 15, 22, 1, 12, 19, 2, 17, 11, 4, 23, 10, 5, 20, 6, 14, 16};

  static bool fill = true;
  static uint8_t i = 0;

  if(i > (NUMBER_OF_LEDS-1) && fill == true)
  {
    i = 0;
    fill = false;
  }
  else if (i > (NUMBER_OF_LEDS-1) && fill == false)
  {
    i = 0;
    fill = true;
  }

  if(fill == true)
      update_pwm_sequence_single_led(green, red, blue, fill_array[i]);
  else
      update_pwm_sequence_single_led(0, 0, 0, fill_array[i]);

  led_start_pwm();

  i++;
}

// Makes a clock that can be used as a timer

void pattern_4(uint8_t green, uint8_t red, uint8_t blue)
{
  static uint8_t i = 0;
	static uint8_t h = NUMBER_OF_LEDS-1;
  uint8_t k = i - 1;


  if(i >= h){
    i = 0;
		if(h > 0)
			h--;
	}

  if(k > (NUMBER_OF_LEDS-1))
    k = NUMBER_OF_LEDS-1; //This means we are on the first LED and want the last to be clear.

  // Set the previous LED low.

  if(h != 0)
	 update_pwm_sequence_single_led(0, 0, 0, k);

	update_pwm_sequence_single_led(green, red, blue, i);

  // Make the static train increasingly larger in order to make the clock fill up gradually.

  for(uint8_t u = NUMBER_OF_LEDS; u >= h; u--)
  {
    update_pwm_sequence_single_led(green, red, blue, u);
  }

  led_start_pwm();

  if(h == 0)
    {
      i = 0;
      h = NUMBER_OF_LEDS;
    }

  i++;
}


void all_patterns(uint8_t green, uint8_t red, uint8_t blue, uint8_t length)
{
  static uint32_t i = 0;

  if(i >= (NUMBER_OF_LEDS*NUMBER_OF_LOOPS*NUMBER_OF_PATTERNS))
    {
      i = 0;
    }

  if(i < NUMBER_OF_LEDS*NUMBER_OF_LOOPS)
    {
      pattern_1(green, red, blue, length);
      i++;
    }
  else if(i < (NUMBER_OF_LEDS*NUMBER_OF_LOOPS)*2)
    {
      pattern_2(green, red, blue);
      i++;
    }
  else if(i < (NUMBER_OF_LEDS*NUMBER_OF_LOOPS)*3)
    {
      pattern_3(green, red, blue);
      i++;
    }
    else if(i < (NUMBER_OF_LEDS*NUMBER_OF_LOOPS)*5)
      {
        pattern_4(green, red, blue);
        i++;
      }
}


void led_event_handler(void)
{

  uint8_t green = pattern.green, red = pattern.red, blue = pattern.blue, length = pattern.length;

  switch(pattern.current_pattern){
    case 1:
      pattern_1(green, red, blue, length);
			break;
    case 2:
      pattern_2(green, red, blue);
			break;
    case 3:
      pattern_3(green, red, blue);
      break;
    case 4:
      pattern_4(green, red, blue);
      break;
    case 255:
      all_patterns(green, red, blue, length);
      break;
	}
}

void change_pattern(Pattern_t * new_pattern)
{
  pattern = *new_pattern;
}

void reset_all_led(void)
{
  for(uint8_t i = 0; i < NUMBER_OF_LEDS; i++)
  {
    update_pwm_sequence_single_led(0, 0, 0, i);
  }
  led_start_pwm();
}

void adr_led_init(void)
{
  pattern.current_pattern = DEFAULT_PATTERN;
  pattern.green = DEFAULT_GREEN_VALUE;
  pattern.red = DEFAULT_RED_VALUE;
  pattern.blue = DEFAULT_BLUE_VALUE;
  pattern.length = DEFAULT_LENGTH;
  pattern.speed = DEFAULT_SPEED;

	led_pwm_init();

	NRF_LOG_RAW_INFO("Adressable led patterns enabled. \n");
}

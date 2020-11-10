/**
 * @file
 * Analog to Digital Converter driver for LPC40xx
 *
 * @note
 * This driver is intentionally simplified, and is meant to be used as a reference.
 * Burst mode should be enabled for a better implementation
 */
#pragma once

#include <stdint.h>

// Only Channel2, Channel4 and Channel5 pins are avaible for use on SJ2 Development board
typedef enum {
  ADC__CHANNEL_2 = 2, // Pin Available on SJ2
  ADC__CHANNEL_4 = 4, // Pin Available on SJ2
  ADC__CHANNEL_5 = 5, // Pin Available on SJ2
} adc_channel_e;

/* Setup Control Reg + Read from Global Data Register */
void adc__initialize(void);
/* Setup Control Reg + Read from Global Data Register (Software Mode) */
uint16_t adc__get_adc_value(adc_channel_e channel_num);

// Enable Analog Mode + ADC function for I/O Pin P0_25 */
void pin_configure_adc_channel_as_io_pin();

/* Enable Burst Mode (Hardware Mode)  */
void adc__enable_burst_mode(adc_channel_e channel_num);
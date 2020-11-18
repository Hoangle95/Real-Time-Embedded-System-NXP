#pragma once

#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "gpio_lab.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {
  OFF = 0,
  ON = 1,
} ON_OFF;

//======================================================================================// // This configures what DMA
// channels the SSP2 driver utilizes for ssp2__dma_write_block() and ssp2__dma_read_block()
//======================================================================================//

// Initialize SPI (Power ON + Config controll Reg + Adjust SPI clock)
void ssp2__init(uint32_t max_clock_mhz);

// Exchange byte (READ/WRITE)
uint8_t ssp2_send_and_receive_read(uint8_t data_out);

// Config IO PIN SSP2 function
void ssp2_pin_configure();

void adesto_cs();
void adesto_ds();

// Enable Specific Slave
void ssp2_chip_select(uint8_t port, uint8_t pin, ON_OFF ONE_or_OFF);
void write_enable();
void write_disable();

// Extra Credit write page
void ssp2_write_page(uint32_t address, uint8_t data);

// Extra Credit read page
void ssp2_read_page(uint32_t address, uint8_t *result);

// Erase Page Flash Memory
void ssp2_erase_page(uint8_t address);

// Sending 24-Bits Address
void adesto_flash_send_address(uint32_t address);
void ssp2_write_byte(uint32_t address, uint8_t data);

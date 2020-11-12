#include "ssp2_lab.h"
#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdbool.h>
#include <stddef.h>

#include "ssp2.h"

#include "clock.h"
#include "gpio.h"
#include "gpio_lab.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdio.h>

/*==========================================SPI2 Pin Config()========================================
*@brief:  Config IO PIN SSP2 function
*@para:   No Parameter
*@return: No Return
*@Note:   Initialize Pin mode from IOControl Register (SPI2)
=====================================================================================================*/
void ssp2_pin_configure() {
  gpio__construct_with_function(GPIO__PORT_1, 0, GPIO__FUNCTION_4); // SCK2
  gpio__construct_with_function(GPIO__PORT_1, 1, GPIO__FUNCTION_4); // MOSI2
  gpio__construct_with_function(GPIO__PORT_1, 4, GPIO__FUNCTION_4); // MISO2
}
void adesto_cs() {
  GPIO__set_as_output(1, 10);
  GPIO__set_low(1, 10);
}

void adesto_ds() {
  GPIO__set_as_output(1, 10);
  GPIO__set_high(1, 10);
}
/*======================================SPI2 Init(max_clock_mhz)=====================================
*@brief:  Initialize SPI (Power ON + Config controll Reg + Adjust SPI clock)
*@para:   max_clock_mhz (in)
*@return: No return
*@Note:   Power On            ( LPC_SC->PCONP )
          Setup Crontrol Reg  ( LPC_SSP2->CR0 or CR1 )
          Setup Prescalar     ( LPC_SSP2->CPSR )
======================================================================================================*/
void ssp2__init(uint32_t max_clock_mhz) {
  max_clock_mhz = max_clock_mhz * 1000 * 1000; // Convert max_clock from mhz to Khz
  // a) Power on Peripheral
  // lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__SSP2); // Option 1
  LPC_SC->PCONP |= (1 << 20); // Option 2: SC is System and Clock control, POwer ON peripheral

  // b) Setup control registers CR0 and CR1
  LPC_SSP2->CR0 |= (7 << 0); // Option1 : Data size
  // LPC_SSP2->CRO = 0b0111; //Option 2
  // SCR = 0 because then
  LPC_SSP2->CR0 |= (0b00 << 4); // SPI mode

  LPC_SSP2->CR1 |= (1 << 1);
  // c) Setup prescalar register to be <= max_clock_mhz

  /*
  SPI maximum work at 24mhz. To make it capture logic analyzer, have to make it work up to 30mhz
  So I need to devide it down --> Ghi vo con CPU register, dung con clock chia ra
  1) 96 / 2 = 48 <= 24 NO
  2) 96 / 4 = 24 <= 24 YES --> break
  SPI frequency equation will be SPI_CLOCK = PCLK / CPSDVSR where PCLK is 96MHz
  The higher number CPSR, the more slow SPI,
  if SPI = 96MHz / 96 = 1MHz with LPC_SSP2->CPSR = 96;
  How fast is the SPI clock : f = 1Mhz -> 1us per clock
  3) 96 / 6 = 16 <= 4 NO
  4) 96 / 8 = 12 <= 4 NO
  5) 96 / 12 = 8 <= 4 NO
  6) 96 / 16 = 6 <= 4 NO
  7) 96 / 24 = 4 <= 4 YES --> Break
  Sumary : less max_clock_mhz -> the higher number divide -> less data capture --> more accurate
  96 / 6 = 16 Mhz which means SPI clock capture 625us per clock
  */
  const uint32_t cpu_clock_mhz = clock__get_core_clock_hz(); // 96MHz

  for (uint8_t divider = 2; divider <= 254; divider = divider + 2) {
    if ((cpu_clock_mhz / divider) <= max_clock_mhz) {
      fprintf(stderr, "divider: %d \n", divider);
      break;
    }
    LPC_SSP2->CPSR = divider;
  }
}
/*==================================ssp2_send_and_receive_read(data)=================================
*@brief:    Exchange byte (READ/WRITE)
*@para:     Data_transfer (in)
*@return:   Data from reg (out)
*@Note:     Send byte to read byte from Data Register
            Check Status Reg [4]
======================================================================================================*/
uint8_t ssp2_send_and_receive_read(uint8_t data) {
  // Configure the Data register(DR) to send and receive data by checking the SPI peripheral status register
  LPC_SSP2->DR = data; // send data
  while (1) {
    if (LPC_SSP2->SR & (1 << 4)) { // Busy mode
      printf("keep waiting\n");
      //;
    } else {
      break;
    }
  }
  return (uint8_t)(LPC_SSP2->DR & 0xFF); // receive data
}
/*==========================SPI2 adesto_flash_send_address (extra credit)=============================
*@brief:     Sending 24-Bits Address
*@para:      address (in)
*@return:    no address
*@Note:
======================================================================================================*/
void adesto_flash_send_address(uint32_t address) {
  (void)ssp2_send_and_receive_read((address >> 16) & 0xFF);
  (void)ssp2_send_and_receive_read((address >> 8) & 0xFF);
  (void)ssp2_send_and_receive_read((address >> 0) & 0xFF);
}

/*==================================SPI2 Chip_select (data exchange)==================================
*@brief:  Enable Specific Slave (CS PIN)
*@para:   Port, Pin, ON/OFF ( in )
*@return: No Return
*@Note:
======================================================================================================*/
void ssp2_chip_select(uint8_t port, uint8_t pin, ON_OFF ONE_or_OFF) {
  if (ONE_or_OFF == ON) {
    gpio__construct_as_output(port, pin);
    GPIO__set(port, pin, false);
  } else if (ONE_or_OFF == OFF) {
    gpio__construct_as_output(port, pin);
    GPIO__set(port, pin, true);
  }
}

void write_enable() {
  ssp2_chip_select(1, 10, ON);
  ssp2_send_and_receive_read(0x06); // OP-Code
  ssp2_chip_select(1, 10, OFF);
}

void write_disable() {
  ssp2_chip_select(1, 10, ON);
  ssp2_send_and_receive_read(0x04); // OP-Code
  ssp2_chip_select(1, 10, OFF);
}
/*===================================SPI2 write page (extra credit)==================================
*@brief:     Extra Credit write page
*@para:      address (in)
*@return:    no return
*@Note:      write 250 blocks test data
======================================================================================================*/
void ssp2_write_page(uint32_t address, uint8_t data) {
  // Enable "write_enable"
  // Step 1 : write enable
  write_enable();
  adesto_cs();
  printf("write page\n");
  ssp2_send_and_receive_read(0x02); // Op code
  adesto_flash_send_address(address);
  for (int i = 1; i <= 255; i++) {
    if (ssp2_send_and_receive_read(data)) {
      data++;
    }
  }
  adesto_ds();
  write_disable();
}
/*====================================SPI2 read page (extra credit)==================================
*@brief:     Extra Credit read page
*@para:      address (in)
             *data   (out)
*@return:    no return
*@Note:      read 250 blocks test data
======================================================================================================*/
void ssp2_read_page(uint32_t address, uint8_t *result) {
  adesto_cs();
  ssp2_send_and_receive_read(0x03); // OP code
  adesto_flash_send_address(address);
  for (int i = 0; i <= 255; i++) {
    result[i] = ssp2_send_and_receive_read(0x00);
  }
  adesto_ds();
  // return result;
}

void ssp2_erase_page(uint8_t address) {
  write_enable();
  adesto_cs();
  ssp2_send_and_receive_read(0x44);    // Op code
  ssp2_send_and_receive_read(address); // address
  ssp2_send_and_receive_read(0x56);    // dummy bytes
  ssp2_send_and_receive_read(0x34);    // dummy bytes
  adesto_ds();
  write_disable();
}

void ssp2_write_byte(uint32_t address, uint8_t data) {
  // Enable "write_enable"
  // Step 1 : Write enable
  write_enable();
  adesto_cs();
  {
    ssp2_send_and_receive_read(0x02);
    adesto_flash_send_address(address);
    ssp2_send_and_receive_read(data);
    adesto_ds();
    write_disable();
  }
  adesto_ds();
}

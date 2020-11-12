#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "gpio_lab.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "pwm1.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "ssp2.h"
#include "ssp2_lab.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

/*
==============================================Part_0===========================================
*/

#include <stdint.h>

void ssp2__init();
// Refer to LPC User manual and setup the register bits correctly
// a) Power on Peripheral
// b) Setup control registers CR0 and CR1
// c) Setup prescalar register to be <= max_clock_mhz

uint8_t ssp2__exchange_byte_lab(uint8_t data_out) {
  // Configure the Data register(DR) to send and receive data by checking the SPI peripheral status register
  LPC_SSP2->DR = data_out;
  // (1 << 4) is to ENABLE Busy mode , 0 if idle, 1 if sending/receiving data
  while (LPC_SSP2->SR & (1 << 4)) {
    fprintf(stderr, "exchange byte\n"); // Wait until SSP is busy
  }

  return (uint8_t)(LPC_SSP2->DR & 0xFF);
}

int main(void) {
  ssp2__init();
  puts("star RTOS");
  return 0;
}

/*
========================================Part_1============================================
*/

// TODO: Study the Adesto flash 'Manufacturer and Device ID' section
typedef struct {
  uint8_t manuf_id;
  uint8_t dev_id_1;
  uint8_t dev_id_2;
  uint8_t ext_dev_id;
} adesto_flash_id_s;

adesto_flash_id_s adesto_read_signature(void) {
  adesto_cs();
  // Send opcode and read bytes
  uint8_t manuf_id = ssp2_send_and_receive_read(0x9F);                                          // Op code
  uint8_t dev_id_1 = ssp2_send_and_receive_read(0x43);                                          // dummy bytes
  uint8_t dev_id_2 = ssp2_send_and_receive_read(0x54);                                          // dummy bytes
  uint8_t ext_dev_id = ssp2_send_and_receive_read(0x66);                                        // dummy bytes
  printf("signature bytes =  %02X %02X %02X %02X\n", manuf_id, dev_id_1, dev_id_2, ext_dev_id); // return data;
  adesto_ds();
}

// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it

void spi_task(void *p) {
  // Truely maximum is 6, if we change it w higher number 8, 12, 16--> capture wrong, why?
  // Sumary : less max_clock_mhz -> the higher number divide -> less data capture --> more accurate
  const uint32_t max_clock_mhz = 6;
  ssp2__init(max_clock_mhz);

  ssp2_pin_configure();
  while (1) {
    adesto_flash_id_s id = adesto_read_signature();
    // TODO: printf the members of the 'adesto_flash_id_s' struct
    vTaskDelay(500);
  }
}
int main(void) {
  puts("start RTOS");
  xTaskCreate(spi_task, "spi", 1024 / sizeof(void *), NULL, 1, NULL);
  vTaskStartScheduler();
  return 0;
}

/*
======================================================Part_2================================================
*/

#include "FreeRTOS.h"
#include "ssp2.h"
#include "task.h"

// static int *task1 = 1;
// static int *task2 = 2;
static SemaphoreHandle_t spi_bus_mutex;

// TODO: Study the Adesto flash 'Manufacturer and Device ID' section
typedef struct {
  uint8_t manuf_id;
  uint8_t dev_id_1;
  uint8_t dev_id_2;
  uint8_t ext_dev_id;
} adesto_flash_id_s;

// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it
adesto_flash_id_s adesto_read_signature(void) {
  adesto_cs();
  // Send opcode and read bytes
  uint32_t manuf_id = ssp2_send_and_receive_read(0x9F);
  uint8_t dev_id_1 = ssp2_send_and_receive_read(0xFF);
  uint8_t dev_id_2 = ssp2_send_and_receive_read(0xFF);
  uint8_t ext_dev_id = ssp2_send_and_receive_read(0xFF);
  adesto_ds();
  // return data;
  printf("signature bytes =  %02X %02X %02X %02X\n", manuf_id, dev_id_1, dev_id_2, ext_dev_id);
}
void spi_task(void *p) {
  // Truely maximum is 6, if we change it w higher number 8, 12, 16--> capture wrong, why?
  // Sumary : less max_clock_mhz -> the higher number divide -> less data capture --> more accurate
  const uint32_t max_clock_mhz = 6;
  ssp2__init(max_clock_mhz);

  ssp2_pin_configure();
  while (1) {
    adesto_flash_id_s id = adesto_read_signature();
    // TODO: printf the members of the 'adesto_flash_id_s' struct
    vTaskDelay(500);
  }
}
void Mutex_task(void *p) {
  // const int *task = (int *)(p);
  const uint32_t max_clock_mhz = 6;
  ssp2__init(max_clock_mhz);
  while (1) {
    if (xSemaphoreTake(spi_bus_mutex, 1000)) {
      const adesto_flash_id_s id = adesto_read_signature();
      // When we read a manufacturer ID we do not expect, we will kill this task
      if (id.manuf_id != 0x1F) {
        fprintf(stderr, "Manufacturer ID read failure\n");
        vTaskSuspend(NULL); // Kill this task
      }
      xSemaphoreGive(spi_bus_mutex);
    }
  }
}
int main(void) {
  spi_bus_mutex = xSemaphoreCreateMutex();
  puts("start RTOS");
  // xTaskCreate(spi_task, "spi", 1024 / sizeof(void *), NULL, 1, NULL);
  // TODO: Initialize your SPI, its pins, Adesto flash CS GPIO etc...

  xTaskCreate(Mutex_task, "task1", 1024, NULL, 1, NULL);
  xTaskCreate(Mutex_task, "task2", 1024, NULL, 1, NULL);
  vTaskStartScheduler();
  return 0;
}

/**
 * Adesto flash asks to send 24-bit address
 * We can use our usual uint32_t to store the address
 * and then transmit this address over the SPI driver
 * one byte at a time
 */
/*
==========================================Part_3=============================
*/

void task_extra_credit(void *p) {
  while (1) {
    uint8_t temp[256];
    // Erasing page 0
    printf("erasing page 0\n");
    ssp2_erase_page(0x000000);
    vTaskDelay(1000);
    // Read page 0 , it should be FF
    ssp2_read_page(0x000000, temp);
    for (int i = 0; i <= 255; i++) {
      printf("data at %d after erasing is: %x\n", i, temp[i]);
    }
    ssp2_write_page(0x000000, 0x00);
    vTaskDelay(1000);
    printf("\n\n");
    // Read page 0
    ssp2_read_page(0x000000, temp);
    for (int i = 0; i <= 255; i++) {
      printf("data at %d after writing is: %x \n", i, temp[i]);
    }
  }
}

int main(void) {
  puts("start RTOS");
  xTaskCreate(task_extra_credit, "extra_credit", 2048 / 1, NULL, 1, NULL);
  vTaskStartScheduler();
  return 0;
}

/*
static void task_extra_credit(void *p);
void task_extra_credit(void *p) {
  const uint32_t spi_clock_mhz = 1;
  ssp2__init(spi_clock_mhz);

  ssp2_pin_configure();

  uint8_t temp[250];
  uint8_t temp2[250];
  // ssp2__write_page(0x000200);

  while (1) {
    ssp2_erase_page(0x00);
    delay__ms(100);

    fprintf(stderr, "Page 0x000 1000 \n");
    ssp2_read_page(0x000100, temp);
    for (int i = 0; i <= 250; i++) {
      fprintf(stderr, "Read Data: %X \n", temp[i]);
    }
    vTaskDelay(1000);
    fprintf(stderr, "\n");
    fprintf(stderr, "Page 0x000 2000 \n");
    ssp2_read_page(0x000000, temp2);
    for (int i = 0; i <= 250; i++) {
      fprintf(stderr, "Read Data: %X \n", temp2[i]);
    }
    vTaskSuspend(NULL);
  }
}
*/

/*
void task_extra_credit(void *p) {
  while (1) {
    printf("erasing page 0\n");
    ssp2_erase_page(0x00);
    vTaskDelay(1000);
    // Read page 0 , it should be FF
    ssp2_write_byte(0x000207, 0x34);
    printf("write data\n");
    ssp2_write_byte(0x000207, 0x45);
    printf("write data\n");
  }
  printf("read data at 0x207: %x\n");
  ssp2_read_byte(0x000207);
}
*/

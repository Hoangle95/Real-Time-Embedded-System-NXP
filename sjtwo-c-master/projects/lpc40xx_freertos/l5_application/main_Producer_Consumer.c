#include "FreeRTOS.h"
#include "board_io.h"
#include "cli_handlers.h"
#include "clock.h"
#include "gpio.h"
#include "gpio_lab.h"
#include "periodic_scheduler.h"
#include "portmacro.h"
#include "queue.h"
#include "sj2_cli.h"
#include "task.h"
#include "uart.h"
#include "uart_lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static QueueHandle_t switch_queue;

typedef enum { switch__off, switch__on } switch_e;
/*
get_switch_input_from_switch0() {
  gpio__construct_with_function(0, 30, GPIO__FUNCITON_0_IO_PIN);
  GPIO__set_as_input(0, 30);
  return GPIO__get_level(0, 30);
}
*/
/*
switch_e get_switch_input_from_switch0() {
  gpio_s button = gpio__construct(GPIO__PORT_1, 19);
  return gpio__get(button);
*/

get_switch_input_from_switch0() {
  GPIO__set_as_input(1, 19);
  if (LPC_GPIO1->PIN & (1 << 19)) {
    return 1;
  } else {
    return 0;
  }
}
void producer(void *p) {
  while (1) {
    const switch_e switch_value = get_switch_input_from_switch0();
    printf("switch: %d \n", switch_value);
    printf("1\n");
    if (xQueueSend(switch_queue, &switch_value, 0)) {
      printf("2\n");
    }
    vTaskDelay(1000);
  }
}
void consumer(void *p) {
  switch_e switch_value;
  while (1) {
    printf("3\n");
    if (xQueueReceive(switch_queue, &switch_value, portMAX_DELAY)) {
      printf("4\n");
    }
    vTaskDelay(1000);
  }
}

void task(void) {
  GPIO__set_as_output(1, 24);
  while (1) {
    GPIO__set_high(1, 24);
    vTaskDelay(1000);
    GPIO__set_low(1, 24);
    vTaskDelay(1000);
  }
}

int main(void) {
  sj2_cli__init();
  puts("start RTOS");
  xTaskCreate(producer, "producer", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(consumer, "consumer", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  switch_queue = xQueueCreate(1, sizeof(switch_e));
  // xTaskCreate(task, "led", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();
  return 0;
}

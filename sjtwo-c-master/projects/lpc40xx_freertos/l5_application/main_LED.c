#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "gpio_lab.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

//======================================================================================//
//                                Part_0                                                //
// Goal: Part_0 is blink LED task on LED_3
//======================================================================================//

void led_task(void *pvParameters) {
  // Choose one of the onboard LEDS by looking into schematics and write code for the below

  LPC_IOCON->P1_18 &= ~(0b111); //0) Set the IOCON MUX function select pins to 000
  LPC_GPIO1->DIR |= (1 << 18); //1) Set the DIR register bit for the LED port pin
  while (true) {
    vTaskDelay(500);
    LPC_GPIO1->PIN &= ~(1 << 18); //2) Set PIN register bit to 0 to turn ON LED (led may be active low)
    vTaskDelay(500);
    LPC_GPIO1->PIN |= (1 << 18); //3) Set PIN register bit to 1 to turn OFF LED
  }
}

int main(void) {
  // Create FreeRTOS LED task
  xTaskCreate(led_task, "led", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
  return 0;
}

//======================================================================================//
//                                Part_2                                                //
// Goal : Press switch 0 to start blink the LED from left to Right
//======================================================================================//

static SemaphoreHandle_t switch_press_indication;

typedef struct {

  uint8_t port;
  uint8_t pin;
} port_pin_s;

int port_arr[2] = {1, 2};
int pin_arr[4] = {18, 24, 26, 3};

void led_task(void *task_parameter) {

  // Type-cast the parameter
  const port_pin_s *led = (port_pin_s *)(task_parameter);
  GPIO__set_as_output(led->port, led->pin);

  while (true) {
    if (xSemaphoreTake(switch_press_indication, 1000)) {
      while (true) {
        int port_array[] = {1, 2};
        int pin_array[] = {18, 24, 26, 3};
        for (int i = 0; i <= 1; i++) {
          for (int j = 0; j <= 3; j++) {
            if (i == 1) {
              j = 3;
            }
            GPIO__set_high(port_array[i], pin_array[j]);
            vTaskDelay(100);

            GPIO__set_low(port_array[i], pin_array[j]);
            vTaskDelay(100);
          }
        }
      }

    } else {
      puts("No switch press indication for 1000ms, press switch 0 to start \n");
    }
  }
}
void switch_task(void *task_parameter) {
  const port_pin_s *SW = (port_pin_s *)(task_parameter);
  GPIO__set_as_input(SW->port, SW->pin);
  while (true) {
    // TODO : Of switch pressed , set the binary semaphore
    if (GPIO__get_level(SW->port, SW->pin)) {
      xSemaphoreGive(switch_press_indication);
    }
    vTaskDelay(100);
  }
}

int main(void) {
  switch_press_indication = xSemaphoreCreateBinary();
  static port_pin_s SW = {1, 19};
  static port_pin_s led = {{1, 2}, {18, 24, 26, 3}};
  // static port_pin_s led = {{1, 18}, {1, 24}, {1, 26}, {2, 3}};
  // static port_pin_s led = {2, 3};

  xTaskCreate(led_task, "led_task", 2048 / sizeof(void *), &led, 1, NULL);
  xTaskCreate(switch_task, "switch_task", 2048 / sizeof(void *), &SW, 1, NULL);
  vTaskStartScheduler();
  return 0;
}

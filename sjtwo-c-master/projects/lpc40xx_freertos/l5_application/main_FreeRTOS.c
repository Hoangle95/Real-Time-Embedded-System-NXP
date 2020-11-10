#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

// Declare proto type
static void task_one(void *task_parameter);
static void task_two(void *task_parameter);

static void task_one(void *task_parameter) {
  while (true) {
    fprintf(stderr, "AAAAAAAAAAAA");
    // Sleep for 100ms
    vTaskDelay(100);
  }
}

static void task_two(void *task_parameter) {
  while (true) {
    fprintf(stderr, "bbbbbbbbbbbb");
    vTaskDelay(100);
  }
}

// int main(void) {

//   // function to declare and assign pointer task_parameter
//   //  void *task_parameter;
//   //  xTaskCreate(task_one, "task_one", 4096 / sizeof(void *), task_parameter, PRIORITY_HIGH, NULL);

//   xTaskCreate(task_one, "task_one", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
//   xTaskCreate(task_two, "task_two", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
//   // function to check size of sizeof(void*) = 1
//   // 4096/1 = 4096Kb
//   printf("%d", sizeof(void *));
//   puts("Starting RTOS");
//   // This function to run the program
//   vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

//   return 0;
// }
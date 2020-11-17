#include "FreeRTOS.h"
#include "adc.h"
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
#include "task.h"
#include <stdio.h>

// ======================================================================================//
//                                Part_0                                                //
// Goal : PWM task to light up LED from Red -> Green -> Blue
// 1. Create pwm_task
// 2. Declare configure pwm in pwm.h
//   define configure pwm in pwm.c
//   use configure pwm in ham main
// 3. use pwm1__set_duty_cycle
// ======================================================================================//

// // static void pin_configure_pwm_channel_as_io_pin(void);
// void pwm_task(void *p) {
//   // PWM run with 1000Hz
//   pwm1__init_single_edge(1000);
//   // Locate a GPIO pin that a PWM channel will control
//   // NOTE You can use gpio__construct_with_function() API from gpio.h
//   // PWM configure led red, green, blue
//   pin_configure_pwm_channel_as_io_pin(2, 1, GPIO__FUNCTION_1); // control red
//   pin_configure_pwm_channel_as_io_pin(2, 0, GPIO__FUNCTION_1); // comtrol green
//   pin_configure_pwm_channel_as_io_pin(2, 2, GPIO__FUNCTION_1); // control blue
//   // This is the way to set directly _ option 2
//   // LPC_IOCON->P2_1 &= ~0b111; // Check pin is available, reset pin to 0000
//   // LPC_IOCON->P2_1 |= 0b001;  // Set pin 2_1 to use PWM at PWM1[3] channel 3
//   // Continue to vary the duty cycle in the loop
//   uint8_t percent_on = 75;
//   pwm1__set_duty_cycle(PWM1__2_1, percent_on);
//   vTaskDelay(500);
//   pwm1__set_duty_cycle(PWM1__2_0, percent_on);
//   vTaskDelay(500);
//   pwm1__set_duty_cycle(PWM1__2_2, percent_on);
//   vTaskDelay(500);
//   while (1) {
//     pwm1__set_duty_cycle(PWM1__2_0, percent_on - 50);
//     vTaskDelay(250);
//     if (++percent_on > 100) {
//       percent_on = 0;
//     }
//     vTaskDelay(100);
//   }
// }
// int main(void) {
//   puts("Start RTOS");
//   xTaskCreate(pwm_task, "pwm_task", 1048, NULL, 1, NULL);
//   vTaskStartScheduler();
//   return 0;
// }

// ======================================================================================//
//                                Part_1                                               //
// Goal : Check the Voltage Value on ADC telemetry
// 1. Enable Burst_mode to allow
// ======================================================================================//

// void adc_task(void *p) {
//   adc__initialize();
//   // TODO This is the function you need to add to adc.h
//   // You can configure burst mode for just the channel you are using
//   adc__enable_burst_mode(ADC__CHANNEL_2);
//   // Configure a pin, such as P1.31 with FUNC 011 to route this pin as ADC channel 5
//   // You can use gpio__construct_with_function() API from gpio.h
//   pin_configure_adc_channel_as_io_pin(); // TODO You need to write this function
//   while (1) {
//     // Get the ADC reading using a new routine you created to read an ADC burst reading
//     // TODO: You need to write the implementation of this function
//     const uint16_t adc_value = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_2);
//     // missing double will cause 0V
//     fprintf(stderr, "Check value: %f V \n", ((double)adc_value / 4095) * 3.3); // suck at programming

//     vTaskDelay(100);
//   }
// }

// int main(void) {
//   xTaskCreate(adc_task, "adc_task", 1048, NULL, 1, NULL);
//   vTaskStartScheduler();
// }

// ======================================================================================//
//                                Part_2                                               //
// Goal : Turn the switch and LED will light up or Light off

// ======================================================================================//
// #include "FreeRTOS.h"
// #include "adc.h"
// #include "queue.h"
// #include "task.h"
// // This is the queue handle we will need for the xQueue Send/Receive API
// static QueueHandle_t adc_to_pwm_task_queue;
// void adc_task(void *p) {
//   // NOTE: Reuse the code from Part 1
//   adc__initialize();
//   adc__enable_burst_mode(ADC__CHANNEL_2);
//   pin_configure_adc_channel_as_io_pin();
//   //  int adc_reading = 0; // Note that this 'adc_reading' is not the same variable as the one from adc_task
//   while (1) {
//     // Implement code to send potentiometer value on the queue
//     // a) read ADC input to 'int adc_reading'
//     const uint16_t adc_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_2);
//     // b) Send to queue: xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
//     xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
//     printf("Queue Send : %f V\n", ((double)adc_reading / 4095) * 3.3);
//     vTaskDelay(250);
//   }
// }
// void pwm_task(void *p) {
//   // NOTE: Reuse the code from Part 0
//   pwm1__init_single_edge(1000);
//   pin_configure_pwm_channel_as_io_pin(2, 1, GPIO__FUNCTION_1); // control red
//   pin_configure_pwm_channel_as_io_pin(2, 0, GPIO__FUNCTION_1); // control green
//   pin_configure_pwm_channel_as_io_pin(2, 2, GPIO__FUNCTION_1); // control blue

//   int percent = 75;
//   uint16_t adc_reading = 0;
//   while (1) {
//     uint32_t match0 = LPC_PWM1->MR0 & 0xFFFFFFFF;
//     uint32_t match1 = LPC_PWM1->MR1 & 0xFFFFFFFF;
//     fprintf(stderr, "MR0: %d MR1: %d \n", match0, match1);
//     // Implement code to receive potentiometer value from queue
//     if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 1000)) {
//       printf("Queue receive: %d\n", adc_reading);
//       percent = (adc_reading * 100) / 4095;
//       printf("percent of LED: %d\n", percent);
//       pwm1__set_duty_cycle(PWM1__2_1, percent);
//       pwm1__set_duty_cycle(PWM1__2_0, percent - 25);
//       pwm1__set_duty_cycle(PWM1__2_2, percent - 50);
//     }
//     // We do not need task delay because our queue API will put task to sleep when there is no data in the queue
//     // vTaskDelay(100);
//   }
// }
// int main(void) {
//   // Queue will only hold 1 integer
//   adc_to_pwm_task_queue = xQueueCreate(1, sizeof(uint16_t));

//   xTaskCreate(adc_task, "adc_task", 1024, NULL, 1, NULL);
//   xTaskCreate(pwm_task, "pwm_task", 1024, NULL, 1, NULL);
//   vTaskStartScheduler();
// }

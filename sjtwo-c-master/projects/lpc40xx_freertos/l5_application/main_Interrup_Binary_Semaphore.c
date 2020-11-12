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
#include "semphr.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

//======================================================================================//
//                                Part_0 - Option 1                                     //
// Goal : Press Button to trigger Interrupt by using Interrupt_vector table
// 1. Set up pin INPUT
// 2. Enable Pin Falling Edge
// 3. Enable GPIO interrupt vector table
//======================================================================================//

// Step 1:
int main(void) {
  // choose SW2 (P0_30) pin and configure as input (port 0 and port 2 has interrupt only)
  // Warning: P0.30, and P0.31 require pull-down resistors
  GPIO__set_as_input(0, 30);
  // b) Configure the registers to trigger Port0 interrupt (such as falling edge)
  // Pull Down Register
  LPC_IOCON->P0_30 &= ~(3 << 3);
  LPC_IOCON->P0_30 |= (1 << 3);
  LPC_GPIOINT->IO0IntEnF |= (1 << 29); // switch ON, trigger interrupt,enable interrupt P0.29
  NVIC_EnableIRQ(GPIO_IRQn);
  while (1) {
    puts("run");
    delay__ms(100);

    // TODO: Toggle an LED here
    GPIO__set_high(1, 18);
    delay__ms(100);
    GPIO__set_low(1, 18);
    delay__ms(100);
  }
}
// Step 2: Go to interrupt_vector_table.c

//======================================================================================//
//                                Part_0 - Option 2                                     //
// Goal : Press Button to trigger Interrupt by using API lpc_peripheral__enable_interrupt()
//======================================================================================//

// Step 1:
static void gpio_interrupt(void);

void gpio_interrupt(void) {
  // a) Use fprintf(stderr) or blink and LED here to test your ISR
  GPIO__set_high(1, 26);
  delay__ms(100);
  GPIO__set_low(1, 26);
  delay__ms(100);
  fprintf(stderr, "Interruptttttttt\n");
  // b) Clear Port0/2 interrupt using CLR0 or CLR2 registers
  LPC_GPIOINT->IO0IntClr |= (1 << 29); // switch off, clear interrupt
}

int main(void) {
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio_interrupt, NULL);
  LPC_GPIOINT->IO0IntEnF |= (1 << 29); // switch ON, trigger interrupt
  NVIC_EnableIRQ(GPIO_IRQn);
  GPIO__set_as_input(0, 30);
  while (1) {
    puts("run");
    delay__ms(100);
    GPIO__set_high(1, 18);
    delay__ms(100);
    GPIO__set_low(1, 18);
    delay__ms(100);
  }
}

//======================================================================================//
//                                Part_1 -                                              //
// Goal : Press Button to trigger Binary Semaphore Interrupt
// 1. Create Semaphore GIVE function
//  a/ unblock interrupt, clear trigger when give out semaphore
// 2. Create Semaphore TAKE function
//  a/ Make sure to put function TAKE in a while loop
// 3. Create function Main
//  a/ lpc_peripheral__enable_interrupt to call function gpio_interrup ( or ISR)
//======================================================================================//

static SemaphoreHandle_t switch_pressed_signal;
void sleep_on_sem_task(void *p);

// Semaphore GIVE
// This is also known as ISR
void gpio_interrupt(void *p) {
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
  LPC_GPIOINT->IO0IntClr |= (1 << 29); // unblock interrupt, clear trigger
  fprintf(stderr, "Interruptttttttt\n");
}
// Semaphore TAKE
void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTake(switch_pressed_signal, 1000)) {
      GPIO__set_high(1, 18);
      delay__ms(100);
      GPIO__set_low(1, 18);
      delay__ms(100);
      fprintf(stderr, "run\n");
    }
  }
}
int main(void) {
  // The way to call function gpio_interrupt
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio_interrupt, NULL);
  // Enable interrupt, create trigger on pin 0.29
  GPIO__set_as_input(0, 30);
  LPC_IOCON->P2_3 &= ~(0b0111);
  LPC_GPIOINT->IO0IntEnF |= (1 << 29); // TODO: Setup interrupt by re-using code from Part 0
  NVIC_EnableIRQ(GPIO_IRQn);           // Enable interrupt gate for the GPIO
  // Create your binary semaphore, semaphore create empty when you create it
  switch_pressed_signal = xSemaphoreCreateBinary();
  xTaskCreate(sleep_on_sem_task, "sem", 2048 / sizeof(void *), NULL, 1, NULL);    // Create the task
  vTaskStartScheduler();      // Start scheduler
  return (0);
}

//======================================================================================//
//                                Part_2 - Option 1                                     //
// Goal : Press Button to trigger Binary Semaphore Interrupt
// Goal : Press Second button to trigger Another Binary Semaphore
// This part 2 contains 2 files : main.c and gpio_isr.c
// In main.c
// 1. Create 2 ISR interrupt : gpio_interrupt1 , gpio_interrupt2
//======================================================================================//

static SemaphoreHandle_t switch_pressed_signal;

void sleep_on_sem_task(void *p);

void lpc_peripheral__halt_handler(void) {}

void gpio_interrupt1(void) {
  GPIO__set_high(1, 26);
  delay__ms(300);
  GPIO__set_low(1, 26);
  delay__ms(300);
  fprintf(stderr, "\nTurn ON OFF LED\n");
}
void gpio_interrupt2(void) {
  fprintf(stderr, "Blink LED\n");
  static port_pin_s array[4] = {{1, 18}, {1, 24}, {1, 26}, {2, 3}};
  for (int i = 0; i <= 4; i++) {
    GPIO__set_high(array[i].port, array[i].pin);
    delay__ms(100);
    GPIO__set_low(array[i].port, array[i].pin);
    delay__ms(100);
  }
}
// Semaphore GIVE
// This is also known as ISR
void gpio_interrupt(void *p) {
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
  LPC_GPIOINT->IO0IntClr |= (1 << 29); // Block interrupt, clear trigger
  fprintf(stderr, "Interruptttttttt\n");
}
// Semaphore TAKE
void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTake(switch_pressed_signal, 1000)) {
      GPIO__set_high(1, 18);
      delay__ms(100);
      GPIO__set_low(1, 18);
      delay__ms(100);
      fprintf(stderr, "run\n");
    }
  }
}
int main(void) {
  // This line to call the function gpio0_interrupt_dispatcher
  puts(" Starting RTOS");
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio0__interrupt_dispatcher, NULL);

  // Create your binary semaphore, semaphore create empty when you create it
  switch_pressed_signal = xSemaphoreCreateBinary();
  // Create the task
  xTaskCreate(sleep_on_sem_task, "sem", 2048 / sizeof(void *), NULL, 1, NULL);
  // Create 2 task interrupt
  gpio0__attach_interrupt(29, 0, gpio_interrupt);  // Falling edge
  gpio0__attach_interrupt(30, 1, gpio_interrupt2); // Rising edge

  return 0;
}

//======================================================================================//
//                                Part_2 - Option 2                                     //
// Goal : Press Button to trigger Binary Semaphore Interrupt
//
//======================================================================================//

static SemaphoreHandle_t switch_pressed_signal;
static SemaphoreHandle_t switch_press_condition;
void sleep_on_sem_task(void *p);
void lpc_peripheral__halt_handler(void) {}
// Semaphore GIVE
// This is also known as ISR
void gpio_interrupt(void *p) {
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
  LPC_GPIOINT->IO0IntClr |= (1 << 29); // Block interrupt, clear trigger
  fprintf(stderr, "Interrupt_1\n");
}
void gpio_interrupt2(void *p) {
  xSemaphoreGiveFromISR(switch_press_condition, NULL);
  LPC_GPIOINT->IO0IntClr |= (1 << 30); // Block interrupt, clear trigger
  fprintf(stderr, "Interrupt_2\n");
}
// Semaphore TAKE
void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the signal
    if (xSemaphoreTake(switch_pressed_signal, 1000)) {
      GPIO__set_high(1, 26);
      delay__ms(100);
      GPIO__set_low(1, 26);
      delay__ms(100);
      fprintf(stderr, "Turn ON OFF LED\n");
    } else if (xSemaphoreTake(switch_press_condition, 1000)) {
      fprintf(stderr, "Blink LED 1 2 3 4\n");
      static port_pin_s array[4] = {{1, 18}, {1, 24}, {1, 26}, {2, 3}};
      for (int i = 0; i <= 4; i++) {
        GPIO__set_high(array[i].port, array[i].pin);
        delay__ms(100);
        GPIO__set_low(array[i].port, array[i].pin);
        delay__ms(100);
      }
    } else {
      fprintf(stderr, "Waiting for interrupt\n");
    }
  }
}
int main(void) {
  puts(" Starting RTOS");
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio0__interrupt_dispatcher, NULL);

  switch_pressed_signal = xSemaphoreCreateBinary();
  switch_press_condition = xSemaphoreCreateBinary();
  // Create the task
  xTaskCreate(sleep_on_sem_task, "sem", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  // Create 2 task interrupt
  gpio0__attach_interrupt(29, 0, gpio_interrupt);  // Falling edge
  gpio0__attach_interrupt(30, 1, gpio_interrupt2); // Rising edge
  // Start scheduler
  vTaskStartScheduler();
  return (0);
}

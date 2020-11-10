#include "FreeRTOS.h"
#include "clock.h"
#include "gpio.h"
#include "gpio_lab.h"
#include "queue.h"
#include "task.h"
#include "uart.h"
#include "uart_lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
=====================================PART_1 ===============================================
FILE TO WORK ON : main.c / uart_lab.c / uart_lab.h
*/
/* -------------------------------------------------------------------------- */
/*                          TASK1: DECLARATION                          */
/* -------------------------------------------------------------------------- */
void uart_write_task(void *p);
void uart_read_task(void *p);
/* -------------------------------------------------------------------------- */
/*                          TASK 1: READ FUNCTION                          */
/* -------------------------------------------------------------------------- */
void uart_read_task(void *p) {
  while (1) {
    char input;
    uart_lab__polled_get(UART_2, &input);
    printf("Read data: %c\n", input);
    vTaskDelay(500);
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 1: WRITE FUNCTION                          */
/* -------------------------------------------------------------------------- */
void uart_write_task(void *p) {
  while (1) {
    // TODO: Use uart_lab__polled_put() function and send a value
    uart_lab__polled_put(UART_2, 'U');
    uart_lab__polled_put(UART_2, 'A');
    uart_lab__polled_put(UART_2, 'R');
    uart_lab__polled_put(UART_2, 'T');
    uart_lab__polled_put(UART_2, '\n');
    vTaskDelay(100);
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 1: MAIN FUNCTION                         */
/* -------------------------------------------------------------------------- */
void main_1() {
  // TODO: Use uart_lab__init() function and initialize UART2 or UART3 (your choice)
  // TODO: Pin Configure IO pins to perform UART2/UART3 function
  // uart_lab__init(0, clock__get_peripheral_clock_hz(), 38400);
  uart_lab__init(UART_2, 96000000, 115200);
  uart_pin_configure(UART_2);

  xTaskCreate(uart_read_task, "read", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(uart_write_task, "write", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();
}
/*
=================================================PART_2======================================
*/
/* -------------------------------------------------------------------------- */
/*                          TASK 2: DECLARATION                        */
/* -------------------------------------------------------------------------- */
// Private queue handle of our uart_lab.c

static QueueHandle_t your_uart_rx_queue; // 1. Tao ra cai Queue

static void uart2_interrupt(void); // 2. Initialize when Interrupt trigger
static void uart3_interrupt(void); // 2. Initialize when Interrupt trigger

void uart_read_task_fromISR(void *p); // 3. Function to read from Queue by Using Pointer
void uart__enable_interrupt(uart_number_e uart);

bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout);

// void uart_read_task2_fromISR(void *p);
/* -------------------------------------------------------------------------- */
/*                          TASK 2: INITIALIZE INTERRUPT                       */
/* -------------------------------------------------------------------------- */
void uart__enable_interrupt(uart_number_e uart) {

  if (uart == UART_2) {
    NVIC_EnableIRQ(UART2_IRQn);
    lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART2, uart2_interrupt, NULL);
    LPC_UART2->LCR &= ~(1 << 7);                        // Set DLab
    LPC_UART2->IER |= (1 << 0);                         // Enable RBR interupt set bit 0 (table 397)
    your_uart_rx_queue = xQueueCreate(5, sizeof(char)); // Create your RX queue
  } else if (uart == UART_3) {
    NVIC_EnableIRQ(UART3_IRQn);
    lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART3, uart3_interrupt, NULL);
    LPC_UART3->LCR &= ~(1 << 7);                        // Set DLab
    LPC_UART3->IER |= (1 << 0);                         // Enable RBR interupt set bit 0 (table 397)
    your_uart_rx_queue = xQueueCreate(5, sizeof(char)); // Create your RX queue
  } else {
    printf("no pin available");
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 2: UART_2 INTERRUPT FUNCTION                       */
/* -------------------------------------------------------------------------- */
static void uart2_interrupt(void) {
  // TODO: IIR register => why you got interrupted
  // 1 option : Interrupt by THR (Write)
  // 2 option : Interrupt by RBR (Read) => Chosen
  if (LPC_UART2->IIR & (0x02 << 1)) // Check data if Receive data available (table 398)
  {
    while (!(LPC_UART2->LSR & (1 << 0))) // Based on IIR status, Check status if Receive data ready is not ready
    {
      ;
    }
    const char byte = LPC_UART2->RBR & 0xFF; // Based on LSR status, read the RBR register buffer --> RX Queue
    xQueueSendFromISR(your_uart_rx_queue, &byte, NULL); // Put address of byte into Queue
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 3: UART_3 INTERRUPT FUNCTION                       */
/* -------------------------------------------------------------------------- */
static void uart3_interrupt(void) {
  // TODO: IIR register => why you got interrupted
  // 1 option : Interrupt by THR (Write)
  // 2 option : Interrupt by RBR (Read) => Chosen
  // IIR : Interrupt ID register: nhan dien interrupt toi tu dau
  // LSR : Line Status Register
  if (LPC_UART3->IIR & (0x02 << 1)) // Check data if Receive data available (table 398)
  {
    while (!(LPC_UART3->LSR & (1 << 0))) // Based on IIR status, Check status if Receive data ready is not ready
    {
      ;
    }
    const char byte = LPC_UART3->RBR & 0xFF; // Based on LSR status, read the RBR register buffer --> RX Queue
    xQueueSendFromISR(your_uart_rx_queue, &byte, NULL); // Put address of byte into Queue
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 2 : QUEUE RECEIVE DATA                        */
/* -------------------------------------------------------------------------- */
// 1. function to get a char from the queue
// 2. Create 1 pointer to get data from queue because in queue, we cannot push data out but USING POINTER
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
  return xQueueReceive(your_uart_rx_queue, input_byte, timeout);
}
/* -------------------------------------------------------------------------- */
/*                          TASK 2: WRITE FUNCTION                       */
/* -------------------------------------------------------------------------- */
void uart_write_task2(void *p) {
  while (1) {
    uart_lab__polled_put(UART_3, '1');
    uart_lab__polled_put(UART_3, '2');
    uart_lab__polled_put(UART_3, '3');
    uart_lab__polled_put(UART_3, '4');
    uart_lab__polled_put(UART_3, '5');
    vTaskDelay(500);
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 2: READ FUNCTION                       */
/* -------------------------------------------------------------------------- */
void uart_read_task_fromISR(void *p) {
  while (1) {
    char *input;
    if (uart_lab__get_char_from_queue(&input, 100)) { // Using pointer to get address of input from Queue
      printf("Queue received: %c\n", input);
    } else {
      printf("Waiting for Signal in 100ms\n"); // if we want to see no writing data to the queue behavior
    }
    // vTaskDelay(500); // No vTaskDelay because we want to MAKE SURE FUNCTION READ ALWAYS READ DATA WHEN AVAILABLE
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 2: MAIN FUNCTION                       */
/* -------------------------------------------------------------------------- */
void main_2() {
  // uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate)
  uart_lab__init(UART_3, 96000000, 115200);
  uart_pin_configure(UART_3);
  uart__enable_interrupt(UART_3);

  xTaskCreate(uart_read_task_fromISR, "read", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(uart_write_task2, "write", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  vTaskStartScheduler();
}
/*
====================================Part_3 (Interface between 2 boards)=================================
*/
/* -------------------------------------------------------------------------- */
/*                          TASK 3: DECLARATION                      */
/* -------------------------------------------------------------------------- */
void board_1_sender_task(void *p);
void board_2_receiver_task(void *p);
/* -------------------------------------------------------------------------- */
/*                          TASK 3: INTERRUPT FUNCTION                       */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                          TASK 3 : BOARD 1 SENDER                        */
/*
/* -------------------------------------------------------------------------- */

// This task is done for you, but you should understand what this code is doing
void board_1_sender_task(void *p) {
  char number_as_string[16] = {0}; // Create array of 16 slots
  while (true) {
    // divided for 1000 to get 3 digit number
    const int number = rand() % 100;         // number run a random function which usually has 10 digit number
    sprintf(number_as_string, "%i", number); // Assign value of number into a string Using sprintf
    // Send one char at a time to the other board including terminating NULL char
    for (int i = 0; i <= strlen(number_as_string); i++) {
      uart_lab__polled_put(UART_3, number_as_string[i]); // Write bytes to THR register
    }
    printf("Sent Random #: %i over UART to other board\n", number);
    vTaskDelay(3000);
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 3 : BOARD 2 RECEIVER                        */
/* -------------------------------------------------------------------------- */
void board_2_receiver_task(void *p) {
  char number_as_string[16] = {0}; // a array of 16 slots
  int counter = 0;

  while (true) {
    char character = 0; // create a pointer to grab bytes from QUeue
    uart_lab__get_char_from_queue(&character, portMAX_DELAY);
    printf("Received: %c\n", character);
    if ('\0' == character) // '\0' is function end of line/string
    {
      number_as_string[counter] = '\0';
      counter = 0;
      printf("Received this number from UART 3: %s\n\n\n", number_as_string);
      memset(number_as_string, 0, 16);

    } else // if it not end of line, keep loading value into array.
    {
      number_as_string[counter] = character;
      printf("loading Array: %s\n", number_as_string);
      counter++;
    }
  }
}
/* -------------------------------------------------------------------------- */
/*                          TASK 3 : MAIN FUNCTION                        */
/* -------------------------------------------------------------------------- */
void main_3() {
  uart_lab__init(UART_3, 96000000, 115200);
  uart_lab__init(UART_2, 96000000, 115200);
  uart2_3_pin_configure();

  uart__enable_interrupt(UART_2);
  // uart__enable_interrupt(UART_3);

  xTaskCreate(board_2_receiver_task, "read", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(board_1_sender_task, "write", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  vTaskStartScheduler();
}
/* -------------------------------------------------------------------------- */
/*                          CPU : MAIN PROGRAM                       */
/* -------------------------------------------------------------------------- */
// int main(void) {
//   // main_1();
//   // main_2();
//   main_3();
// }

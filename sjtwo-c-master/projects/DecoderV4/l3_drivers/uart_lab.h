#pragma once

#include "lpc_peripherals.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  UART_2,
  UART_3,
  UART_0,
  UART_1,
} uart_number_e;

// Initialize ( UART Port + CLOCK + Baud Rate )
void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate);

// Receive Input data from RX line
bool uart_lab__polled_get(uart_number_e uart, char *input_byte);

// Transmit Output data from TX line
bool uart_lab__polled_put(uart_number_e uart, char output_byte);

void uart_pin_configure(uart_number_e uart);

void uart2_3_pin_configure();

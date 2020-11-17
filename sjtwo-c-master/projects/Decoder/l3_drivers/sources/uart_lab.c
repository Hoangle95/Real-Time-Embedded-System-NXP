#include "uart_lab.h"
#include "gpio.h"
#include "lpc40xx.h"
#include <stdio.h>
/* -------------------------------------------------------------------------- */
/*                           INITIALIZE UART2/UART3                        */
/* -------------------------------------------------------------------------- */
// Set up DLAB to enable access for DLL and DLM
// LPC_UART2->LCR |= (1 << 7); // DLAB : enable DLAB to 1 to access DLM, DLL side
// divider : 0x 0000 0000 1111 1111
// DLM : 0000 0000
// DLL : 1111 1111
void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
  if (uart == UART_2) {
    LPC_SC->PCONP |= (1 << 24); // Power ON peripheral ==> UART2 set bit 24 (table 16)
    LPC_UART2->FCR |= (1 << 0); // Enable FIFO FCR bit 0 (table 400)
    LPC_UART2->LCR |= (1 << 7); // Enable DLAB- Divisor Latch Access (table 401)
    LPC_UART2->LCR |= (3 << 0); // Set 8 bit length
    uint16_t divider = peripheral_clock / (16 * baud_rate);
    LPC_UART2->DLM = (divider >> 8) & 0xFF; // register is 8 bit. The most significant bit to DLL
    LPC_UART2->DLL = (divider >> 0) & 0xFF; // the least significant bit to DLM
  } else if (uart == UART_3) {
    LPC_SC->PCONP |= (1 << 25); // Power ON peripheral ==> UART3 set bit 25 (table 16)
    LPC_UART3->FCR |= (1 << 0); // Enable FIFO FCR bit 0 (table 400)
    LPC_UART3->LCR |= (1 << 7); // Enable DLAB- Divisor Latch Access (table 401)
    LPC_UART3->LCR |= (3 << 0); // Set 8 bit length
    uint16_t divider = peripheral_clock / (16 * baud_rate);
    LPC_UART3->DLM = (divider >> 8) & 0xFF; // register is 8 bit. The most significant bit to DLL
    LPC_UART3->DLL = (divider >> 0) & 0xFF; // the least significant bit to DLM
  }
}
/* ----------------------------------------------------------------------------------------------------- */
/*                          Function READ ==> RBR - Receive buffer register( RO)                       */
/* ------------------------------------------------------------------------------------------------------- */
bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {
  bool check = false;
  if (uart == UART_2) {
    LPC_UART2->LCR &= ~(1 << 7);           // enable DLAB to 0 to access THR, RBR side
    while (!(LPC_UART2->LSR & (1 << 0))) { // The UART receiver FIFO is not empty
      ;                                    // do nothing
    }
    *input_byte = LPC_UART2->RBR; // Copy data from RBR register to pointer input_byte                            // d
  } else if (uart == UART_3) {
    LPC_UART3->LCR &= ~(1 << 7);           // enable DLAB to 0 to access THR, RBR side
    while (!(LPC_UART3->LSR & (1 << 0))) { // The UART receiver FIFO is not empty
      ;                                    // do nothing
    }
    *input_byte = LPC_UART3->RBR; // Copy data from RBR register to pointer input_byte
  }
  return check = true;
}
/* ----------------------------------------------------------------------------------------------------- */
/*                          Function WRITE ==> THR - transmite hold register, (WO)                       */
/* ------------------------------------------------------------------------------------------------------- */
bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
  bool check = false;
  if (uart == UART_2) {
    LPC_UART2->LCR &= ~(1 << 7);                    // // enable DLAB to 0 to access THR, RBR side
    uint8_t transmit_data_reg = (1 << 5);           // Set up THR is empty (table 402)
    while (!(LPC_UART2->LSR & transmit_data_reg)) { // Check THR is not empty, not write
    }
    LPC_UART2->THR = output_byte; // Copy output_byte to THR register
  } else if (uart == UART_3) {
    LPC_UART3->LCR &= ~(1 << 7);                    // // enable DLAB to 0 to access THR, RBR side
    uint8_t transmit_data_reg = (1 << 5);           // Set up THR is empty (table 402)
    while (!(LPC_UART3->LSR & transmit_data_reg)) { // Check THR is not empty, not write
    }
    LPC_UART3->THR = output_byte; // Copy output_byte to THR register/* code */
  }
  return check = true;
}
/* ----------------------------------------------------------------------------------------------------- */
/*                          PIN CONFIGURATION UART2/UART3                       */
/* ------------------------------------------------------------------------------------------------------- */
void uart_pin_configure(uart_number_e uart) {
  if (uart == UART_2) {
    gpio__construct_with_function(2, 8, GPIO__FUNCTION_2);
    gpio__construct_with_function(2, 9, GPIO__FUNCTION_2);
  } else if (uart == UART_3) {
    gpio__construct_with_function(4, 28, GPIO__FUNCTION_2);
    gpio__construct_with_function(4, 29, GPIO__FUNCTION_2);
  }
}
/* ----------------------------------------------------------------------------------------------------- */
/*                          TASK 3: PIN CONFIGURE 2 BOARDS                       */
/* ------------------------------------------------------------------------------------------------------- */
void uart2_3_pin_configure() {
  gpio__construct_with_function(4, 28, GPIO__FUNCTION_2); // Transmit A
  gpio__construct_with_function(2, 9, GPIO__FUNCTION_2);  // Receive B
}

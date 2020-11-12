#include "gpio_isr.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "gpio_lab.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "task.h"
#include <stdio.h>

// Note: You may want another separate array for falling vs. rising edge callbacks
/**
 * Map of the Callback function.
 * This lpc_peripheral__halt_handler will create an empty array contains 32 slots
 * First, function is empty, when an rigister has been assign, it starts fill out the array
 * if no register has been assign, its not going to do anything
 * Faling or Rising is just how the function interrupt during risng ( interrupt during from bottom to top)
 * interrupt during falling (interrupt during from top to bottom)
 */
extern void lpc_peripheral__halt_handler(void);

static function_pointer_t gpio0_callbacks[32 + 32] = {
    lpc_peripheral__halt_handler, // PIN 0
    lpc_peripheral__halt_handler, // PIN 1
    lpc_peripheral__halt_handler, // PIN 2
    lpc_peripheral__halt_handler, // PIN 3
    lpc_peripheral__halt_handler, // PIN 4
    lpc_peripheral__halt_handler, // PIN 5
    lpc_peripheral__halt_handler, // PIN 6
    lpc_peripheral__halt_handler, // PIN 7

    lpc_peripheral__halt_handler, // PIN 8
    lpc_peripheral__halt_handler, // PIN 9
    lpc_peripheral__halt_handler, // PIN 10
    lpc_peripheral__halt_handler, // PIN 11
    lpc_peripheral__halt_handler, // PIN 12
    lpc_peripheral__halt_handler, // PIN 13
    lpc_peripheral__halt_handler, // PIN 14
    lpc_peripheral__halt_handler, // PIN 15

    lpc_peripheral__halt_handler, // PIN 16
    lpc_peripheral__halt_handler, // PIN 17
    lpc_peripheral__halt_handler, // PIN 18
    lpc_peripheral__halt_handler, // PIN 19
    lpc_peripheral__halt_handler, // PIN 20
    lpc_peripheral__halt_handler, // PIN 21
    lpc_peripheral__halt_handler, // PIN 22

    lpc_peripheral__halt_handler, // PIN 23
    lpc_peripheral__halt_handler, // PIN 24
    lpc_peripheral__halt_handler, // PIN 25
    lpc_peripheral__halt_handler, // PIN 26
    lpc_peripheral__halt_handler, // PIN 27
    lpc_peripheral__halt_handler, // PIN 28
    lpc_peripheral__halt_handler, // PIN 29 (SWITCH 2)
    lpc_peripheral__halt_handler, // PIN 30 (SWITCH 3)
    lpc_peripheral__halt_handler, // PIN 31

    /*========================= Port 2: Interupt Array ======================*/

    lpc_peripheral__halt_handler, // Pin 0; Port 2 start at index[32]
    lpc_peripheral__halt_handler, // Pin 1
    lpc_peripheral__halt_handler, // Pin 2
    lpc_peripheral__halt_handler, // Pin 3
    lpc_peripheral__halt_handler, // Pin 4
    lpc_peripheral__halt_handler, // Pin 5
    lpc_peripheral__halt_handler, // Pin 6
    lpc_peripheral__halt_handler, // Pin 7

    lpc_peripheral__halt_handler, // Pin 8
    lpc_peripheral__halt_handler, // Pin 9
    lpc_peripheral__halt_handler, // Pin 10
    lpc_peripheral__halt_handler, // Pin 11
    lpc_peripheral__halt_handler, // Pin 12
    lpc_peripheral__halt_handler, // Pin 13
    lpc_peripheral__halt_handler, // Pin 14
    lpc_peripheral__halt_handler, // pin 15

    lpc_peripheral__halt_handler, // Pin 16
    lpc_peripheral__halt_handler, // Pin 17
    lpc_peripheral__halt_handler, // Pin 18
    lpc_peripheral__halt_handler, // Pin 19
    lpc_peripheral__halt_handler, // Pin 20
    lpc_peripheral__halt_handler, // Pin 21
    lpc_peripheral__halt_handler, // Pin 22
    lpc_peripheral__halt_handler, // Pin 23

    lpc_peripheral__halt_handler, // pin 24
    lpc_peripheral__halt_handler, // Pin 25
    lpc_peripheral__halt_handler, // Pin 26
    lpc_peripheral__halt_handler, // Pin 27
    lpc_peripheral__halt_handler, // Pin 28
    lpc_peripheral__halt_handler, // Pin 29
    lpc_peripheral__halt_handler, // Pin 30
    lpc_peripheral__halt_handler, // Pin 31; Port 2 End at index[63]
};

/*===============================gpio0__attach_Interrupt (Port 0)========================================
*@brief:    Save the CallBack_function add to INTR table + Config INTR for port 0
*@para:     Pin_num
            Interrupt_type ( Rising or Falling)
            Callback (function address)
*@return:   No Return
*@Note:     Set Pin as ( Input + PullDown + Rising or Falling + NVIC_EnableIQR )
            LPC_GPIOINT ( IO0IntEnR or IO0IntEnF )
            Require PullDown -->but It's been config as external resistor base on schematic
// This gpio0__attach_interrupt do 2 things:
// 1. Configure GPIO for enable Rising or Falling edge
// 2. Store the callback based on the pin
// Figure out which pin that generated interrupt. Then these pin as an index into 32 punching pointers
// Simplely called that into attached_user_handler. Check which pin generated the interrupt
========================================================================================================*/
void gpio0__attach_interrupt(uint32_t pin_num, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
  // 1) Configure GPIO 0 pin for rising or falling edge
  if (interrupt_type) {
    // Set PIN as input
    GPIO__set_as_input(0, pin_num);
    // Enable Rising Edge Interrupt
    LPC_GPIOINT->IO0IntEnR |= (1 << pin_num); // Configure GPIO 0 pin for Rising edge
    // Enable NVIC
    NVIC_EnableIRQ(GPIO_IRQn);
    // calling function gpio0_callbacks with pin_num to function pointer "callback"
    gpio0_callbacks[pin_num] = callback;
    fprintf(stderr, "Rising edge\n");
  } else {
    // 2) Store the callback based on the pin at gpio0_callbacks
    // Set PIN as input
    GPIO__set_as_input(0, pin_num);
    // Enable Falling Edge Interrupt
    LPC_GPIOINT->IO0IntEnF |= (1 << pin_num);
    // Enable NVIC
    NVIC_EnableIRQ(GPIO_IRQn);
    // Store the callback based on the pin at gpio0_callbacks
    gpio0_callbacks[pin_num] = callback;
    fprintf(stderr, "falling edge\n");
  }
}

/*===============================gpio0__interrupt_dispatcher (Port 0)======================================
*@brief:    Dispatch the specific function in Interrupt Service Routine (ISR)
*@para:     No parameter
*@return:   No Return
*@Note:     Two main things ( check the interrupt Pin + Map it to interrupt vector table in ISR, then clear)
            LPC_GPIOINT->status ( IO0IntStatR or IO0IntStatF )
            LPC_GPIOINT->clear ( IO0IntClr )
===========================================================================================================*/
void gpio0__interrupt_dispatcher(void) {
  // This part is to write the logic for checking interrupt
  // if Interrupt either at Rising edge or falling edge for whatever variable i
  // it will look for that variable i and assign that variable i for pin_that_generated_interrupt
  // initial pin_that_generated_interrupt
  int pin_that_generated_interrupt = 0;

  for (int i = 0; i <= 31; i++) {
    if ((LPC_GPIOINT->IO0IntStatF & (1 << i)) || (LPC_GPIOINT->IO0IntStatR & (1 << i))) {
      pin_that_generated_interrupt = i;
      break;
    }
  }
  function_pointer_t attached_user_handler = gpio0_callbacks[pin_that_generated_interrupt];
  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();
  LPC_GPIOINT->IO0IntClr |= (1 << pin_that_generated_interrupt);
  fprintf(stderr, "clear\n");
}

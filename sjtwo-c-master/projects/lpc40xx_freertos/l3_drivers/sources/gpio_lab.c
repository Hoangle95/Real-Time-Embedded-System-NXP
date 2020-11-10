#include "gpio_lab.h"
#include "lpc40xx.h"
#include <stdio.h>

/*==================================gpio1__set_as_input==============================
*@brief:    config DIR register as input
*@para:     port_num
            pin_num
*@return:   No return
*@Note:     Manually config GPIO register  ( DIR = 0 as input )-Chap8 P.148
            From Port0 --> Port3
====================================================================================*/
void GPIO__set_as_input(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 1) {
    LPC_GPIO1->DIR &= ~(1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->DIR &= ~(1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->DIR &= ~(1 << pin_num);
  } else {
    printf("Port number is not valid");
  }
}
/*==================================gpio1__set_as_output==============================
*@brief:    config DIR register as output
*@para:     port_num
            pin_num
*@return:   No return
*@Note:     Manually config GPIO register  ( DIR = 1 as input )-Chap8 P.148
            From Port0 --> Port3
====================================================================================*/
void GPIO__set_as_output(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 1) {
    LPC_GPIO1->DIR |= (1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->DIR |= (1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->DIR |= (1 << pin_num);
  } else {
    printf("Port number is not valid");
  }
}
/*==================================gpio1__set_as_high==============================
*@brief:    config SET register as HIGH state
*@para:     port_num
            pin_num
*@return:   No return
*@Note:     Manually config GPIO register  ( SET = 1 as HIGH state )-Chap8 P.148
            From Port0 --> Port3
====================================================================================*/
void GPIO__set_high(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 1) {
    LPC_GPIO1->PIN |= (1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->PIN |= (1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->PIN |= (1 << pin_num);
  } else {
    printf("Port number is not valid");
  }
}
/*==================================gpio1__set_as_low===============================
*@brief:    config CLR register as HIGH state
*@para:     port_num
            pin_num
*@return:   No return
*@Note:     Manually config GPIO register  ( CLR = 1 as LOW state )-Chap8 P.148
            From Port0 --> Port3
====================================================================================*/
void GPIO__set_low(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 1) {
    LPC_GPIO1->PIN &= ~(1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->PIN &= ~(1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->PIN &= ~(1 << pin_num);
  } else {
    printf("Port number is not valid");
  }
}
/*==================================gpio1__set=======================================
*@brief:    config CLR or SET register as HIGH or LOW state base on condition
*@para:     port_num
            pin_num
            high (PIN state HIGH = True or LOW = False)
*@return:   No return
*@Note:     Manually config GPIO register:
            ( CLR=1 or SET=1 as LOW or HIGH state )-Chap8 P.148
            From Port0 --> Port3
====================================================================================*/
void GPIO__set(uint8_t port_num, uint8_t pin_num, bool high) {
  if (high == true) {
    LPC_GPIO1->PIN |= (1 << pin_num);
  } else {
    LPC_GPIO0->PIN &= ~(1 << pin_num);
  }
}
/*==================================gpio1__get_level=======================================
*@brief:    Using PIN register to read physical PIN State as HIGH or LOW
*@para:     port_num
            pin_num
*@return:   If High return "True" Else return "False"
*@Note:     Using GPIO register PIN to update State (HIGH or LOW)
            From Port0 --> Port3
==========================================================================================*/
bool GPIO__get_level(uint8_t port_num, uint8_t pin_num) {
  {
    if (LPC_GPIO1->PIN & (1 << pin_num)) {
      return true;
    } else {
      return false;
    }
  }
}
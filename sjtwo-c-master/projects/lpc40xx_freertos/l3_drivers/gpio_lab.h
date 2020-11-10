#pragma once

#include <stdbool.h>
#include <stdint.h>

void GPIO__set_as_input(uint8_t port_num, uint8_t pin_num);
void GPIO__set_as_output(uint8_t port_num, uint8_t pin_num);
void GPIO__set_high(uint8_t port_num, uint8_t pin_num);
void GPIO__set_low(uint8_t port_num, uint8_t pin_num);
void GPIO__set(uint8_t port_num, uint8_t pin_num, bool high);
bool GPIO__get_level(uint8_t port_num, uint8_t pin_num);
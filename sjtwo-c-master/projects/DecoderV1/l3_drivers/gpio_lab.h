// file gpio_lab.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

/*===================================CONFIG GPIO PIN as INPUT====================================*/
void GPIO__set_as_input(uint8_t port_num, uint8_t pin_num);

/*===================================CONFIG GPIO PIN as OUTPUT===================================*/
void GPIO__set_as_output(uint8_t port_num, uint8_t pin_num);

/*=================================CONFIG GPIO PIN as HIGH state=================================*/
void GPIO__set_high(uint8_t port_num, uint8_t pin_num);

/*=================================CONFIG GPIO PIN as LOW state==================================*/
void GPIO__set_low(uint8_t port_num, uint8_t pin_num);

/*=============================CONFIG GPIO PIN as HIGH or LOW state==============================*/
void GPIO__set(uint8_t port_num, uint8_t pin_num, bool high);

/*==============================READ GPIO PIN state (HIGH or LOW)================================*/
bool GPIO__get_level(uint8_t port_num, uint8_t pin_num);
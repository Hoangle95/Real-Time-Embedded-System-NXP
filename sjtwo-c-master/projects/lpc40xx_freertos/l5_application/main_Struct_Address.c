#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

typedef struct my_s { // define a struct name my_s
  float f1;           // 4 bytes
  char c1;            // 1 byte
  float f2;
  char c2;
} my_s;
typedef struct __attribute__((packed)) my_s_packed {
  float f1;
  char c1;
  float f2;
  char c2;
} my_s_packed;

// int main(void) {

//   // TODO : Instantiate a structure type my_s with the name of "s"
//   my_s s;
//   my_s_packed v;
//   printf("Size : %d bytes\n"
//          "floats 0x%p 0x%p\n"
//          "chars  0x%p 0x%p\n",
//          sizeof(s), &s.f1, &s.f2, &s.c1, &s.c2);
//   printf("Size : %d bytes\n"
//          "floats 0x%p 0x%p\n"
//          "chars  0x%p 0x%p\n",
//          sizeof(v), &v.f1, &v.f2, &v.c1, &v.c2);

//   puts("Starting RTOS");
//   vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

//   return 0;
// }

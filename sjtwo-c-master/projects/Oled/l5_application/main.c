#include "oled.h"

void print_songINFO() {
  oled_print("Duc", page_0, init);
  oled_print("trong", page_2, 0);
  scrolling_addr_mode(page_0, page_0);
  // vertical_addr_mode();
}

int main(void) {

  print_songINFO();
  vTaskStartScheduler();
  return 0;
}
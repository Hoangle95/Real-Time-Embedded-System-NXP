// #include "FreeRTOS.h"
// #include "board_io.h"
// #include "cli_handlers.h"
// #include "clock.h"
// #include "gpio.h"
// #include "gpio_lab.h"
// #include "periodic_scheduler.h"
// #include "portmacro.h"
// #include "queue.h"
// #include "sj2_cli.h"
// #include "task.h"
// #include "uart.h"
// #include "uart_lab.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// // -------------------------------------------------------------------------- //
// //                           Version : Send song request                      //

// // -------------------------------------------------------------------------- //

// // static QueueHandle_t song_name_queue;
// // typedef struct {
// //   char name[32]; // set song name to be in range of 32 charracter (fix length), it could be less than or more than
// // } songname_t;

// // static void cli_simulation_task(void *p) {
// //   songname_t songname = {}; // Initialize songname = empty
// //   strncpy(songname.name, "song.mp3", sizeof(songname.name) - 1);
// //   snprintf(songname.name, sizeof(songname.name), "%s", "song.mp3");
// //   // sprintf(songname.name, "s", "song.mp3saahhshhshshHhF")

// //   if (xQueueSend(song_name_queue, &songname, 0)) {
// //     puts("Success : Send songname to the queue");
// //   } else {
// //     puts("Failure : failed to queue songname");
// //   }
// //   vTaskSuspend(NULL); // Suspend this task forever (we are just mocking CLI task)
// //   while (1) {
// //   }
// // }

// // // Create a help function bc we dont want to write a function with too much line of code
// // static void read_file(const char *filename) { printf("Request : receive to play/read a file: '%s' \n", filename);
// }
// // static void mp3_file_reader_task(void *p) {
// //   songname_t songname = {};
// //   while (1) {
// //     if (xQueueReceive(song_name_queue, &songname, 3000)) {
// //       read_file(songname.name);
// //     } else {
// //       puts("Warning : No new request to read a file");
// //     }
// //   }
// // }

// // static void mp3_data_reader_task(void *p) {
// //   while (1) {
// //   }
// // }

// // int main(void) {
// //   song_name_queue = xQueueCreate(1, sizeof(songname_t)); // Send 1 song to play at a time
// //   xTaskCreate(cli_simulation_task, "cli", 1024 / 1, NULL, 1, NULL);
// //   xTaskCreate(mp3_file_reader_task, "file", 1024 / 1, NULL, 1, NULL);

// //   puts("Starting FreeRTOS");
// //   vTaskStartScheduler();
// //   return 0;
// // }

// // -------------------------------------------------------------------------- //
// //                           Version : Send song request                      //

// // -------------------------------------------------------------------------- //
// static QueueHandle_t song_name_queue;
// static QueueHandle_t mp3_song_data_queue;

// typedef struct {
//   char name[32]; // set song name to be in range of 32 charracter (fix length), it could be less than or more than
// } songname_t;

// static void cli_simulation_task(void *p) {
//   songname_t songname = {}; // Initialize songname = empty
//   strncpy(songname.name, "README.md", sizeof(songname.name) - 1);
//   // snprintf(songname.name, sizeof(songname.name), "%s", "song.mp3");
//   // sprintf(songname.name, "s", "song.mp3saahhshhshshHhF")

//   if (xQueueSend(song_name_queue, &songname, 0)) {
//     puts("Success : Send songname to the queue");
//   } else {
//     puts("Failure : failed to queue songname");
//   }
//   vTaskSuspend(NULL); // Suspend this task forever (we are just mocking CLI task)
//   while (1) {
//   }
// }

// // Create a help function bc we dont want to write a function with too much line of code
// static void read_file(const char *filename) {
//   printf("Request : receive to play/read a file: '%s' \n", filename);

//   // Open file
//   UINT br;
//   FIL file;                                            /* File object */
//   FRESULT result = f_open(&file, filename, (FA_READ)); /* FatFs return code */
//   if (FR_OK == result) {
//     char buffer[512];
//     while (br != 0) {
//       f_read(&file, buffer, sizeof(buffer), &br);
//       printf("Read a block '%d' bytes from file \n");
//     }
//     f_close(&file);
//   } else {
//     printf("Failed to open mp3 file \n");
//   }
// }

// static void mp3_file_reader_task(void *p) {
//   songname_t songname = {};
//   while (1) {
//     if (xQueueReceive(song_name_queue, &songname, 3000)) {
//       read_file(songname.name);
//     } else {
//       puts("Warning : No new request to read a file");
//     }
//   }
// }

// static void mp3_data_reader_task(void *p) {
//   while (1) {
//   }
// }

// int main(void) {
//   song_name_queue = xQueueCreate(1, sizeof(songname_t)); // Send 1 song to play at a time
//   xTaskCreate(cli_simulation_task, "cli", 1024 / 1, NULL, 1, NULL);
//   xTaskCreate(mp3_file_reader_task, "file", 1024 / 1, NULL, 1, NULL);

//   puts("Starting FreeRTOS");
//   vTaskStartScheduler();
//   return 0;
// }

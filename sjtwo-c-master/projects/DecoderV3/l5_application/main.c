#include "FreeRTOS.h"
#include "adc.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "gpio.h"
#include "gpio_isr.h"

#include "gpio_lab.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"

#include "oled.h"
#include "queue.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "ssp2_lab.h"
#include "task.h"
#include <stdio.h>

#include "cli_handlers.h"
#include "decoder_mp3.h"
#include "ff.h"
#include "song_handler.h"
#include "string.h"
#include "uart_lab.h"

// ------------------------------ Queue handle ------------------------------

QueueHandle_t Q_trackname; // CLI or SONG_Control --> reader_Task
QueueHandle_t Q_songdata;  // reader_Task --> player_Task

// Set song name to be in range of 64 character (fix length), it could be less or more than
typedef char trackname_t[64]; // 64 songs can be store
typedef char songdata_t[512]; // 512 bytes idealy

// ------------------------------ Task Handle ------------------------------------------//
TaskHandle_t player_handle;

// ------------------------------ Semaphore Trigger Signal -----------------------------//
SemaphoreHandle_t play_next;
SemaphoreHandle_t play_previous;
SemaphoreHandle_t pause_resume;  // Control both task pause and resume
SemaphoreHandle_t next_previous; // Control both task play_next and play_previous

// ------------------------------ Control Function -------------------------------------//

// Interrupt Service Rountine
static void interrupt_setup();

// ISR Button
static void play_next_ISR(); // ISR next song
static void play_previous_ISR();
static void pause_resume_ISR();    // ISR pause and resume song
static void pause_resume_Button(); // Pause and resume task
//======================================================================================//
//                                MP3 Reader Task                                       //
// Goal : Receive Q_trackname from CLI and reader the trackname in SD card,
//       Then send Q_songdata over to play task
// brief: Receive song_name(Queue track_name) <--CLI input <handler_general.c>
//          Open file = song_name
//          Read file --> Store in buffer[size512]
//          Send it to player_task(Queue songdata)
//          Close file
// note:  There are two Queues involved
//          ---> QueueHandle_t Q_trackname;
//          ---> QueueHandle_t Q_songdata;
//          All use PortMax_delay to sleep and wait to wakeup
//======================================================================================//
static void mp3_reader_task(void *p) {
  trackname_t song_name;
  songdata_t buffer512; // 512 bytes
  UINT br;              // byte read
  while (1) {
    xQueueReceive(Q_trackname, song_name, portMAX_DELAY);

    // ----- OLED screen -----
    oled_print(song_name, page_0, init);
    horizontal_scrolling(page_0, page_0);

    // ----- Reading file -----
    const char *file_name = song_name;
    FIL file;
    FRESULT result = f_open(&file, file_name, (FA_READ));
    if (FR_OK == result) {
      // Update NEW "br" for Loopback
      f_read(&file, buffer512, sizeof(buffer512), &br);
      // When br (read byte) does not 0 then it had data in it, then send song data
      while (br != 0) {
        // Read-->Store Data (object_file --> buffer)
        f_read(&file, buffer512, sizeof(buffer512), &br);
        xQueueSend(Q_songdata, buffer512, portMAX_DELAY);
        // New Song request (CLI)
        if (uxQueueMessagesWaiting(Q_trackname)) {
          printf("New song request\n");
          break;
        }
      }
      f_close(&file);
    }
    // When br (read byte) is 0 then there is no data of song, play next to play to first song
    else if (br == 0) {
      xSemaphoreGive(next_previous);
      xSemaphoreGive(play_next);
      printf("Read byte is 0\n");
    } else {
      printf("Failed to open mp3 object_file \n");
    }
  }
}

//======================================================================================//
//                                MP3 Player Task                                       //
// Goal : Using pointer to get Q_songdata from reader task to play on decoder
//@brief: Receive reader_task(Queue song_data)
//          Send every singple byte from buffer to Decoder
//@note:  Need to wait for Data Request pin (DREQ_HighActive)
//======================================================================================//
static void mp3_player_task(void *p) {
  int counter = 1;
  while (1) {
    // Receive songdata ---> Transfer to DECODER
    songdata_t buffer512;
    xQueueReceive(Q_songdata, buffer512, portMAX_DELAY);
    for (int i = 0; i < 512; i++) {
      // Data Request (Activated)
      while (!get_DREQ_HighActive()) {
        vTaskDelay(1);
      }
      decoder_send_mp3Data(buffer512[i]);
    }
    printf("Buffer Transmit: %d (times)\n", counter);
    counter++;
  }
}

//======================================================================================//
//                                MP3 Song Control Task                                      //
// Goal : Keep track of song name, play next, previous and loop back
//@brief: Control song play list (Loopback + Next + Previous)
//@note:  Loopback at the end (Done)
//          Next song (Done)
//          Previous (???)
//======================================================================================//
static void mp3_SongControl_task(void *p) {
  volatile size_t song_index = 0;
  song_list__populate();
  while (1) {
    if (xSemaphoreTake(next_previous, portMAX_DELAY)) {
      if (xSemaphoreTake(play_next, 10)) {
        // Loop back when hit last song
        if (song_index >= song_list__get_item_count()) {
          song_index = 0;
        }
        xQueueSend(Q_trackname, song_list__get_name_for_item(song_index), portMAX_DELAY);
        song_index++;
      } else if (xSemaphoreTake(play_previous, 10)) {
        if (song_index == 0) {
          song_index = song_list__get_item_count();
        }
        xQueueSend(Q_trackname, song_list__get_name_for_item(song_index), portMAX_DELAY);
        song_index--;
      }
    }
    vTaskDelay(100);
  }
}

int main(void) {

  //===================================Object Control===========================================//
  Q_trackname = xQueueCreate(1, sizeof(trackname_t));
  Q_songdata = xQueueCreate(1, sizeof(songdata_t));
  play_next = xSemaphoreCreateBinary();
  play_previous = xSemaphoreCreateBinary();
  pause_resume = xSemaphoreCreateBinary();
  next_previous = xSemaphoreCreateBinary();

  //===================================Initialize Function=====================================//
  interrupt_setup();
  decoder_setup();
  sj2_cli__init();

  //===================================Song List===============================================//
  song_list__populate();
  song_counter();

  //===================================FreeRTOS Tasks=========================================//
  xTaskCreate(mp3_reader_task, "task_reader", (2048 * 4) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL); // play next
  xTaskCreate(mp3_player_task, "task_player", (2048 * 4) / sizeof(void *), NULL, PRIORITY_MEDIUM,
              &player_handle); // pause and resume
  xTaskCreate(mp3_SongControl_task, "song_control", (2048 * 4) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(pause_resume_Button, "Pause_task", (2048) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  vTaskStartScheduler();
  return 0;
}

//======================================================================================//
//                                Button Function                                      //
//======================================================================================//

//===================================Interrupt Setup===============================================//
static void interrupt_setup() {
  LPC_IOCON->P0_25 &= ~(0b111);
  GPIO__set_as_input(0, 25);

  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio0__interrupt_dispatcher, "INTR Port 0");

  gpio0__attach_interrupt(29, GPIO_INTR__FALLING_EDGE, pause_resume_ISR);
  gpio0__attach_interrupt(30, GPIO_INTR__FALLING_EDGE, play_next_ISR);
  gpio0__attach_interrupt(25, GPIO_INTR__RISING_EDGE, play_previous_ISR);
}

static void play_next_ISR() {
  xSemaphoreGiveFromISR(next_previous, NULL);
  xSemaphoreGiveFromISR(play_next, NULL);
}
static void pause_resume_ISR() { xSemaphoreGiveFromISR(pause_resume, NULL); }
static void play_previous_ISR() {
  xSemaphoreGiveFromISR(next_previous, NULL);
  xSemaphoreGiveFromISR(play_previous, NULL);
}

//===================================P0_29 Pause and Resume Button===============================================//
volatile bool pause = true;

static void pause_resume_Button() {
  while (1) {
    if (xSemaphoreTake(pause_resume, portMAX_DELAY)) {
      if (pause) {
        // if pause is true, suspend task then set pause to false
        vTaskSuspend(player_handle);
        vTaskDelay(300);
        pause = false;
      } else {
        // when hit pause again (false value), it runs the else case and resume the task
        vTaskResume(player_handle);
        vTaskDelay(300);
        pause = true;
      }
    }
  }
}
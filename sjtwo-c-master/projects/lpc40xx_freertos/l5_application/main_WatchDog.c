#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "event_groups.h"
#include "ff.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "gpio_lab.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "pwm1.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "ssp2.h"
#include "ssp2_lab.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// -------------------------------------------------------------------------- //
//                           PART 0 : How to read and write data to SD card   //
// 1. Create Producer task
// 2. Create Consumer task
// 3. Create main with 2 task
// -------------------------------------------------------------------------- //
/*
//
//========================Sample code to WRITE/READ a file to the SD Card=====================================
//1. Open file or Create file using f_open
//2. Convert data to string to put into array
//3. Write data to file using f_write, no pointer
//4. Close file after DONE using f_close
//5. Read data from file using f_read, Using pointer to pointer to that string (&string)
// CAUTION :
//1. WRITE : put it inside a for loop because we want to keep writing data
//2. READ : put it outside a for loop because we want to read it after DONE
//3. Remember to close file after DONE or USing f_sync(&file) to synchronize data process at the moment
//
QueueHandle_t sd_card_Q;
char string[64];
const char *filename = "file.txt";
FIL file; // File handle
void sd_card_init(int data) {

  UINT bytes_written = 0;
  FRESULT open_file = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS)); // Open File OR Create file
  if (FR_OK == open_file) {
    sprintf(string, "SD card open,%i\n", data); // Convert data into string to put in array
    // Write data to file
    if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
      fprintf(stderr, "Successfully write data to SD card\n");
    } else {
      fprintf(stderr, "ERROR: Failed to write data to file\n");
    }
    f_close(&file); // Close file after DONE
  } else {
    fprintf(stderr, "ERROR: Failed to open: %s\n", filename);
  }
  f_read(&file, &string, strlen(string), &bytes_written); // Read data from file
  for (int i = 0; i < strlen(string); i++) {
    fprintf(stderr, "%c", string[i]);
  }
}

void Producer(void *p) {
  int data_send;
  while (1) {
    data_send = 99;
    if (xQueueSend(sd_card_Q, &data_send, 0)) {
    }
    vTaskDelay(1000);
  }
}

void Consumer(void *p) {
  int data_receive;
  while (1) {
    if (xQueueReceive(sd_card_Q, &data_receive, portMAX_DELAY)) {
      sd_card_init(data_receive);
    }
    // TaskHandle_t task_handle = xTaskGetHandle("Consumer");
    f_close(&file);
    vTaskSuspend(NULL);
  }
}

int main(void) {
  xTaskCreate(Producer, "Producer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(Consumer, "Consumser", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);

  sd_card_Q = xQueueCreate(1, sizeof(int));
  vTaskStartScheduler();
  return 0;
}

// -------------------------------------------------------------------------- //
//                           PART 1
//1. Reuse Function read SD card from part 0
//2. Write Producer task
//3. Write COnsumer task
//4. Write Watchdog task which is check if the task complete on producer task and consumer task                      */
// -------------------------------------------------------------------------- //
/*
#include "event_groups.h"
EventGroupHandle_t test;
QueueHandle_t dog;

// Sample code to write a file to the SD Card

char string[64];
const char *filename = "output.txt";
FIL file; // File handle
void sd_card_init(int data) {

  UINT bytes_written = 0;
  FRESULT open_file = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  // puts("before");

  if (FR_OK == open_file) {
    // char string[64];
    sprintf(string, "Value,%i\n", data);
    if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
    } else {
      fprintf(stderr, "ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    fprintf(stderr, "ERROR: Failed to open: %s\n", filename);
  }
}
// -------------------------------------------------------------------------- //
//                           SEND                                             //
// -------------------------------------------------------------------------- //
uint8_t producer = 0x01;
uint8_t consumer = 0x02;
void producer_task(void *p) {
  uint8_t send = 99;  //raw manual set data
  while (1) {
    printf("Send status: %s\n", xQueueSend(dog, &send, 0) ? "T" : "F");
    xEventGroupSetBits(test, producer);
    vTaskDelay(1000);
  }
}
// -------------------------------------------------------------------------- //
//                           RECEIVE                                          //
// -------------------------------------------------------------------------- //
void consumer_task(void *p) {
  while (1) {
    uint8_t receive = 0;
    printf("Receive status: %s Value: %d \n", xQueueReceive(dog, &receive, portMAX_DELAY) ? "T" : "F", receive);
    xEventGroupSetBits(test, consumer);
  }
}
// -------------------------------------------------------------------------- //
//                           WATCHDOG                                         //
// -------------------------------------------------------------------------- //
void Watchdog(void) {
  while (1) {
    uint8_t expected_value = xEventGroupWaitBits(test, 0x04, pdTRUE, pdTRUE, 1000);
    printf("WatchDog verify: %s Value: %d \n\n", (expected_value == 0x04) ? "T" : "F", expected_value);
  }
}
void main(void) {
  // xTaskCreate(Producer, "Producer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(consumer_task, "Consumser", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(producer_task, "Producer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(Watchdog, "Watchdog", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  test = xEventGroupCreate();
  dog = xQueueCreate(1, sizeof(int));
  vTaskStartScheduler();
}
*/
// -------------------------------------------------------------------------- //
//                           PART 1 : Advance                                 //
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//                           STRUCT                                             //
// 1. create sample array of 100 samples
// 2. We only need to grab 1 of these x, y, or z and average of it
// -------------------------------------------------------------------------- //
typedef struct {
  acceleration__axis_data_s sample[100];
  double sum_y;
  double avg_y;
} data;

static QueueHandle_t sensor_queue;
static EventGroupHandle_t WatchDog;
static TickType_t counting; // Create a counting object to keep track of counting
// Create a counting object to keep track of both task healthy(dog_counter1), Consumer, Producer error (dog_counter2)
static TickType_t dog_counter1, dog_counter2;

uint8_t producer = 0x01; // xEventGroupSetBits
uint8_t consumer = 0x02; // xEventGroupSetBits
uint8_t verified = 0x03; // xEventGroupWaitBits
// -------------------------------------------------------------------------- //
//                           SEND                                         //
// -------------------------------------------------------------------------- //
// How to use API sensor ?
// 1. Calling acceleration__init()
// 2. Put it in while loop and grab API to read sensor
// 3. Calculate average data and put value in Queue
//
// Why we need to check acceleration__init() ready or not ready ?
// ==> Because acceleration__init() is type bool
//
// How to read data from acceleration sensor
// 1. Create an array[100] empty slots
// 2. Initilize acceleration
// 3. Read(save) data sensor into array using for loop
// 4. Calculate average value
// 5. Send average value in xQueueSend()
static void producer_task(void *P) {
  /*Initialize sensor*/
  printf("Sensor Status: %s\n", acceleration__init() ? "Ready" : "Not Ready"); // Check sensor status
  data data1;                                                                  // Review how to declare a struct
  while (1) {
    for (int i = 0; i < 100; i++) {
      data1.sample[i] = acceleration__get_data();
      data1.sum_y += data1.sample[i].y;
    }
    data1.avg_y = data1.sum_y / 100;
    printf("Queue: %s\n", xQueueSend(sensor_queue, &data1.avg_y, 0) ? "T" : "F");
    xEventGroupSetBits(WatchDog, producer);
    data1.sum_y = 0.000; // Reset data sum to 0 to avoid data keep adding up

    vTaskDelay(100); // Writing process fast for 0.1s
  }
}
// -------------------------------------------------------------------------- //
//                           RECEIVE                                         //
// -------------------------------------------------------------------------- //
// 1. Open file using f_open
// 2. Create array string
// 3. Grab sensor_queue and pointer it to address of receive
// 3. Conver data into string using sprintf
// 4. Write data from string to address of file using strlen(string) and f_write
// 5. Synchronize the data process at the moment
// 6. Print out data to the screen
static void consumer_task(void *P) {

  const char *filename = "file.txt";
  FIL file;
  UINT bytes_written = 0;
  // File Open IF exist file or Create IF NOT exist
  FRESULT open_file = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  static char string[64]; // Manualy set up array string

  double receive;
  while (1) {
    if (xQueueReceive(sensor_queue, &receive, portMAX_DELAY)) {
      // Check if open_file succeed which FR_OK is API succeed
      if (FR_OK == open_file) {
        counting = xTaskGetTickCount();                               // Keep track Counting Time execution
        sprintf(string, "%li mS <-> Avg: %.2f\n", counting, receive); // put string value into array
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file); // Synchronize and save the process of data at the moment
        }
      }
    }
    printf("%li mS <-> Avg: %.2f\n", counting, receive);
    xEventGroupSetBits(WatchDog, consumer);
  }
}
// -------------------------------------------------------------------------- //
//                           WATCHDOG                                         //
// The purpose of watchdog :
// 1. is to verify task running succesfully
// 2. is to write the test case for tasks
// -------------------------------------------------------------------------- //
void Watchdog_task(void *P) {

  const char *filename1 = "watch_dog.txt";
  FIL file;
  UINT bytes_written = 0;
  FRESULT open_file = f_open(&file, filename1, (FA_WRITE | FA_CREATE_ALWAYS));

  while (1) {
    uint8_t expected_value = xEventGroupWaitBits(WatchDog, verified, pdTRUE, pdTRUE, 2000);
    printf("WatchDog verify: %s\tReturn_Value: %d \n\n ", (expected_value == verified) ? "T" : "F", expected_value);

    // ---------------------------- BOTH TASK HEALTHY ---------------------------
    if ((expected_value == verified)) {
      printf("Both Task Healthy\n");
      if (FR_OK == open_file) {
        static char string[64];
        dog_counter1 = xTaskGetTickCount();
        sprintf(string, "Verified at time: %ldmS\n", dog_counter1);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        }
      }
    }
    //----------------------------- CONSUMER ERROR -----------------------------
    else if (expected_value == 0x04) {
      printf("C_Task Suspend\n");
      if (FR_OK == open_file) {
        static char string[64];
        dog_counter2 = xTaskGetTickCount();
        sprintf(string, "Consumer Error at time: %ldmS\n ", dog_counter2);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        }
      }
    }
    //----------------------------- PRODUCER ERROR -----------------------------
    else {
      printf("P_Task Suspend\n");
      if (FR_OK == open_file) {
        static char string[64];
        dog_counter2 = xTaskGetTickCount();
        sprintf(string, "Producer Error at time: %ldmS\n ", dog_counter2);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        } else {
          printf("ERROR: Failed to write data to file\n");
        }
      } else {
        printf("ERROR: Failed to open: %s\n", filename1);
      }
    }
  }
}
int main(void) {

  //----------------------------- Initialization -----------------------------
  puts("Starting RTOS\n");
  sj2_cli__init();

  //--------------------------- Written to SD card ---------------------------
  sensor_queue = xQueueCreate(1, sizeof(double));
  WatchDog = xEventGroupCreate();
  xTaskCreate(producer_task, "producer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(consumer_task, "consumer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(Watchdog_task, "Watchdog", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}

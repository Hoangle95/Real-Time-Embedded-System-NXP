## MP3 Project: Develop VS1053b (SPI) MIDI Audio Codec Circuit

![image](https://user-images.githubusercontent.com/38081550/97953088-5e363000-1d54-11eb-8272-0d227433ceaf.png)

### OBJECTIVE

1. Create TaskHandle_t "player_handle" to Suspend(pause) or Resume(resume) a song
2. Link the "&player_handle" to the last parameter of mp3_player_task to pause_resume_task
3. Create "pause_resume" binary semaphore to trigger signal
4. Create pause_resume_ISR and attach it to function enbale_interrupt() : review gpio_isr.c
5. Create pause_resume_Button task to decide when to suspend and when to resume
   a) SemaphoreGiveFromISR(pause_resume) in pause_resume_ISR task
   b) SemaphoreTake(pause_resume) in pause_resume_Button task
   i) When (pause) is true, suspend task then set pause to false
   ii) When hit pause again (false value), it runs the else case and resume the task

- L3-Driver [Decoder Header](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/Decoder/l3_drivers/decoder_mp3.h)

- L3-Driver [Decoder Source](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/Decoder/l3_drivers/sources/decoder_mp3.c)
- L5-Application [main](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/Decoder/l5_application/main.c)
- L5-Application [song_handler.c](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/DecoderV1/l5_application/song_handler.c)
- L5-Application [song_handler.h](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/DecoderV1/l5_application/song_handler.h)

```
# Compile Command:
scons --project=mp3_decoder

# SJ2 Board Flash Command:
python nxp-programmer/flash.py --input _build_mp3_decoder/mp3_decoder.bin
```

11/21/2020

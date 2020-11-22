## MP3 Project: Develop VS1053b (SPI) MIDI Audio Codec Circuit

![image](https://user-images.githubusercontent.com/38081550/97953088-5e363000-1d54-11eb-8272-0d227433ceaf.png)

### OBJECTIVE

1. Create "next_previous" to control both task play_next and play_previous songs
2. In the play_next_ISR, send by order "next_previous" , "play_next" binary semaphore
3. In the play_previous_ISR, send by order "next_previous" , "play_previos" binary semaphore
4. In the "mp3_SongControl" task, it has to take "next_previous" object first, then it will decide to go
   "play_next" or "play_previous"
5. Set up an external switch on pin 0_25 as set it up as input and Pull Down Register
   ![image](http://www.falstad.com/circuit/circuitjs.html?cct=%24+3+0.000005+10.20027730826997+50+5+50%0A172+352+200+352+152+0+6+5+5+0+0+0.5+Voltage%0Ar+352+240+352+304+0+1000%0Ag+352+368+352+384+0%0Ac+352+304+352+368+0+0.00001+0%0AS+384+304+432+304+0+1+false+0+2%0Aw+352+240+352+200+0%0Aw+352+304+384+304+0%0Aw+432+320+432+368+0%0Aw+432+368+352+368+0%0Ao+6+64+0+4098+0.0000762939453125+0.025+0+2+6+3%0A)

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

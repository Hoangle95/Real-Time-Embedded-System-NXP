## MP3 Project: Develop VS1053b (SPI) MIDI Audio Codec Circuit

![image](https://user-images.githubusercontent.com/38081550/97953088-5e363000-1d54-11eb-8272-0d227433ceaf.png)
![image](https://user-images.githubusercontent.com/38081550/97952012-e9152b80-1d50-11eb-9854-d748d2d60bc8.png)
![image](https://user-images.githubusercontent.com/38081550/97953356-106df780-1d55-11eb-9d8f-08fd07f97da2.png)

### OBJECTIVE

1. Develop a working driver for Audio Codec Circuit [VS1053b (SPI)](https://cdn-shop.adafruit.com/datasheets/vs1053.pdf)
2. Using [File I/O](http://elm-chan.org/fsw/ff/00index_e.html) API to read mp3 SD card though the CLI command in Terminal Emulator
3. Create --> MP3_reader_task + MP3_player_task <---
   - Using Queue to transmit payload for cooperative tasks (Reader --> Player)
   - MP3_reader_task: OPEN_file --> READ_file --> TRANSMIT to player_task (Queue Song_data)
   - MP3_player_task: RECEIVE from reader_task (Queue Song_data) --> TRANSMIT through VS1053(SPI_0)
4. Audio should not sound distorted, slower or faster when running on your system )

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

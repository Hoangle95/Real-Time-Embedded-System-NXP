## MP3 Project: Develop SSD1306 (SPI) 126x64 OLED Driver (Naive-Way)

![image](https://user-images.githubusercontent.com/38081550/96647106-fc041680-12e1-11eb-9040-10024724c92c.png)
![image](https://user-images.githubusercontent.com/38081550/96643072-e7bd1b00-12db-11eb-8e9c-c9b6a657c082.png)

#### IDEA BEHIND THIS PROJECT

There are many sample drivers for this OLED Display. Most of them are based on the Adafruit Graphic Core libraries source code and Arduino library. Arduino board is excellent for various functions. For instance, it is excellent for developing a quick application. Nevertheless, our teammates believe that relying too much on the Arduino API will create a gap between understanding Hardware concept and Implementation (the ability to develop Firmware base on Datasheet).

1. This is a part of a fully functional MP3 Music player project.
2. Practice the ability of developing Firmware base on using Datasheet only (without exist API from Arduino)
3. We decided to develop our own working driver from scratch without using exist API as our side project.
4. The driver is able to display 128 character from ASCII table

#### NOTE:

1.  The Code might not utilize and be efficiency enough to scale up.
2.  We just want to demonstrate the how this OLED module work in simplest way:
    - Setup the SPI peripheral from MCU
    - Initialize the panel - Sequence of Operation
    - How Data can transfer from MCU to 1KB GDDRAM.
    - How Data Byte convert to pixels.

- L3-Driver [OLED Header](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/Oled/l3_drivers/oled.h)

- L3-Driver [OLED Source](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/Oled/l3_drivers/sources/oled.c)
- L5-Application [main](https://github.com/Hoangle95/Real-Time-Embedded-System-NXP/blob/main/sjtwo-c-master/projects/Oled/l5_application/main.c)

```
# Compile Command:
scons --project=oled

# SJ2 Board Flash Command:
python nxp-programmer/flash.py --input _build_oled/oled.bin
```

# RCDudelTiny
## MP3-Player control via RC
![RCDudelTiny](https://marcostoffers.github.io/rc_dudel.png)

- This board will control the following functions of a MP3-Player: ON / OFF | Play / Pause | Next / Previous | Volume + / -
- The functions will be controlled by only one RC channel. See my Youtube Video of my prototype for a short intro: https://youtu.be/fYEXbu7Ohzc

## Requirement
- A MP3-Player or MP3-board with 3.3 to 5.5V Power
- An amplifier with 1W - 50W regarding the used speaker
- Power supply for amplifier (the MP3-Player will be powered via the RC Receiver

## Connection
Have a look at the following pinout of the MP3-Player connector. A detailed Manual is also available (only german at this time) with lots of informations for building the RCDudelTiny and disassembling the MP3-Player
![Connector](https://marcostoffers.github.io/MP3-Player-Anschluss-RCDudelTiny.png)

## Base
The module is based on a microchip ATtiny84A which is programmed via the [Arduino IDE](https://arduino.cc/). To use the microcontroller, the extension of [Spence Konde](https://github.com/SpenceKonde/ATTinyCore) is necessary. Programming is done without a bootloader directly via the ISP connector with an AVR programmer.

## License
The project is licensed under the Creative Common License. A rebuild for private or non-profit associations is desired, commercial rebuild or distribution I forbid. If someone should develop the mixer further, I ask for the naming of the original project.

![CreativeCommonLicense](https://marcostoffers.github.io/cc.png)

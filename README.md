# 8Encoder-7seg-I2C
OpenHardware / 8-rotary encoder + 8 3digit LED 

Default I2C Address: 0x50

Transmission/reception data format
(16 bits per channel (little endian))
byte 0
bit: 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
     y y y y y y y y y y y y y y y y
     
byte 1
bit: 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
     f x x x x x y y y y y y y y y y
// f: First 1 letter alphabet.
//    If f is 1, the number of significant digits will be 2.
// x: character number (0-26:@-Z)
//    @..0, A..1, B..2, ...
// y: Encoder value (0-99 or 0〜999)

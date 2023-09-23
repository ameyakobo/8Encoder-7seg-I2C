// Read 8 rotary encoders and 8 7-segment LEDs via I2C.
// Copyleft 2023 ameya kobo
// This source requires arduino "megaTinyCore"
#include <Wire.h>

#define I2C_ADDR 0x50  // default address 0x50

#define ATTYNY3217  // use ATtiny*17

#ifdef ATMEGA328
#define SEG_A 2
#define SEG_B 3
#define SEG_C 4
#define SEG_D 5
#define SEG_E 6
#define SEG_F 7
#define SEG_G 8
#define SEG_H 9

#define LS_A0 10
#define LS_A1 11
#define LS_A2 12
#define LS_A3 13
#define LS_A4 14

#define SW_A 15
#define SW_B 16
#endif

#ifdef ATTYNY3217
#define SEG_A PIN_PA1
#define SEG_B PIN_PA2
#define SEG_C PIN_PA3
#define SEG_D PIN_PA4
#define SEG_E PIN_PA5
#define SEG_F PIN_PA6
#define SEG_G PIN_PA7
#define SEG_H PIN_PC1

#define LS_A0 PIN_PB2
#define LS_A1 PIN_PB3
#define LS_A2 PIN_PB4
#define LS_A3 PIN_PB5
#define LS_A4 PIN_PC0

#define SW_A PIN_PC2
#define SW_B PIN_PC3
#endif

#define WAITD 500
#define WAITL 5

volatile int16_t EncBuf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t EncA[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t EncAb[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t EncB[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t EncBb[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t EncFast[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t fChr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

#define MAXEncBuf 16

void setup() {
  pinModeFast(SEG_A, OUTPUT);
  pinModeFast(SEG_B, OUTPUT);
  pinModeFast(SEG_C, OUTPUT);
  pinModeFast(SEG_D, OUTPUT);
  pinModeFast(SEG_E, OUTPUT);
  pinModeFast(SEG_F, OUTPUT);
  pinModeFast(SEG_G, OUTPUT);
  pinModeFast(SEG_H, OUTPUT);
  pinModeFast(LS_A0, OUTPUT);
  pinModeFast(LS_A1, OUTPUT);
  pinModeFast(LS_A2, OUTPUT);
  pinModeFast(LS_A3, OUTPUT);
  pinModeFast(LS_A4, OUTPUT);
  pinModeFast(SW_A, INPUT_PULLUP);
  pinModeFast(SW_B, INPUT_PULLUP);

  digitalWriteFast(SEG_A, LOW);
  digitalWriteFast(SEG_B, LOW);
  digitalWriteFast(SEG_C, LOW);
  digitalWriteFast(SEG_D, LOW);
  digitalWriteFast(SEG_E, LOW);
  digitalWriteFast(SEG_F, LOW);
  digitalWriteFast(SEG_G, LOW);
  digitalWriteFast(SEG_H, LOW);
  digitalWriteFast(LS_A0, HIGH);
  digitalWriteFast(LS_A1, HIGH);
  digitalWriteFast(LS_A2, HIGH);
  digitalWriteFast(LS_A3, HIGH);
  digitalWriteFast(LS_A4, HIGH);

  Wire.begin(I2C_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void receiveEvent() {
  int cnt;
  for (cnt = 0; Wire.available(); cnt++) {
    *((uint8_t *)EncBuf + cnt) = Wire.read();
  }
}

void requestEvent(int count) {
  Wire.write((uint8_t *)EncBuf, 16);
}

const uint8_t Digi[][7] = {
  { 1, 1, 1, 1, 1, 1, 0 },  // 0
  { 0, 1, 1, 0, 0, 0, 0 },  // 1
  { 1, 1, 0, 1, 1, 0, 1 },  // 2
  { 1, 1, 1, 1, 0, 0, 1 },  // 3
  { 0, 1, 1, 0, 0, 1, 1 },  // 4
  { 1, 0, 1, 1, 0, 1, 1 },  // 5
  { 1, 0, 1, 1, 1, 1, 1 },  // 6
  { 1, 1, 1, 0, 0, 0, 0 },  // 7
  { 1, 1, 1, 1, 1, 1, 1 },  // 8
  { 1, 1, 1, 1, 0, 1, 1 },  // 9
  { 0, 0, 0, 0, 0, 0, 0 },  // null

  { 1, 1, 1, 1, 1, 0, 1 },  // @
  { 1, 1, 1, 0, 1, 1, 1 },  // A
  { 0, 0, 1, 1, 1, 1, 1 },  // b
  { 0, 0, 0, 1, 1, 0, 1 },  // c
  { 0, 1, 1, 1, 1, 0, 1 },  // d
  { 1, 0, 0, 1, 1, 1, 1 },  // E
  { 1, 0, 0, 0, 1, 1, 1 },  // F
  { 1, 0, 1, 1, 1, 1, 0 },  // G
  { 0, 0, 1, 0, 1, 1, 1 },  // h
  { 0, 0, 1, 0, 0, 0, 0 },  // i
  { 0, 1, 1, 1, 0, 0, 0 },  // J
  { 0, 1, 0, 1, 1, 1, 1 },  // K
  { 0, 0, 0, 1, 1, 1, 0 },  // L
  { 1, 0, 1, 0, 1, 0, 0 },  // M
  { 0, 0, 1, 0, 1, 0, 1 },  // n
  { 0, 0, 1, 1, 1, 0, 1 },  // o
  { 1, 1, 0, 0, 1, 1, 1 },  // P
  { 1, 1, 1, 0, 0, 1, 1 },  // q
  { 1, 1, 0, 0, 1, 1, 0 },  // R
  { 0, 0, 1, 1, 0, 1, 1 },  // S
  { 0, 0, 0, 1, 1, 1, 1 },  // t
  { 0, 1, 1, 1, 1, 1, 0 },  // U
  { 0, 0, 1, 1, 1, 0, 0 },  // v
  { 0, 1, 0, 1, 0, 1, 0 },  // w
  { 1, 0, 0, 1, 0, 0, 1 },  // x
  { 0, 1, 1, 1, 0, 1, 1 },  // y
  { 0, 1, 0, 0, 1, 0, 1 },  // z
};

void loop() {
  uint8_t count, addr, lop;
  uint16_t cnt, chr;
  uint8_t nunber[] = { 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0 };

  while (1) {
    for (count = 0; count <= 7; count++) {

      chr = EncBuf[count];
      fChr[count] = chr & 0x8000 ? 1 : 0;
      chr >>= 10;
      chr &= 0b00011111;
      addr = count * 3;

      if (chr == 0 || fChr[count] != 0) {  // No alphabet or first character
        cnt = EncBuf[count] & 0x03ff;
        nunber[addr + 2] = cnt % 10;
        if (cnt >= 10) {
          nunber[addr + 1] = (cnt / 10) % 10;
        } else {
          nunber[addr + 1] = 10;  // null
          nunber[addr] = 10;      // null
        }

        if (cnt >= 100) {
          nunber[addr] = (cnt / 100) % 10;
        } else {
          nunber[addr] = 10;  // null
        }
      }

      if (chr != 0) {
        if (fChr[count] == 0) {  // If the first character is not an alphabet, blank 2 digits.
          nunber[addr + 1] = 10;
          nunber[addr + 2] = 10;
        }
        if (chr <= 26) {
          nunber[addr] = chr + 11;
        } else {
          nunber[addr] = 10;
        }
      }
    }

    for (count = 0; count <= 8; count++) {
      if (count == 8) {
        swread();
        continue;
      }
      addr = count * 3;

      swread();

        digitalWriteFast(SEG_A, Digi[nunber[addr]][0]);
        digitalWriteFast(SEG_B, Digi[nunber[addr]][1]);
        digitalWriteFast(SEG_C, Digi[nunber[addr]][2]);
        digitalWriteFast(SEG_D, Digi[nunber[addr]][3]);
        digitalWriteFast(SEG_E, Digi[nunber[addr]][4]);
        digitalWriteFast(SEG_F, Digi[nunber[addr]][5]);
        digitalWriteFast(SEG_G, Digi[nunber[addr]][6]);

        _setLedAdrs(addr);
        delayMicroseconds(WAITD);
        _setLedAdrs(24);
        delayMicroseconds(WAITL);
        addr++;

        digitalWriteFast(SEG_A, Digi[nunber[addr]][0]);
        digitalWriteFast(SEG_B, Digi[nunber[addr]][1]);
        digitalWriteFast(SEG_C, Digi[nunber[addr]][2]);
        digitalWriteFast(SEG_D, Digi[nunber[addr]][3]);
        digitalWriteFast(SEG_E, Digi[nunber[addr]][4]);
        digitalWriteFast(SEG_F, Digi[nunber[addr]][5]);
        digitalWriteFast(SEG_G, Digi[nunber[addr]][6]);

        _setLedAdrs(addr);
        delayMicroseconds(WAITD);
        _setLedAdrs(24);
        delayMicroseconds(WAITL);
        addr++;

        digitalWriteFast(SEG_A, Digi[nunber[addr]][0]);
        digitalWriteFast(SEG_B, Digi[nunber[addr]][1]);
        digitalWriteFast(SEG_C, Digi[nunber[addr]][2]);
        digitalWriteFast(SEG_D, Digi[nunber[addr]][3]);
        digitalWriteFast(SEG_E, Digi[nunber[addr]][4]);
        digitalWriteFast(SEG_F, Digi[nunber[addr]][5]);
        digitalWriteFast(SEG_G, Digi[nunber[addr]][6]);

        _setLedAdrs(addr);
        delayMicroseconds(WAITD);
        _setLedAdrs(24);
        delayMicroseconds(WAITL);
    }
  }
}

void _setLedAdrs(uint8_t addr) {
  digitalWriteFast(LS_A0, addr & 1);
  digitalWriteFast(LS_A1, addr & 2 ? 1 : 0);
  digitalWriteFast(LS_A2, addr & 4 ? 1 : 0);
  digitalWriteFast(LS_A3, addr & 8 ? 1 : 0);
  digitalWriteFast(LS_A4, addr & 16 ? 1 : 0);
}

uint16_t swread_times[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

void swread() {
  int16_t buf;
  uint8_t count;

  _setLedAdrs(24);

  digitalWriteFast(SEG_A, LOW);
  digitalWriteFast(SEG_B, HIGH);
  digitalWriteFast(SEG_C, HIGH);
  digitalWriteFast(SEG_D, HIGH);
  digitalWriteFast(SEG_E, HIGH);
  digitalWriteFast(SEG_F, HIGH);
  digitalWriteFast(SEG_G, HIGH);
  digitalWriteFast(SEG_H, HIGH);

  EncA[0] = digitalRead(SW_A);
  EncB[0] = digitalRead(SW_B);

  digitalWriteFast(SEG_A, HIGH);
  digitalWriteFast(SEG_B, LOW);

  EncA[1] = digitalRead(SW_A);
  EncB[1] = digitalRead(SW_B);

  digitalWriteFast(SEG_B, HIGH);
  digitalWriteFast(SEG_C, LOW);

  EncA[2] = digitalRead(SW_A);
  EncB[2] = digitalRead(SW_B);

  digitalWriteFast(SEG_C, HIGH);
  digitalWriteFast(SEG_D, LOW);

  EncA[3] = digitalRead(SW_A);
  EncB[3] = digitalRead(SW_B);

  digitalWriteFast(SEG_D, HIGH);
  digitalWriteFast(SEG_E, LOW);

  EncA[4] = digitalRead(SW_A);
  EncB[4] = digitalRead(SW_B);

  digitalWriteFast(SEG_E, HIGH);
  digitalWriteFast(SEG_F, LOW);

  EncA[5] = digitalRead(SW_A);
  EncB[5] = digitalRead(SW_B);

  digitalWriteFast(SEG_F, HIGH);
  digitalWriteFast(SEG_G, LOW);

  EncA[6] = digitalRead(SW_A);
  EncB[6] = digitalRead(SW_B);

  digitalWriteFast(SEG_G, HIGH);
  digitalWriteFast(SEG_H, LOW);

  EncA[7] = digitalRead(SW_A);
  EncB[7] = digitalRead(SW_B);

  digitalWriteFast(SEG_H, LOW);

  for (count = 0; count <= 7; count++) {
    if (EncA[count] == 0) {
      if (EncAb[count] < 9) {
        EncAb[count]++;
      }
    } else {
      EncAb[count] = 0;
    }
    if (EncB[count] == 0) {
      if (EncBb[count] < 9) {
        EncBb[count]++;
      }
    } else {
      EncBb[count] = 0;
    }

    swread_times[count]++;
    if (swread_times[count] >= 50) {
      EncFast[count] = 0;
      swread_times[count] = 0;
    }

    if (EncAb[count] == 1) {
      noInterrupts();
      if (EncBb[count] == 0) {  // Right rotation
        EncFast[count]++;
        buf = EncBuf[count] & 0x3ff;
        buf++;
        if (EncFast[count] >= 3) {
          buf += 9;
        }
        EncBuf[count] &= 0xfc00;
        if (fChr[count] != 0) {  // If the first character is an alphabet, up to 99.
          if (buf > 99) {
            buf = 99;
          }
        }
        if (buf > 999) {
          buf = 999;
        }
        EncBuf[count] += buf;
      } else {  // Left rotation
        EncFast[count]++;
        buf = EncBuf[count] & 0x3ff;
        buf--;
        if (EncFast[count] >= 3) {
          buf -= 9;
        }
        EncBuf[count] &= 0xfc00;
        if (buf > 0) {
          EncBuf[count] += buf;
        }
      }
      interrupts();
    }
  }
}

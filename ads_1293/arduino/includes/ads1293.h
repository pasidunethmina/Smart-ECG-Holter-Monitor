#ifndef ads1293_H
#define ads1293_H

#include <SPI.h>

// Pin definitions
#define PIN_MISO 37
#define PIN_MOSI 35
#define PIN_SCLK 36
#define PIN_CS   39
// Control Pins
#define PIN_DRDYB 14
#define PIN_ALARMB 15

int ch1_ = 0;
int ch2_ = 0;
int ch3_ = 0;

SPISettings ads1293Settings(1000000, MSBFIRST, SPI_MODE0);

// ADS1293 functions remain unchanged
void ads1293_init() {}
void ads1293_write_reg(uint8_t reg, uint8_t value) {
  SPI.beginTransaction(ads1293Settings);
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(reg & 0x7F);
  SPI.transfer(value);
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
}

uint8_t ads1293_read_reg(uint8_t reg) {
  SPI.beginTransaction(ads1293Settings);
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(reg | 0x80);
  uint8_t value = SPI.transfer(0x00);
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
  return value;
}

void ads1293_read_ecg(uint8_t *data) {
  SPI.beginTransaction(ads1293Settings);
  digitalWrite(PIN_CS, LOW);
  data[0] = SPI.transfer(0x80 | 0x37);
  for (int i = 1; i < 10; i++) {
    data[i] = SPI.transfer(0x00);
  }
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
}

#endif
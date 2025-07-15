#ifndef init_H
#define init_H

#include <SPI.h>
#include <SD.h>
#include "data_store.h"
#include "display.h"

// Your SD card SPI pins
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   10

SPIClass SPI_ECG(2);
SPIClass SPI_SD(1);  // Create SPI bus instance (can also try HSPI if VSPI doesn’t work)

// Tell TFT to use SPI_SD
Adafruit_ST7735 tft(&SPI_SD, TFT_CS, TFT_DC, TFT_RST);


void init_setup() {
  Serial.begin(115200);
  delay(200);

  // ========== Initialize ECG SPI (ADS1293) ==========
  SPI_ECG.begin(PIN_SCLK, PIN_MISO, PIN_MOSI, PIN_CS);
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH); // Deselect ADS
  pinMode(PIN_DRDYB, INPUT_PULLUP);
  pinMode(PIN_ALARMB, INPUT_PULLUP);
  delay(50);

  // ========== Initialize SD/TFT SPI ==========
  SPI_SD.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, -1);
  pinMode(PIN_NUM_CS, OUTPUT);
  digitalWrite(PIN_NUM_CS, HIGH); // Deselect SD
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);     // Deselect TFT
  delay(50);

  // ========== Initialize SD Card ==========
  Serial.println("Initializing SD card...");
  if (!SD.begin(PIN_NUM_CS, SPI_SD)) {
    Serial.println("SD Card Init Failed - Check connections and format (FAT32).");
    while (1);
  }

  // Open data file
  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile) {
    Serial.println("Failed to open file on SD card.");
  }

  // ========== Initialize TFT Display ==========
  tft.initR(INITR_BLACKTAB);  // ST7735S (black tab)
  tft.setRotation(1);         // Landscape
  tft.fillScreen(ST77XX_BLACK);
  showWelcomeScreen();

  // ========== Initialize ADS1293 ==========
  digitalWrite(PIN_CS, LOW);   // Select ADS1293
  ads1293_init();
  ads1293_write_reg(0x00, 0x01); // Soft reset
  delay(10);
  digitalWrite(PIN_CS, HIGH);  // Deselect ADS

  struct {
    uint8_t reg;
    uint8_t val;
  } ads1293_config[] = {
    {0x00, 0x02}, {0x01, 0x11}, {0x02, 0x19}, {0x03, 0x2E},
    {0x0A, 0x07}, {0x0C, 0x04}, {0x0D, 0x01}, {0x0E, 0x02},
    {0x0F, 0x03}, {0x10, 0x01}, {0x12, 0x04}, {0x21, 0x02},
    {0x22, 0x02}, {0x23, 0x02}, {0x24, 0x02}, {0x27, 0x08},
    {0x2F, 0x70},
  };

  for (size_t i = 0; i < sizeof(ads1293_config)/sizeof(ads1293_config[0]); ++i) {
    digitalWrite(PIN_CS, LOW);
    ads1293_write_reg(ads1293_config[i].reg, ads1293_config[i].val);
    digitalWrite(PIN_CS, HIGH);
    delay(2);
  }

  // Start Conversion
  digitalWrite(PIN_CS, LOW);
  ads1293_write_reg(0x00, 0x03); // Start Conversions (active mode)
  digitalWrite(PIN_CS, HIGH);

  Serial.println("CH1\tCH2\tCH3");  // For Serial Plotter
}

#endif
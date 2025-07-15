#ifndef init_H
#define init_H

#include <SPI.h>
#include <SD.h>
#include "data_store.h"
#include "firebase.h"

// Your SD card SPI pins
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   10

SPIClass SPI_SD(1);  // Create SPI bus instance (can also try HSPI if VSPI doesn’t work)


void init_setup() {
  SPI.begin(PIN_SCLK, PIN_MISO, PIN_MOSI, PIN_CS);
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);
  pinMode(PIN_DRDYB, INPUT_PULLUP);
  pinMode(PIN_ALARMB, INPUT_PULLUP);
  delay(100);
  Serial.println("Initializing SD card...");
  
  // Initialize SPI_SD with your custom pins
  SPI_SD.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CS);
  pinMode(PIN_NUM_CS, OUTPUT);
  digitalWrite(PIN_NUM_CS, HIGH); // Deselect SD

  if (!SD.begin(PIN_NUM_CS, SPI_SD)) {
    Serial.println("SD Card Init Failed - Check:");
    Serial.println("1. Different SPI pins than ADS1293");
    Serial.println("2. SD card formatted as FAT32");
    Serial.println("3. Proper power supply (3.3V stable)");
    while(1); // Halt if SD fails
  }

  // Open in append mode (creates if doesn't exist)
  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile) {
    Serial.println("File open failed");
  }

  
  // Initialize and configure ADS1293
  ads1293_init();
  ads1293_write_reg(0x00, 0x01); // Soft reset
  delay(10);
  
  // Configuration settings
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
    ads1293_write_reg(ads1293_config[i].reg, ads1293_config[i].val);
    delay(2);
  }

  ads1293_write_reg(0x00, 0x01); // Start conversion
  
  // Serial Plotter setup - add offsets to separate graphs
  Serial.println("CH1\tCH2\tCH3");

    // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  esp_sntp_servermode_dhcp(1);  // (optional)

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Connected");
  
  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagically.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /**
   * A more convenient approach to handle TimeZones with daylightOffset
   * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  //configTzTime(time_zone, ntpServer1, ntpServer2);

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);
  
  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

#endif
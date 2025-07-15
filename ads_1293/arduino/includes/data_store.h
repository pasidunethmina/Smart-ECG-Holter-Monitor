#ifndef data_store_H
#define data_store_H


// SPI pins for ESP32 (adjust according to your board)
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   10

File dataFile;
const char* filename = "/data.log"; // Fixed filename
String channel_output = "";

void writeData(const String &data) {
  if (dataFile) {
    dataFile.println(data); // Auto-appends to end
    dataFile.flush(); // Ensure data is written
  }
  else {
    Serial.println("failed");
  }
}


// Call this when done writing
void closeFile() {
  if (dataFile) {
    dataFile.close();
  }
}

#endif
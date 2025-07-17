#include <string>

#include "ads1293.h"
#include "firebase.h"
#include "init.h"

unsigned long lastCheck = 0;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("hhhhhhhhhhhhhhhh");
  // Initialize SPI and pins
  init_setup();
}

void loop() {
  while (digitalRead(PIN_DRDYB)) { delay(1); }

  uint8_t ecg_data[10] = {0};
  ads1293_read_ecg(ecg_data);

  // Convert to 24-bit signed values
  int32_t ch1 = (ecg_data[1] << 16) | (ecg_data[2] << 8) | ecg_data[3];
  int32_t ch2 = (ecg_data[4] << 16) | (ecg_data[5] << 8) | ecg_data[6];
  int32_t ch3 = (ecg_data[7] << 16) | (ecg_data[8] << 8) | ecg_data[9];

  // Two's complement conversion
  ch1 = (ch1 & 0x800000) ? ch1 - 0x1000000 : ch1;
  ch2 = (ch2 & 0x800000) ? ch2 - 0x1000000 : ch2;
  ch3 = (ch3 & 0x800000) ? ch3 - 0x1000000 : ch3;

  ch1_ = ch1/2^23;
  ch2_ = ch2/2^23;
  ch3_ = ch3/2^23;

  String ch1_str = String(ch1_);
  String ch2_str = String(ch2_);
  String ch3_str = String(ch3_);
  String channel_output = ch1_str + " " + ch2_str + " " + ch3_str;
  writeData(channel_output);
  delay(5);
  // Add offsets to separate graphs vertically
  // Serial.print(3050000); // Upper bound
  // Serial.print("  ");
  Serial.print(ch1_);   // CH1 at original position
  Serial.print("  ");
  Serial.print(ch2_);   // CH2 shifted up by 100,000
  Serial.print("  ");
  Serial.println(ch3_); // CH3 shifted up by 200,000
  // Serial.print("  ");
  // Serial.println(3030000);  // Lower bound
  delay(5);

  app.loop();
  
  // Check if authentication is ready
  if (app.ready()){ 
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      lastSendTime = currentTime;

      // Prepare data
      String timeStr = getLocalTimeString();
      floatValue1 = 0.01 + random(0, 100);
      floatValue2 = 0.02 + random(0, 100);
      floatValue3 = 0.03 + random(0, 100);
      String combinedData = timeStr + "," + ch1_str + "," + ch2_str + "," + ch3_str;

      // Send as one string to Firebase
      Database.push<String>(aClient, "/devices/dev001/data2", combinedData, processData, "RTDB_Send_Combined");

      // Update intValue for next round
      intValue++;
    }
  }

}

/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
*********/
#ifndef firebase_H
#define firebase_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <sys/time.h>  // for gettimeofday()
#include "time.h"
#include "esp_sntp.h"


// Network and Firebase credentials
#define WIFI_SSID "Galaxy A33 5G 87B5"
#define WIFI_PASSWORD "duqo0858"

#define Web_API_KEY "AIzaSyAPQlPG6WhdHUKFZs2jcXJeuisI2lkZ6-8"
#define DATABASE_URL "https://esp-32---firebase-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "nethminapasidu8@gmail.com"
#define USER_PASS "20020301"

const char *ssid = "Dialog 4G 159";
const char *password = "vinuka.1234";

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 3600*4.5;
const int daylightOffset_sec = 3600;

const char *time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

String getLocalTimeString() {
  struct tm timeinfo;
  struct timeval now;

  if (!getLocalTime(&timeinfo)) {
    return "No time available (yet)";
  }

  gettimeofday(&now, NULL); // get seconds + microseconds

  char timeStringBuff[64];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%B %d %Y %H:%M:%S", &timeinfo);

  // calculate milliseconds from microseconds
  int millis = now.tv_usec / 1000;

  // combine date/time string with milliseconds
  char finalString[80];
  snprintf(finalString, sizeof(finalString), "%s.%03d", timeStringBuff, millis);

  return String(finalString);
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  getLocalTimeString();
}

// User function
void processData(AsyncResult &aResult);

// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 500; // 10 seconds in milliseconds

// Variables to send to the database
int intValue = 0;
float floatValue1 = 0.01;
float floatValue2 = 0.01;
float floatValue3 = 0.01;
String stringValue = "";

// void setup(){
//   Serial.begin(115200);
//   delay(1000);

//   // Connect to Wi-Fi
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");

//   esp_sntp_servermode_dhcp(1);  // (optional)

//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println("Connected");
  
//   // set notification call-back function
//   sntp_set_time_sync_notification_cb(timeavailable);

//   /**
//    * This will set configured ntp servers and constant TimeZone/daylightOffset
//    * should be OK if your time zone does not need to adjust daylightOffset twice a year,
//    * in such a case time adjustment won't be handled automagically.
//    */
//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

//   /**
//    * A more convenient approach to handle TimeZones with daylightOffset
//    * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
//    * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
//    */
//   //configTzTime(time_zone, ntpServer1, ntpServer2);

//   // Configure SSL client
//   ssl_client.setInsecure();
//   ssl_client.setConnectionTimeout(1000);
//   ssl_client.setHandshakeTimeout(5);
  
//   // Initialize Firebase
//   initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
//   app.getApp<RealtimeDatabase>(Database);
//   Database.url(DATABASE_URL);
// }

// void loop(){
//   app.loop();
  
//   // Check if authentication is ready
//   if (app.ready()){ 
//     unsigned long currentTime = millis();
//     if (currentTime - lastSendTime >= sendInterval){
//       lastSendTime = currentTime;

//       // Prepare data
//       String timeStr = getLocalTimeString();
//       floatValue1 = 0.01 + random(0, 100);
//       floatValue2 = 0.02 + random(0, 100);
//       floatValue3 = 0.03 + random(0, 100);
//       String combinedData = "Date&Time: " + timeStr + ",  Ch1: " + String(floatValue1, 2) + ",  Ch2: " + String(floatValue2, 2) + ",  Ch3: " + String(floatValue3, 2);

//       // Send as one string to Firebase
//       Database.set<String>(aClient, "/Pasidu/CombinedData", combinedData, processData, "RTDB_Send_Combined");

//       // Update intValue for next round
//       intValue++;
//     }
//   }
// }


void processData(AsyncResult &aResult) {
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}


#endif
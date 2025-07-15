#ifndef display_H
#define display_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


// TFT Display pins for ESP32
#define TFT_CS         8
#define TFT_RST        21
#define TFT_DC         9
#define TFT_MOSI       11
#define TFT_SCLK       12

// Button pins
#define PB_OK 5    // Menu/Select button
#define PB_UP 6    // Up button
#define PB_DOWN 7  // Down button

// Create TFT display object
extern Adafruit_ST7735 tft;  // only declare

// Global variables
unsigned long monitoringDuration = 0; // in seconds (0 = not monitoring)
unsigned long monitoringStartTime = 0;
unsigned long lastButtonPressTime = 0;
const unsigned long cancelTimeout = 5000; // 5 seconds timeout

// Menu variables
int current_mode = 0;
int max_modes = 2;
String modes[] = {
  "1. 1 day",
  "2. 2 days"
};


void print_line(String text, int column, int row, int text_size) {
  tft.setTextSize(text_size);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(column, row);
  tft.println(text);
}

void showWelcomeScreen() {
  tft.fillScreen(ST77XX_BLACK);
  print_line("SmartCare", 30, 30, 2);
  print_line("Smart Cardiac", 15, 60, 2);
  print_line("Monitor", 40, 90, 2);
}


void displayMonitoringStatus() {
  unsigned long remainingTime = monitoringDuration - (millis() - monitoringStartTime) / 1000;
  
  int days = remainingTime / 86400;
  int hours = (remainingTime % 86400) / 3600;
  int minutes = (remainingTime % 3600) / 60;
  int seconds = remainingTime % 60;
  
  char timeStr[30];
  sprintf(timeStr, "%02d:%02d:%02d:%02d", days, hours, minutes, seconds);

  tft.fillScreen(ST77XX_BLACK);
  print_line("Monitoring...", 20, 10, 1);
  print_line(timeStr, 20, 40, 2);
  print_line("Remaining Time", 20, 80, 1);
}

void monitoringComplete() {
  monitoringDuration = 0;
  tft.fillScreen(ST77XX_BLACK);
  print_line("Monitoring Done!", 10, 50, 2);
  delay(3000);
  showWelcomeScreen(); // Return to welcome screen after completion
}

int wait_for_button_press() {
  lastButtonPressTime = millis();
  
  while(true) {
    if (millis() - lastButtonPressTime > cancelTimeout) {
      return -1; // Timeout (cancel/back)
    }
    
    if (digitalRead(PB_UP) == LOW) {
      delay(200); // Debounce
      lastButtonPressTime = millis();
      return PB_UP;
    }
    else if (digitalRead(PB_DOWN) == LOW) {
      delay(200); // Debounce
      lastButtonPressTime = millis();
      return PB_DOWN;
    }
    else if (digitalRead(PB_OK) == LOW) {
      delay(200); // Debounce
      lastButtonPressTime = millis();
      return PB_OK;
    }
  }
}


void start_monitoring(int mode) {
  switch (mode) {
    case 0: // 1 day
      monitoringDuration = 24 * 60 * 60;
      break;
    case 1: // 2 days
      monitoringDuration = 2 * 24 * 60 * 60;
      break;
  }
  
  monitoringStartTime = millis();
  
  tft.fillScreen(ST77XX_BLACK);
  print_line("Starting...", 30, 50, 2);
  delay(1000);
}

void go_to_menu() {
  lastButtonPressTime = millis();
  
  while (true) {
    tft.fillScreen(ST77XX_BLACK);
    print_line("Monitoring Time", 20, 10, 1);
    
    for (int i = 0; i < max_modes; i++) {
      if (i == current_mode) {
        print_line(">", 10, 30 + i * 20, 1);
        print_line(modes[i], 30, 30 + i * 20, 1);
      } else {
        print_line(modes[i], 30, 30 + i * 20, 1);
      }
    }

    int pressed = wait_for_button_press();
    if (pressed == -1) {
      showWelcomeScreen(); // Return to welcome screen on timeout
      return;
    }
    else if (pressed == PB_UP) {
      current_mode = (current_mode - 1 + max_modes) % max_modes;
    }
    else if (pressed == PB_DOWN) {
      current_mode = (current_mode + 1) % max_modes;
    }
    else if (pressed == PB_OK) {
      start_monitoring(current_mode);
      return;
    }
  }
}


#endif
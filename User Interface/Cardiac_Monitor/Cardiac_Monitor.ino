#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

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

// Declare objects
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Global variables
unsigned long monitoringDuration = 0; // in seconds (0 = not monitoring)
unsigned long monitoringStartTime = 0;
unsigned long lastButtonPressTime = 0;
const unsigned long cancelTimeout = 5000; // 5 second timeout for cancel/back

// Menu variables
int current_mode = 0;
int max_modes = 2;
String modes[] = {
  "1. 1 day",
  "2. 2 days"
};

void setup() {
  pinMode(PB_OK, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);

  Serial.begin(9600);
  
  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);  // Initialize ST7735S chip, black tab
  tft.setRotation(1);         // Landscape orientation
  tft.fillScreen(ST77XX_BLACK);
  
  // Show welcome message
  showWelcomeScreen();
}

void loop() {
  if (monitoringDuration > 0) {
    // Monitoring is active
    displayMonitoringStatus();
    
    // Check if monitoring period has ended
    if (millis() - monitoringStartTime >= monitoringDuration * 1000) {
      monitoringComplete();
    }
  }
  
  // Check for menu button press
  if (digitalRead(PB_OK) == LOW) {
    delay(200); // Debounce
    lastButtonPressTime = millis();
    if (monitoringDuration == 0) {
      // Only allow menu access when not monitoring
      go_to_menu();
    }
  }
}

void showWelcomeScreen() {
  tft.fillScreen(ST77XX_BLACK);
  print_line("SmartCare", 30, 30, 2);
  print_line("Smart Cardiac", 15, 60, 2);
  print_line("Monitor", 40, 90, 2);
  delay(3000); // Show welcome screen for 3 seconds
  tft.fillScreen(ST77XX_BLACK);
}

void print_line(String text, int column, int row, int text_size) {
  tft.setTextSize(text_size);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(column, row);
  tft.println(text);
}

void displayMonitoringStatus() {
  unsigned long remainingTime = monitoringDuration - (millis() - monitoringStartTime) / 1000;
  
  int days = remainingTime / 86400;
  int hours = (remainingTime % 86400) / 3600;
  int minutes = (remainingTime % 3600) / 60;
  int seconds = remainingTime % 60;
  
  tft.fillScreen(ST77XX_BLACK);
  print_line("Monitoring...", 20, 10, 1);
  
  char timeStr[30];
  sprintf(timeStr, "%02d:%02d:%02d:%02d", days, hours, minutes, seconds);
  print_line(timeStr, 20, 50, 2);
  
  print_line("Remaining Time", 20, 80, 1);
}

void monitoringComplete() {
  monitoringDuration = 0; // Stop monitoring
  tft.fillScreen(ST77XX_BLACK);
  print_line("Monitoring Done!", 10, 50, 2);
  delay(3000);
  tft.fillScreen(ST77XX_BLACK);
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

void go_to_menu() {
  lastButtonPressTime = millis();
  
  while (true) {
    tft.fillScreen(ST77XX_BLACK);
    print_line("Monitoring Time", 10, 10, 1);
    
    for (int i = 0; i < max_modes; i++) {
      print_line(modes[i], 20, (i+1)*20 + 20, 1);
    }
    
    print_line(">", 10, (current_mode+1)*20 + 20, 1);

    int pressed = wait_for_button_press();
    if (pressed == -1) {
      // Timeout - exit menu
      tft.fillScreen(ST77XX_BLACK);
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

void start_monitoring(int mode) {
  switch (mode) {
    case 0: // 1 day
      monitoringDuration = 24 * 60 * 60; // 1 day in seconds
      break;
    case 1: // 2 days
      monitoringDuration = 2 * 24 * 60 * 60; // 2 days in seconds
      break;
  }
  
  monitoringStartTime = millis();
  
  tft.fillScreen(ST77XX_BLACK);
  print_line("Starting...", 30, 50, 2);
  delay(1000);
}
// setup 8266 chip
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// setup LED backpack
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment matrix = Adafruit_7segment();

#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi Configuration
const char*   ssid        = "YOUR_SSID";
const char*   password    = "YOUR_PASSWORD";

// NTP Configuration
const char*   ntpServer   = "2.sg.pool.ntp.org";
const int8_t  ntpOffset   = -4; // hours

// Struct to store time, mostly for ESP8266 RTC memory
struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} timeStruct;

bool updating = false;

void setup() {
  matrix.begin(0x70);
  Serial.begin(115200);
  Serial.println();
  
  // Begin WiFi
  Serial.print("Wi-Fi starting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) yield();
  Serial.println(" OK.");

  matrix.writeDigitRaw(0,84); // n
  matrix.writeDigitRaw(1,120); // t
  matrix.writeDigitRaw(3,115); // p
  matrix.writeDigitRaw(4,0);
  matrix.writeDisplay();
  
  getTimeFromNtp();
  updateDisplay();
}

void loop() {
  if  (!updating) {
    getTimeFromNtp();
    updateDisplay();
  }
  delay(30000);
};

void getTimeFromNtp() {
  updating = true;

  // Begin NTP
  Serial.print("Begin NTP...");
  WiFiUDP udp;
  NTPClient timeClient(udp, ntpServer, ntpOffset * 3600);
  timeClient.begin();
  while (!timeClient.update()) yield();
  timeStruct.hours = timeClient.getHours();
  timeStruct.minutes = timeClient.getMinutes();
  timeStruct.seconds = timeClient.getSeconds();
  Serial.println(" OK.");
  
  updating = false;
}

void updateDisplay() {
  Serial.print("Updating display...");
  Serial.print("hours");
  Serial.print(timeStruct.hours);
  Serial.print("minutes");
  Serial.print(timeStruct.minutes);
  matrix.writeDigitNum(0, timeStruct.hours < 10 ? 0 : timeStruct.hours / 10);
  matrix.writeDigitNum(1, timeStruct.hours % 10);
  matrix.writeDigitNum(3, timeStruct.minutes < 10 ? 0 : timeStruct.minutes / 10);
  matrix.writeDigitNum(4, timeStruct.minutes % 10);
  matrix.drawColon(1);
  matrix.writeDisplay();
  Serial.println(" OK.");
}

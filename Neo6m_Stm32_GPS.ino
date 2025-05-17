
//Neo6m Stm32 GPS-
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// GPS
TinyGPSPlus gps;

void setup() {
  Serial1.begin(9600);    // GPS UART
  Serial.begin(115200);   // Debug monitor

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 init failed!"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("STM32 + GPS");
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();
  delay(3000);
  display.clearDisplay();
}

void loop() {
  GPSDelay(1000); // Process GPS data
  double lat_val = gps.location.lat();
  double lng_val = gps.location.lng();
  bool loc_valid = gps.location.isValid();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (!loc_valid) {
    display.println("Waiting for GPS...");
    Serial.println("Latitude : *****");
    Serial.println("Longitude : *****");
  } else {
    Serial.println("GPS READING: ");
    Serial.print("Latitude : ");
    Serial.println(lat_val, 6);
    Serial.print("Longitude : ");
    Serial.println(lng_val, 6);

    display.print("LAT: ");
    display.println(lat_val, 6);
    display.print("LONG: ");
    display.println(lng_val, 6);
  }

  display.display();
  delay(4000);
}

static void GPSDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

//transmeterstm32_0.5
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>  // OLED display library

// Define pins and parameters
#define throttle_pin PA0
#define yaw_pin PA1
#define roll_pin PA3
#define pitch_pin PA2
#define OLED_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Define the NRF24L01 Transmitter pins for STM32
RF24 radio(PA4, PB0); // CE pin on PA4, CSN pin on PB0

// Define the structure to store signal data
struct Signal {
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
};

Signal data;
bool connectionStatus = false; // Track the connection status

// OLED Display Initialization
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Reset signal data to default values
void ResetData() {
  data.throttle = 0;   // Motor stop
  data.pitch = 127;     // Center
  data.roll = 127;      // Center
  data.yaw = 127;       // Center
}

void setup() {
  // Initialize the OLED display
  if (!display.begin(OLED_I2C_ADDRESS, OLED_RESET)) {
    Serial.println("OLED initialization failed!");
    while (1);  // Stop if OLED fails
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("Transmitter Ready"));
  display.display();

  // Start the NRF24L01 module
  radio.begin();
  radio.openWritingPipe(0xE9E8F0F0E1LL); // Use the same pipe address as in the receiver
  radio.setAutoAck(false);                    // Ensure autoACK is disabled
  radio.setChannel(0x2F);
  radio.setPALevel(RF24_PA_MAX);  
  radio.setDataRate(RF24_2MBPS);

  radio.stopListening();  // Set to Transmitter mode
  ResetData();

  Serial.println("Transmitter Setup Done!");
}

// Map joystick values to 0-255 range (Low-0, Middle-128, High-255)
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle)
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return (reverse ? 255 - val : val);
}

void loop() {
  // Read potentiometer values and map them to 0-255 range
data.throttle = mapJoystickValues(analogRead(PA0),0, 2448, 4095, true);
data.yaw = mapJoystickValues(analogRead(PA1), 0, 3080, 4095, false);
data.pitch = mapJoystickValues(analogRead(PA2), 0, 3140, 4095, false);
data.roll = mapJoystickValues(analogRead(PA3), 0, 3050, 4095, false);


  
 

  // Send data via NRF24L01 and wait for acknowledgment
  connectionStatus = radio.write(&data, sizeof(Signal));

  // Display transmitted data on OLED
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("T:");
  display.println(data.throttle);

  display.setCursor(40, 10);   
  display.print("P:");
  display.println(data.pitch);

  display.setCursor(40, 20);   
  display.print("R:");
  display.println(data.roll);

  display.setCursor(0, 20);  
  display.print("Y:");
  display.println(data.yaw);

  display.display();  // Update the OLED display

  if (connectionStatus) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.println("Transmission failed.");
  }

  delay(50);  // Wait before sending the next batch of data
}


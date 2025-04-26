//stm32 transmeter0.4
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define input pins
#define throttle_pin PA0
#define yaw_pin PA1
#define roll_pin PA3
#define pitch_pin PA2

// Define nRF24L01 module pins
#define CE_PIN  PB4
#define CSN_PIN PB5

RF24 radio(CE_PIN, CSN_PIN);  // CE, CSN

// Define the address for the receiver
const uint64_t pipeOut = 0xE8E8F0F0E1LL;  // Receiver pipe address

struct MyData {
  long throttle;
  long yaw;
  long roll;
  long pitch;
};

MyData data;

// Function to reset data
void resetData() {
  data.throttle = 1000;
  data.yaw = 1500;
  data.roll = 1500;
  data.pitch = 1500;
}

void setup() {
  Serial.begin(9600);  // Initialize Serial Monitor

  // Initialize the NRF24 module
  if (!radio.begin()) {
    Serial.println("❌ nRF24L01 module initialization failed");
    while (1);  // Stay here if the module doesn't initialize
  }
  Serial.println("✅ nRF24L01 module initialized successfully!");

  radio.setAutoAck(true);  // Auto acknowledgment for reliability
  radio.setChannel(0x2F);  // Set the RF channel
  radio.setPALevel(RF24_PA_HIGH);  // Set power amplification level
  radio.setDataRate(RF24_2MBPS);  // Set the data rate
  radio.openWritingPipe(pipeOut);  // Open the pipe for writing
  radio.stopListening();  // Stop listening for incoming data (TX mode)

  resetData();  // Initialize data
}

void loop() {
  // Read analog values from the sensors
  long rawThrottle = analogRead(throttle_pin);
  long rawYaw = analogRead(yaw_pin);
  long rawRoll = analogRead(roll_pin);
  long rawPitch = analogRead(pitch_pin);

  // Map the analog values to a desired range (1000-2000)
  data.throttle = map(rawThrottle, 0, 4095, 1000, 2000);
  data.yaw = map(rawYaw, 0, 4095, 1000, 2000);
  data.roll = map(rawRoll, 0, 4095, 1000, 2000);
  data.pitch = map(rawPitch, 0, 4095, 1000, 2000);

  // Send the data to the receiver
  bool result = radio.write(&data, sizeof(MyData));

  // Print status and data to Serial Monitor
  if (result) {
    Serial.println("✔️ Data sent successfully!");
  } else {
    Serial.println("❌ Transmission failed!");
  }
  
  Serial.print("Throttle: ");
  Serial.print(data.throttle);
  Serial.print(" | Yaw: ");
  Serial.print(data.yaw);
  Serial.print(" | Roll: ");
  Serial.print(data.roll);
  Serial.print(" | Pitch: ");
  Serial.println(data.pitch);

  delay(100);  // Delay to slow down the transmission (10 Hz)
}

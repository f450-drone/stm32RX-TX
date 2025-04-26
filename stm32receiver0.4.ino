//stm32 receiver0.4
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// Define nRF24L01 module pins
#define CE_PIN  PB4
#define CSN_PIN PB5

// Servo pins for controlling the motors
#define THROTTLE_PIN PA0
#define YAW_PIN      PA1
#define PITCH_PIN    PA2
#define ROLL_PIN     PA3

RF24 radio(CE_PIN, CSN_PIN);  // CE, CSN

const uint64_t pipeIn = 0xE8E8F0F0E1LL;  // Same pipe address as transmitter

// Define the data structure
struct MyData {
  long throttle;
  long yaw;
  long roll;
  long pitch;
};

MyData data;

// Servo objects for controlling servos
Servo throttleServo;
Servo yawServo;
Servo rollServo;
Servo pitchServo;

// Variable to track the last time data was received
unsigned long lastRecvTime = 0;

void resetData() {
  data.throttle = 1000;
  data.yaw = 1500;
  data.roll = 1500;
  data.pitch = 1500;
}

void recvData() {
  while (radio.available()) {
    radio.read(&data, sizeof(MyData));  // Read the data from the transmitter
    lastRecvTime = millis();  // Record the last received time
  }
}

void setup() {
  Serial.begin(9600);  // Initialize Serial Monitor
  resetData();  // Initialize data

  // Attach the servos to the PWM pins
  throttleServo.attach(THROTTLE_PIN);
  yawServo.attach(YAW_PIN);
  rollServo.attach(ROLL_PIN);
  pitchServo.attach(PITCH_PIN);

  // Initialize the NRF24 module
  if (!radio.begin()) {
    Serial.println("❌ nRF24L01 module initialization failed");
    while (1);  // Stay here if the module doesn't initialize
  }
  Serial.println("✅ nRF24L01 module initialized successfully!");

  radio.setAutoAck(true);  // Enable auto acknowledgment
  radio.setChannel(0x2F);  // Set the RF channel (must match the transmitter)
  radio.setPALevel(RF24_PA_HIGH);  // Set power amplification level
  radio.setDataRate(RF24_2MBPS);  // Set the data rate
  radio.openReadingPipe(1, pipeIn);  // Open the pipe for reading
  radio.startListening();  // Start listening for incoming data
}

void loop() {
  recvData();  // Try to receive data

  unsigned long now = millis();

  // Failsafe: Reset data if no signal is received for 1 second
  if (now - lastRecvTime > 1000) {
    resetData();  // Reset data to failsafe position
  }

  // Output the received data to the servos (PWM)
  throttleServo.writeMicroseconds(data.throttle);  // Control throttle servo
  yawServo.writeMicroseconds(data.yaw);            // Control yaw servo
  rollServo.writeMicroseconds(data.roll);          // Control roll servo
  pitchServo.writeMicroseconds(data.pitch);        // Control pitch servo

  // Print the data to Serial Monitor
  Serial.print("Throttle: ");
  Serial.print(data.throttle);
  Serial.print(" | Yaw: ");
  Serial.print(data.yaw);
  Serial.print(" | Roll: ");
  Serial.print(data.roll);
  Serial.print(" | Pitch: ");
  Serial.println(data.pitch);

  delay(100);  // Delay to control the update rate (50 Hz)
}

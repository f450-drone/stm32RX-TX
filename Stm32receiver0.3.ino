//stm32 receiver
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>


// NRF24 Setup
const uint64_t pipeIn = 0xE8E8F0F0E1LL;
RF24 radio(PB4, PB5);  // CE, CSN

// Data structure
struct MyData {
  long throttle;
  long yaw;
  long roll;
  long pitch;
};
MyData data;

void resetData() {
  data.throttle = 1000;
  data.yaw = 1500;
  data.roll = 1500;
  data.pitch = 1500;
}

// Servo objects
Servo throttleServo;
Servo yawServo;
Servo rollServo;
Servo pitchServo;

// Output pins
#define THROTTLE_PIN PA0
#define YAW_PIN      PA1
#define PITCH_PIN    PA2
#define ROLL_PIN     PA3



void setup() {
  Serial.begin(9600);
  resetData();

  // Attach servo outputs
    throttleServo.attach(THROTTLE_PIN);
      yawServo.attach(YAW_PIN);
  rollServo.attach(ROLL_PIN);
  pitchServo.attach(PITCH_PIN);

  // NRF24 Setup

  radio.begin();
  radio.setAutoAck(true);
  radio.setChannel(0x2F);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();
}

unsigned long lastReceiveTime = 0;

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(MyData));
    lastReceiveTime = millis();
  }

  // Failsafe: no signal for 1 sec
  if (millis() - lastReceiveTime > 1000) {
    resetData();
     radio.stopListening();   // Refresh the radio
  delay(5);
  radio.startListening();
  }

  // Output to PWM (in microseconds)
  throttleServo.writeMicroseconds(data.throttle);
  yawServo.writeMicroseconds(data.yaw);
  rollServo.writeMicroseconds(data.roll);
  pitchServo.writeMicroseconds(data.pitch);

  // Debug
  Serial.print("Throttle: "); Serial.print(data.throttle);
  Serial.print(" | Yaw: "); Serial.print(data.yaw);
  Serial.print(" | Roll: "); Serial.print(data.roll);
  Serial.print(" | Pitch: "); Serial.println(data.pitch);

  delay(20); // 50 Hz
}

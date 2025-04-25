//stm32 trnsmeter0.3
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define input pins
#define throttle_pin PA0
#define yaw_pin PA1
#define roll_pin PA3
#define pitch_pin PA2
//#define aux1_pin PB8
//#define aux2_pin PB9
//#define pot_pin PA4

const uint64_t pipeOut = 0xE8E8F0F0E1LL;
RF24 radio(PB4, PB5);  // CE, CSN

struct MyData {
  long throttle;
  long yaw;
  long roll;
  long pitch;
  //bool AUX1;
 // bool AUX2;
};

MyData data;

void resetData() {
  data.throttle = 1000;
  data.yaw = 2048;
  data.pitch = 2048;
  data.roll = 2048;
  //data.AUX1 = 0;
  //data.AUX2 = 0;
}

void setup() {
  pinMode(throttle_pin, INPUT);
  pinMode(yaw_pin, INPUT);
  pinMode(roll_pin, INPUT);
  pinMode(pitch_pin, INPUT);
  //pinMode(aux1_pin, INPUT_PULLDOWN);
  //pinMode(aux2_pin, INPUT_PULLDOWN);
//  pinMode(pot_pin, INPUT);

  Serial.begin(9600);  // <-- Open Serial Monitor and select 9600 baud

  radio.begin();
  radio.setAutoAck(true);
  radio.setChannel(0x2F);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(pipeOut);
radio.stopListening();                      // <== Force TX mode
  resetData();
}

void loop() {
  long rawThrottle = analogRead(throttle_pin);
  long rawYaw = analogRead(yaw_pin);
  long rawRoll = analogRead(roll_pin);
  long rawPitch = analogRead(pitch_pin);
 // bool aux_1 = digitalRead(aux1_pin);
 // bool aux_2 = digitalRead(aux2_pin);

  data.throttle = map(rawThrottle, 0, 4095, 1000, 2000);
  data.yaw = map(rawYaw, 0, 4095, 1000, 2000);
  data.roll = map(rawRoll, 0, 4095, 1000, 2000);
  data.pitch = map(rawPitch, 0, 4095, 1000, 2000);
  //data.AUX1 = aux_1;
  //data.AUX2 = aux_2;

  // Send data over NRF
  radio.write(&data, sizeof(MyData));

  // Print to Serial Monitor
  Serial.print("Throttle: ");
  Serial.print(data.throttle);
  Serial.print(" | Yaw: ");
  Serial.print(data.yaw);
  Serial.print(" | Roll: ");
  Serial.print(data.roll);
  Serial.print(" | Pitch: ");

  Serial.println(data.pitch); // ✅ ab yeh newline add karega
  //Serial.print(" | AUX1: ");
  //Serial.print(data.AUX1);
  //Serial.print(" | AUX2: ");
 // Serial.println(data.AUX2);

  delay(100);  // Slow it down for readability
}


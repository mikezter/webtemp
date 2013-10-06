/*
  DHT Libraries:
 https://github.com/ringerc/Arduino-DHT22
 https://github.com/markruys/arduino-DHT
 https://github.com/nethoncho/Arduino-DHT22
 https://github.com/ninjablocks/arduino/blob/master/DHT22/DHT22.cpp
 */
#include "DHT.h"
#include <Console.h>
#include "Timer.h"

#define LED 13
#define DHTPIN 7
#define DIRECTION 12
#define BRAKE 9
#define SPEED 3

#define BREAK_SENSE 50

boolean flag = true;
boolean ledState = LOW;
unsigned long previousMillis = 0;
DHT dht;
float hum, tmp, oldHum, oldTmp;
Timer t;

int currentId = 30000;
int currents[BREAK_SENSE];
int currentIndex = 0;

void setup() {
  // Begin
  pinMode(LED, OUTPUT);
  pinMode(DIRECTION, OUTPUT); //Initiates Motor Channel A
  pinMode(BRAKE, OUTPUT); //Initiates Brake Channel A

  digitalWrite(BRAKE, LOW);
  digitalWrite(LED, HIGH);

  // Setup
  dht.setup(DHTPIN);
  Bridge.begin();
  Console.begin();

  t.every(dht.getMinimumSamplingPeriod(), pollSensor);

  // Ready
  digitalWrite(LED, LOW);
}

void pollSensor() {
  hum = dht.getHumidity();
  tmp = dht.getTemperature();
  t.pulseImmediate(LED, 10, HIGH);
}

void printReadings() {
  if (oldHum != hum || oldTmp != tmp) {
    Console.print(hum);
    Console.print("\t");
    Console.println(tmp);

    oldHum = hum;
    oldTmp = tmp;
  }
}

void readCommand() {
  if (Console.available() > 0) {
    char c = Console.read();
    if (c == 'U') stepUp();
    if (c == 'D') stepDown();
    if (c == 'S') stop();
  }
}

void stepUp() {
  stop();
  Console.println("Step up!");
  digitalWrite(DIRECTION, LOW);
  digitalWrite(BRAKE, LOW);
  currentId = t.pulseImmediate(SPEED, 9000, HIGH);
}

void stepDown() {
  stop();
  Console.println("Step down!");
  digitalWrite(DIRECTION, HIGH);
  digitalWrite(BRAKE, LOW);
  currentId = t.pulseImmediate(SPEED, 9000, HIGH);
}

void stop() {
  Console.println("Stop!");
  t.stop(currentId);
  digitalWrite(BRAKE, HIGH);
  analogWrite(SPEED, 0);
  delay(300);
}

void checkCurrent() {
  int current = analogRead(0);
  currents[currentIndex] = current;
  currentIndex = (currentIndex + 1) % BREAK_SENSE;
  int sum = 0;
  for (int i = 0; i < BREAK_SENSE; i++) {
    sum += currents[i];
  }
  if (sum > 30 * BREAK_SENSE) {
    Console.print("Current is ");
    Console.print(sum);
    Console.print(" -> ");
    stop();
  }
}

void loop() {
  printReadings();
  readCommand();
  checkCurrent();
  t.update();
}


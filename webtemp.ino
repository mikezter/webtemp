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
#define DELAY 2000

#define BREAK_SENSE 50

DHT dht;
Timer t;
float hum, tmp;
unsigned long stopTime = 0;
unsigned long upwards = 400000;
unsigned long downwards = 400000;

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

  // Timers
  t.every(DELAY, pollSensor);
  t.every(DELAY, printReadings);
  t.every(DELAY, putReadings);
  t.every(1000, doApi);

  // Ready
  digitalWrite(LED, LOW);
}

void putReadings() {
  Bridge.put("hum", String(hum));
  Bridge.put("tmp", String(tmp));
}

void pollSensor() {
  hum = dht.getHumidity();
  tmp = dht.getTemperature();
  t.pulseImmediate(LED, 10, HIGH);
}

void printReadings() {
  Console.print(hum);
  Console.print("\t");
  Console.println(tmp);
}

void calibrate() {
  unsigned long upwards, downwards;
  range(LOW);
  delay(1000);
  upwards = range(HIGH);
  delay(1000);
  downwards = range(LOW);

  Console.print("Calibrated: U:");
  Console.print(upwards);
  Console.print(" D:");
  Console.println(downwards);

  Bridge.put("downwards", String(downwards));
  Bridge.put("upwards", String(upwards));
}

unsigned long range(bool dir) {
  unsigned long startTime = millis();
  unsigned long previous = stopTime;

  while (previous == stopTime) {
    digitalWrite(SPEED, HIGH);
    digitalWrite(BRAKE, LOW);
    digitalWrite(DIRECTION, dir);
    delay(100);
    checkCurrent();
  }

  return stopTime - startTime;

}

void readCommand() {
  if (Console.available() > 0) {
    char c = Console.read();
    runCommand(c);
  }
}

void doApi() {
  char c;
  Bridge.get("c", &c, 1);
  Bridge.put("c", "");
  if (c != '\0') runCommand(c);
}

void runCommand(char c) {
    if (c == 'U') stepUp();
    if (c == 'D') stepDown();
    if (c == 'S') stop();
    if (c == 'P') printReadings();
    if (c == 'C') calibrate();
}

void stepUp() {
  stop();
  Console.println("Step up!");
  digitalWrite(DIRECTION, HIGH);
  digitalWrite(BRAKE, LOW);
  currentId = t.pulseImmediate(SPEED, 9000, HIGH);
}

void stepDown() {
  stop();
  Console.println("Step down!");
  digitalWrite(DIRECTION, LOW);
  digitalWrite(BRAKE, LOW);
  currentId = t.pulseImmediate(SPEED, 9000, HIGH);
}

void stop() {
  stopTime = millis();
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
  readCommand();
  checkCurrent();
  t.update();
}


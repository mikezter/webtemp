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

#include "valve.h"

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
  t.every(DELAY, doApi);
  t.every(DELAY * 10, softReset);

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

void softReset() {
  asm volatile("jmp 0");
}

void printReadings() {
  Console.print(hum);
  Console.print("\t");
  Console.println(tmp);
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
  runCommand(c);
}

void runCommand(char c) {
    if (c == 'U') stepUp();
    if (c == 'D') stepDown();
    if (c == 'S') stop();
    if (c == 'P') printReadings();
    if (c == 'C') calibrate();
}
void loop() {
  readCommand();
  checkCurrent();
  t.update();
}


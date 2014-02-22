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
#define DELAY 2000

DHT dht;
Timer t;
float hum, tmp;

#include "valve.h"

void setup() {
  valve_setup();

  // Begin
  pinMode(LED, OUTPUT);
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


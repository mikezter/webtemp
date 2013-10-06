/*
  DHT Libraries:
  https://github.com/ringerc/Arduino-DHT22
  https://github.com/markruys/arduino-DHT
  https://github.com/nethoncho/Arduino-DHT22
  https://github.com/ninjablocks/arduino/blob/master/DHT22/DHT22.cpp
*/
#include "DHT.h"
#include<stdlib.h>
#include <Console.h>

#define LEDPIN 13
#define DHTPIN 7

boolean flag = true;
boolean ledState = LOW;
unsigned long previousMillis = 0;
DHT dht;
float hum, tmp;

void setup() {
  // Begin
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);

  // Setup
  dht.setup(DHTPIN);
  Bridge.begin();
  Console.begin();

  // Ready
  digitalWrite(LEDPIN, LOW);
}


void loop() {
  blinker(10, 1100);
//  delay(dht.getMinimumSamplingPeriod());

  hum = dht.getHumidity();
  tmp = dht.getTemperature();

  Console.print(hum);
  Console.print("\t");
  Console.println(tmp);
}

void blinker(unsigned long on, unsigned long off) {
  unsigned long now = millis();
  unsigned long elapsed = now - previousMillis;

  if (!ledState && elapsed > off) {
    toggleLed();
    previousMillis = now;
  }

  else if(ledState && elapsed > on) {
    toggleLed();
    previousMillis = now;

  }
}

void toggleLed() {
  ledState = !ledState;
  digitalWrite(LEDPIN, ledState);
}




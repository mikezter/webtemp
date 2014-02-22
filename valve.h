/* valve.h
 * used to control a radiator valve
 */

void stop() {
  stopTime = millis();
  Console.println("Stop!");
  t.stop(currentId);
  digitalWrite(BRAKE, HIGH);
  analogWrite(SPEED, 0);
  delay(300);
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

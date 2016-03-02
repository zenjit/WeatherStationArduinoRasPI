#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(9);
}

void loop() {
  for (pos = 0; pos < 180)
  myservo.write(0);
  delay (1000);
  myservo.write(200);
  delay (1000);
  myservo.write(-200);
  delay (1000);
  //myservo.detach();
}

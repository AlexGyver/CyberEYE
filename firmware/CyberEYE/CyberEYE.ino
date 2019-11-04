#define SERVO_Y A0
#define SERVO_X A1

#define SERVO_Y_MIN 0
#define SERVO_Y_MAX 40
#define SERVO_X_MIN 0
#define SERVO_X_MAX 180

#include <ServoSmooth.h>
ServoSmooth servoY, servoX;

void setup() {
  // подключаем серво и отпраляем в центр
  servoY.attach(SERVO_Y);
  servoX.attach(SERVO_X);
  servoY.write((SERVO_Y_MAX + SERVO_Y_MIN) / 2);
  servoX.write((SERVO_X_MAX + SERVO_X_MIN) / 2);
  servoY.setTargetDeg((SERVO_Y_MAX + SERVO_Y_MIN) / 2);
  servoX.setTargetDeg((SERVO_X_MAX + SERVO_X_MIN) / 2);
  servoY.setAutoDetach(false);
  servoX.setAutoDetach(false);

  servoY.setSpeed(50);
  servoX.setSpeed(50);
  servoY.setAccel(0.1);
  servoX.setAccel(0.1);
}

void loop() {
  servoY.tick();
  servoX.tick();
  timerTick();
}

void timerTick() {
  static uint32_t prevTime = 0;
  static int rndTime = 0;
  if (millis() - prevTime >= rndTime) {
    prevTime = millis();
    rndTime = random(200, 1000);
    setEyeDeg(random(0, 350), 20);
  }
}

void setEyeXY(int8_t x, int8_t y) {
  int deg = degrees(atan2(y, x));
  if (y < 0) deg += 360;
  setEyeDeg(deg, constrain(hypot(x, y), 0, 20));
}

void setEyeDeg(int deg, byte r) {   // deg 0-360, r 0-20
  byte valX, valY;
  if (deg < 180) {
    valX = map(deg, 0, 179, SERVO_X_MAX, SERVO_X_MIN);
    valY = map(r, 0, 20, (SERVO_Y_MIN + SERVO_Y_MAX) / 2, SERVO_Y_MAX);
  } else {
    valX = map(deg, 180, 359, SERVO_X_MAX, SERVO_X_MIN);
    valY = map(r, 0, 20, (SERVO_Y_MIN + SERVO_Y_MAX) / 2, SERVO_Y_MIN);
  }
  servoY.setTargetDeg(valY);
  servoX.setTargetDeg(valX);
}

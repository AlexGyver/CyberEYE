#define SERVO_Y A0
#define SERVO_X A1

#define SERVO_Y_MIN 0
#define SERVO_Y_MAX 40
#define SERVO_X_MIN 0
#define SERVO_X_MAX 180

#define INVERT_Y_GYRO 1
#define INVERT_X_GYRO 0

#include <ServoSmooth.h>
ServoSmooth servoY, servoX;
#include <Wire.h>
int16_t GyX, GyY;
const byte MPUaddr = 0x68;

// Y GyY вниз - верх +
// X GyX лево - право +
// -32000.. 32000

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPUaddr);   // MPUaddr адрес MPU6050
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

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
  static int posX = 0;
  static int posY = 0;
  if (millis() - prevTime >= 50) {
    prevTime = millis();
    getGyro();
    if (INVERT_Y_GYRO) GyY = -GyY;
    if (INVERT_X_GYRO) GyX = -GyX;
    GyX /= 400;
    GyY /= 400;
    posX += (float)(GyX - posX) * 0.1;
    posY += (float)(GyY - posY) * 0.1;       
    setEyeXY(posX, posY);
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

void getGyro() {
  Wire.beginTransmission(MPUaddr);
  Wire.write(0x43);  // starting with register 0x43 (GYRO_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPUaddr, 4, true); // request a total of 14 registers
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
}

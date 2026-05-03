#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include <ESP32Servo.h> 

const int motorPins[] = {1, 7, 5, 10}; 
Servo escs[4];
MPU6050 mpu(Wire);

bool isArmed = false;
int throttle = 1500;       
const int stopPulse = 1500; 
float offsetX = 0, offsetY = 0, offsetZ = 0;
int m[4];


float Kp = 1.3, Ki = 0.008, Kd = 0.5;
float KpYaw = 1.5, KiYaw = 0.0005, KdYaw = 0.4;

float iTermRoll = 0, iTermPitch = 0, iTermYaw = 0;
float lastRoll = 0, lastPitch = 0, lastYaw = 0;
float yaw_set = 0;

unsigned long lastTime, lastSerialTime;

void setup() {
  Serial.begin(115200);
  Wire.begin(3, 4);
  mpu.begin();
  
  Serial.println("Hay de drone nam yen!");
  mpu.calcGyroOffsets(true);
  
  for(int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }
  
  Dabble.begin("UAV_Dung_C3");
  lastTime = millis();
}

void loop() {
  Dabble.processInput();
  mpu.update();

  float roll_act  = mpu.getAngleX() - offsetX;
  float pitch_act = mpu.getAngleY() - offsetY;
  float yaw_act   = mpu.getAngleZ() - offsetZ;

  if (GamePad.isSquarePressed()) {
    isArmed = !isArmed;
    delay(500);
    if (isArmed) {
      for(int i = 0; i < 4; i++) {
        escs[i].setPeriodHertz(50);
        escs[i].attach(motorPins[i], 1000, 2000);
        escs[i].writeMicroseconds(stopPulse); 
      }
    } else {
      throttle = stopPulse;
      for(int i = 0; i < 4; i++) {
        escs[i].writeMicroseconds(stopPulse);
        delay(10);
        escs[i].detach();
      }
     
      iTermRoll = iTermPitch = iTermYaw = 0;
    }
  }

  //  (Circle): Reset góc về 0 (Sử dụng trước khi cất cánh)
  if (GamePad.isCirclePressed()) {
    offsetX = mpu.getAngleX();
    offsetY = mpu.getAngleY();
    offsetZ = mpu.getAngleZ();
    yaw_set = 0;
    iTermRoll = iTermPitch = iTermYaw = 0;
    delay(300);
  }

  if (isArmed) {
  
    if (GamePad.isTrianglePressed()) { throttle = constrain(throttle + 5, 1500, 1950); delay(100); }
    if (GamePad.isCrossPressed())    { throttle = constrain(throttle - 5, 1500, 1950); delay(100); }

    if (throttle > 1520) {
      unsigned long now = millis();
      float dt = (now - lastTime) / 1000.0;
      if (dt <= 0) dt = 0.01;
      lastTime = now;
// nhận lệnh định hướng
      float roll_set = 0, pitch_set = 0;
      if (GamePad.isUpPressed())    pitch_set = 12;
      if (GamePad.isDownPressed())  pitch_set = -12;
      if (GamePad.isLeftPressed())  roll_set  = -12;
      if (GamePad.isRightPressed()) roll_set  = 12;

    
      float errR = roll_set - roll_act;
      iTermRoll = constrain(iTermRoll + (errR * Ki * dt), -50, 50);
      float roll_out = constrain((errR * Kp) + iTermRoll - ((roll_act - lastRoll) / dt * Kd), -150, 150);
      lastRoll = roll_act;

      float errP = pitch_set - pitch_act;
      iTermPitch = constrain(iTermPitch + (errP * Ki * dt), -50, 50);
      float pitch_out = constrain((errP * Kp) + iTermPitch - ((pitch_act - lastPitch) / dt * Kd), -150, 150);
      lastPitch = pitch_act;

      // PID YAW (Chống trôi)
      float errY = yaw_set - yaw_act;
      if (abs(errY) < 1.2) errY = 0; // Vùng chết 1.2 độ để triệt tiêu trôi Gyro nhẹ

      if (abs(errY) > 0) {
        iTermYaw = constrain(iTermYaw + (errY * KiYaw * dt), -30, 30);
      } else {
        iTermYaw *= 0.9; // Xóa dần sai số tích lũy khi đứng yên
      }
      float yaw_out = constrain((errY * KpYaw) +    iTermYaw - ((yaw_act - lastYaw) / dt * KdYaw), -80, 80);
      lastYaw = yaw_act;

      
      m[0] = constrain(throttle + pitch_out + roll_out + yaw_out, 1500, 2000);
      m[1] = constrain(throttle + pitch_out - roll_out - yaw_out, 1500, 2000);
      m[2] = constrain(throttle - pitch_out + roll_out - yaw_out, 1500, 2000);
      m[3] = constrain(throttle - pitch_out - roll_out + yaw_out, 1500, 2000);

      for(int i = 0; i < 4; i++) escs[i].writeMicroseconds(m[i]);

    } else {
      // Giữ motor dừng hẳn ở mức ga thấp
      for(int i = 0; i < 4; i++) {
        escs[i].writeMicroseconds(stopPulse);
        m[i] = stopPulse;
      }
    }
  } else {
    // Ngắt tín hiệu khi chưa Arm
    for(int i = 0; i < 4; i++) digitalWrite(motorPins[i], LOW);
  }

  // Monitor dữ liệu
  if (millis() - lastSerialTime > 400) {
    Serial.printf("Thr:%d | PWM:[%d,%d,%d,%d] | Goc: R%.1f P%.1f Y%.1f\n", 
                  throttle, m[0], m[1], m[2], m[3], roll_act, pitch_act, yaw_act);
    lastSerialTime = millis();
  }
}
#include <stdio.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>

#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

#define normsp 2048
#define topsp 4095

#define C2 2
#define C3 3
#define C4 4
#define C5 5
#define C6 6
#define C7 7
#define C8 8
#define C9 9
#define C10 10
#define C11 11
#define C12 12
#define C13 13
#define C14 14
#define C15 15

#define rn 0
#define ln 0

PS2X ps2x;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setPWMMotors(int c8, int c9, int c10, int c11, int c12, int c13, int c14, int c15)
{
  pwm.setPWM(C8, 0, c8);
  pwm.setPWM(C9, 0, c9);
  pwm.setPWM(C10, 0, c10);
  pwm.setPWM(C11, 0, c11);
  pwm.setPWM(C12, 0, c12);
  pwm.setPWM(C13, 0, c13);
  pwm.setPWM(C14, 0, c14);
  pwm.setPWM(C15, 0, c15);
}

void setup() {
  Serial.begin(115200);

  Wire.begin();
  pwm.begin();

  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  Wire.setClock(400000);
  delay(10);
  setPWMMotors(0, 0, 0, 0, 0, 0, 0, 0);
  
  int err = -1;
  while (err != 0)
  {
    err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
  }
}

bool PS2control()
{
  int speed = normsp;
  if (ps2x.Button(PSB_R2))
  {
    speed = topsp;
  }

  int c8 = 0, c9 = 0, c10 = 0, c11 = 0, c12 = 0, c13 = 0, c14 = 0, c15 = 0;

  int joyx = 127 - ps2x.Analog(PSS_RX);
  int joyy = 128 - ps2x.Analog(PSS_LY);

  if (joyx == 0 && joyy > 0) 
  {
    c8 = joyy;
    c8 = map(c8, 0, 128, 0, speed);
    c10 = c8;
  }

  if (joyx == 0 && joyy < 0)
  {
    c9 = abs(joyy) + 1;
    c9 = map(c9, 0, 128, 0, speed);
    c11 = c9;
  }

  if (joyx > 0 && joyy == 0)
  {
    c10 = joyx + 1;
    c10 = map(c10, 0, 128, 0, speed);
  }

  if (joyx < 0 && joyy == 0)
  {
    c8 = abs(joyx);
    c8 = map(c8, 0, 128, 0, speed);
  }

  if (ps2x.ButtonPressed(PSB_CROSS)) 
  {
    for (uint16_t pulselen = 700; pulselen <= 1000; pulselen++) 
      {
        pwm.writeMicroseconds(C2, pulselen);
      }
    delay(500);
  }

  if (ps2x.ButtonPressed(PSB_TRIANGLE)) //  Trả về giá trị TRUE khi nút được ấn (từ tắt sang bật)
    {
      for (uint16_t pulselen = 1000; pulselen > 700; pulselen--) 
      {
        pwm.writeMicroseconds(C2, pulselen);
      }
      delay(500);
    }

    if (ps2x.ButtonPressed(PSB_R1)) //  Trả về giá trị TRUE khi nút được ấn (từ tắt sang bật)
    {
      for (uint16_t pulselen = 150; pulselen < 600; pulselen++) 
      {
        pwm.setPWM(C3, 0, pulselen);
      }
      delay(500);
    }


    if (ps2x.ButtonPressed(PSB_SQUARE))
    {
      for (uint16_t pulselen = 600; pulselen <= 601; pulselen++) 
      {
        pwm.writeMicroseconds(C3, pulselen);
      }
      delay(500);
    }

   /* if (ps2x.ButtonPressed(PSB_CIRCLE))
    {
      for (uint16_t pulselen = 2400; pulselen >= 1800; pulselen--) 
      {
        pwm.writeMicroseconds(C3, pulselen);
      }
      delay(500);
    }
    */
  bool quay1 = ln;
  if (ps2x.Button(PSB_CIRCLE))
  {
    quay1 =! quay1;
  }
  int angle4 = quay1 ? 1600 : 0;
  pwm.writeMicroseconds(C3, angle4);

  setPWMMotors(c8, c9, c10, c11, c12, c13, c14, c15);
  return 1;
}

void loop() {
  ps2x.read_gamepad(0, 0);
  PS2control();

  delay(50);
}

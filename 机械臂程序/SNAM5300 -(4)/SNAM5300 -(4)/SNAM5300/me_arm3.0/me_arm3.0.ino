#include "VarSpeedServo.h"
#include <EEPROM.h>
#include <Wire.h>
#define SLAVE_ADDRESS 0x04
#define HIGH_LED 3
VarSpeedServo servo1;
VarSpeedServo servo2;
VarSpeedServo servo3;
VarSpeedServo servo4;
int j=0;
int flag;

int val1;
int val2;
int val3;
int val4;

int x = 90;
int y = 40;
int z = 180;
int k = 0;       //舵机的初始位置。

int data[4];
int condition = 0; //控制机械臂的姿态模式,0:初始化 1:保持水平向前伸。2：机械臂伸直
int a;             //手背的翻转角度
int b;             //手背的俯仰角度
int c;             //手指与手背的角度差

bool latestates;
void setup()
{
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(HIGH_LED,OUTPUT);
  latestates=LOW;
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);
  // define callbacks for i2c communication
  Wire.onReceive(receiveData);//从机 接收 主机 发来的数据
  // Wire.onRequest(sendData); //从机 请求 主机 发送数据
  Serial.println("Ready");
  delay(100);
}
void loop()
{
  if (flag == 1)          //当所有数据都被接收后，分别赋值。
  {
    condition = data[0];
    a = data[1];
    b = data[2];
    c = data[3];
  }

 
  if (condition == 0)               //初始化舵机的角度
  {
    control_servo(90, 40, 180, 0);
  }
  else if (condition == 1)                   //舵机3与舵机2保持平行
  {
    if (a <= 180 && a >= 0 && b <= 90 && b >= 0)
    {
      control_servo(a, 130 - b, 90 + b, c);
    }
    if (a <= 180 && a >= 0 && b <= 130 && b > 90)
    {
      control_servo(a, 130 - b, 180, c);
    }
    if (a > 180 || a < 0 || b > 130 || b < 0)
    {
      Serial.println("gloves' angle is wrong!");
    }
  }
  else if (condition == 2)                       //机械臂伸直
  {
    if (a <= 180 && a >= 0)
    {
      control_servo(a, 130 - b, 90, c);
    }
    if (a > 180 || a < 0 || b > 130 || b < 0)
    {
      Serial.println("gloves' angle is wrong!");
    }
  }
  else if(condition==3)
  {
    latestates=!latestates;
    digitalWrite(HIGH_LED,latestates);
    delay(100);
  }
  else if (condition >= 4 || condition < 0)
  {
    Serial.println("Error: condition's wrong!");
  }


}
void receiveData(int byteCount)
{
  while (Wire.available())
  {
    data[j] = Wire.read();
    // Serial.print("data:");
    //Serial.println(data[i]);
    j++;
    flag = 0;
    if (j == 4)
    {
      j = 0; flag = 1;
    }
  }
}

void control_servo(int a, int b, int c, int d)
{
  for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
  {
    val1 = a;
    servopulse(a, 11);  //引用脉冲函数
  }


  for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
  {
    val2 = b;
    servopulse(b, 10);  //引用脉冲函数
  }

  for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
  {
    val3 = c;
    servopulse(c, 9);  //引用脉冲函数
  }

  for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
  {
    val4 = map(d, 0, 90, 0, 70);
    servopulse(d, 6);  //引用脉冲函数
  }
  Serial.print("val1=");
  Serial.print(val1);
  Serial.print(" val2=");
  Serial.print(val2);
  Serial.print(" val3=");
  Serial.print(val3);
  Serial.print(" val4=");
  Serial.println(val4);
}
void servopulse(int angle, int servopin) //定义一个脉冲函数
{
      int pulsewidth = (angle * 11) + 500;
      digitalWrite(servopin,HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(servopin,LOW);
      delayMicroseconds(20000-pulsewidth);
}

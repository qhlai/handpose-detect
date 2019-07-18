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
int latedata[4]={0};
int isfirst=0;

#define MAXSTEP 5

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

    if(a<0)
    {
      a=0;
    }
    if(a>180)
    {
      a=180;
    }
    if(b<0)
    {
      b=0;
    }
    if(b>130)
    {
      b=130;
    }
    if(c<0)
    {
      c=0;
    }
    if(c>180)
    {
      c=180;
    }
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
    control_servo(a,130-b,90+b,c);
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

void control_servo(int target0, int target1, int target2, int target3)
{
  
  int now0= latedata[0],now1= latedata[1],now2= latedata[2],now3= latedata[3];
  if((target0-latedata[0])>5)
  {
    for(int m =1; m<= (target0-latedata[0])/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val1 = target0;
        servopulse(now0, 11);  //引用脉冲函数
      }
      now0 += m*5;
    }
  }
  else if((target0-latedata[0])<-5)
  {
    for(int m =1; m<= (latedata[0]-target0)/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val1 = target0;
        servopulse(now0, 11);  //引用脉冲函数
      }
      now0 -= m*5;
    }
  }

  
   if((target1-latedata[1])>5)
  {
    for(int m =1; m<= (target1-latedata[1])/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val2 = target1;
        servopulse(now1, 10);  //引用脉冲函数
      }
      now1 += m*5;
    }
  }
  else if((target1-latedata[1])<-5)
  {
    for(int m =1; m<= (latedata[1]-target1)/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val2 = target1;
        servopulse(now1, 10);  //引用脉冲函数
      }
      now1 -= m*5;
    }
  }

 if((target2-latedata[2])>5)
  {
    for(int m =1; m<= (target2-latedata[2])/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val3 = target2;
        servopulse(now2, 9);  //引用脉冲函数
      }
      now2 += m*5;
    }
  }
  else if((target2-latedata[2])<-5)
  {
    for(int m =1; m<= (latedata[2]-target2)/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val3 = target2;
        servopulse(now2, 9);  //引用脉冲函数
      }
      now2 -= m*5;
    }
  }  

 if((target3-latedata[3])>5)
  {
    for(int m =1; m<= (target3-latedata[3])/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        val4 = target3;
        servopulse(now3, 6);  //引用脉冲函数
      }
      now3 += m*5;
    }
  }
  else if((target3-latedata[3])<-5)
  {
    for(int m =1; m<= (latedata[3]-target3)/5 ; m++)
    {
      for (int i = 0; i < 50; i++) //发送50个脉冲   //为什么是50个？
      {
        target3=map(target3, 0, 90, 0, 70);
        val4 = target3;
        now3 = map(now3, 0, 90, 0, 70);
        servopulse(now3, 6);  //引用脉冲函数
      }
      now3 -= m*5;
    }
  }  

  latedata[0]=target0;
  latedata[1]=target1;
  latedata[2]=target2;
  latedata[3]=target3;
  
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

#include "PID_v1.h"
    /*
    IDE 版本:1.0.1
    功能：利用SR04超声波传感器进行测距，并用串口显示测出的距离值
    */
    //slam
    
//0：前方；1：左；2：右；3：后
    // 设定SR04连接的Arduino引脚
    const int TrigPin[6] = {A0,A2,A4,2,4,6};
    const int EchoPin[6] = {A1,A3,A5,3,5,7};
    float distance[5];
    float Car_Velocity_Set = 0,Car_Angle_Set = 0;
    float Sense_Velocity[5] = {0};
    void setup()
    {  
      // 初始化串口通信及连接SR04的引脚
            Serial.begin(9600);
            pinMode(TrigPin[0], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[0], INPUT);
             pinMode(TrigPin[1], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[1], INPUT);
             pinMode(TrigPin[2], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[2], INPUT);
             pinMode(TrigPin[3], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[3], INPUT);
           /*  pinMode(TrigPin[4], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[4], INPUT);
             pinMode(TrigPin[5], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[5], INPUT);*/
        Serial.println("Ultrasonic sensor:");
    }
     /*************************************************
      * 功能：获取指定超声波数据，并保存
      **************************************************/
    void Get_Distance(int i, float *distance)
    {
            float tempdistance;
            // 产生一个10us的高脉冲去触发TrigPin
            digitalWrite(TrigPin[i], LOW);
            delayMicroseconds(2);
            digitalWrite(TrigPin[i], HIGH);
            delayMicroseconds(10);
            digitalWrite(TrigPin[i], LOW);
            //检测脉冲宽度，并计算出距离
            tempdistance = pulseIn(EchoPin[i], HIGH) / 58.00;
            if(tempdistance>400)
            {
              distance[i]= 400;
             }
             else if(tempdistance<4)
             {
                distance[i]= 4;
             }
            else
            {distance[i]= tempdistance;}
            
   }
      /*************************************************
      * 功能：获取所有超声波数据，并保存
      **************************************************/
   void Get_All_Distance(int num)
   {
    for(int i = 0; i< num; i++)
          {
            Get_Distance( i, distance);
          }
    }
     /*************************************************
      * 功能：pid
      **************************************************/
      #define PERIOD 20
      float u = 0;
      float eI;
      float eII;
      float eIII;
      //PID控制器，初始值供参考，Kp= 1,Ti = 10,T = PERIOD
      float pidController(float target, float current)
      {
        float output;
        eI = target - current;
        float Kp = 0.5, Ti = 5, Td = 0, T = PERIOD;
        float q0 = Kp * (1 + T / Ti + Td / T);
        float q1 = -Kp * (1 + 2 * Td / T);
        float q2 = Kp * Td / T;
        u = u + q0 * eI + q1 * eII + q2 * eIII;
        eIII = eII;
        eII = eI;
       if(u >= 255)
        {
          u = 255;
        }
        if(u <= -255)
        {
          u = -255;
        }
        output = u;
        return output;
      }
       /*************************************************
      * 函数
      * 功能：展示所有超声波数据
      **************************************************/
      void Display(int num)
      {
          //int count = 0;
          for(int i = 0; i< num; i++)
          {
           Get_Distance(i,  distance);
          Serial.print("distance[");
          Serial.print(i);
          Serial.print("] = ");
          Serial.print(distance[i]);
          Serial.print("cm ");
          Serial.println();
          /*count++
          if(count == 3)
          {
          Serial.println();
          }
          else if(count == 6)
          {
            Serial.println();
            Serial.println();
            count=0;
          }*/
        }
    }
      /*************************************************
      * 函数
      * 功能：避障
      **************************************************/
      void Avoid_Obstacle()
      {
        
        /*************************************************
      * 设备：前置超声波
      * 功能：减速防止前方碰撞，并在很近距离时后退
      **************************************************/
     if(distance[0]< 50)//前
     {
       Car_Velocity_Set = 10;
       Car_Angle_Set = 0;
        if(distance[0]< 20)
       {
            Car_Velocity_Set = 5;
            Car_Angle_Set = 0;
           if(distance[0]< 10)
            {
                Car_Velocity_Set = -5;//后退以保持距离
                Car_Angle_Set = 0;
            }
        }
      }
      /*************************************************
      * 设备：后置超声波
      * 功能：加速防止后方碰撞
      **************************************************/
      if(distance[3]< 20)//后
       {
            Car_Velocity_Set = 12;
            Car_Angle_Set = 0;
           if(distance[3]< 10)
            {
                Car_Velocity_Set = 15;
                Car_Angle_Set = 0;
            }
        }
      /*************************************************
      * 设备：前置超声波，左超声波双 融合
      * 功能：左斜将碰墙时自动纠偏
      * 备注：舵机右正左负
      **************************************************/
     if(distance[0]< 30)//前
     {
       //Car_Velocity_Set = 10;
      // Car_Angle_Set = 0;
           if(distance[1]< 25)//左
                {
                    Car_Velocity_Set = 5;
                    Car_Angle_Set = 5;
                }
            else if(distance[1]< 15)//左
            {
               Car_Velocity_Set = 5;
               Car_Angle_Set = 10;
              }
              else if(distance[1]< 8)//左
            {
               Car_Velocity_Set = 5;
               Car_Angle_Set = 15;
              }
     }
      eles if(distance[0]< 15)//前
       {
            Car_Velocity_Set = 5;
            Car_Angle_Set = 0;
                 if(distance[1]< 25)//左
                  {
                      Car_Velocity_Set = 5;
                      Car_Angle_Set = 20;
                  }
                else if(distance[1]< 15)//左
                {
                 Car_Velocity_Set = 5;
                 Car_Angle_Set = 20;
                }
                else if(distance[1]< 8)//左
                {
                 Car_Velocity_Set = 5;
                 Car_Angle_Set = 30;
                }
        }
        eles if(distance[0]< 5)//前
       {
            Car_Velocity_Set = -5;
            Car_Angle_Set = 0;
                 if(distance[1]< 25)//左
                 {
                      Car_Velocity_Set = 5;
                      Car_Angle_Set = 20;
                 }
                  else if(distance[1]< 15)//左
                {
                 Car_Velocity_Set = 5;
                 Car_Angle_Set = 30;
                }
                else if(distance[1]< 8)//左
                {
                 Car_Velocity_Set = 5;
                 Car_Angle_Set = 45;
                }
        }
    }
    void loop()
    {
      Get_All_Distance(4);
      Display(4);
      Avoid_Obstacle()
    }

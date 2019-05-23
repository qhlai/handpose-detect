#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
 
MPU6050 mpu; //实例化一个 MPU6050 对象，对象名称为 mpu
int16_t ax, ay, az, gx, gy, gz;

#define C1 2
#define C2 3
#define C3 4
#define C4 5
#define C5 6
#define C6 7
#define C7 8
#define C8 9

int i = 1;
int data[8][6] =  {0};
 
//********************angle data*********************//
float Gyro_y; //Y轴陀螺仪数据暂存
float Gyro_x; //X轴陀螺仪数据暂存
float Gyro_z; //Z轴陀螺仪数据暂存
float angleAx;
float angle6;
float K1 = 0.05; // 对加速度计取值的权重
float Angle; //一阶互补滤波计】算出的小车最终倾斜角度
float accelz = 0;
 
//********************angle data*********************//
 
//***************Kalman_Filter*********************//
float P[2][2] = {{ 1, 0 },
  { 0, 1 }
};
float Pdot[4] = { 0, 0, 0, 0};
float Q_angle = 0.001, Q_gyro = 0.005; //角度数据置信度,角速度数据置信度
float R_angle = 0.5 , C_0 = 1;
float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;
float timeChange = 5; //滤波法采样时间间隔毫秒
float dt = timeChange * 0.001; //注意：dt的取值为滤波器采样时间
//***************Kalman_Filter*********************//
 
void Angletest()
{
  //平衡参数
  Angle = atan2(ay , az) * 57.3;           //角度计算公式
  Gyro_x = (gx - 128.1) / 131;              //角度转换
  Kalman_Filter(Angle, Gyro_x);            //卡曼滤波
  //旋转角度Z轴参数
  if (gz > 32768) gz -= 65536;              //强制转换2g  1g
  Gyro_z = -gz / 131;                      //Z轴参数转换
  accelz = az / 16.4;
 
  angleAx = atan2(ax, az) * 180 / PI; //计算与x轴夹角
  Gyro_y = -gy / 131.00; //计算角速度
  //一阶互补滤波
  angle6 = K1 * angleAx + (1 - K1) * (angle6 + Gyro_y * dt);
}
 
////////////////////////kalman/////////////////////////
float angle, angle_dot;                                //平衡角度值
void Kalman_Filter(double angle_m, double gyro_m)
{
  angle += (gyro_m - q_bias) * dt;
  angle_err = angle_m - angle;
  Pdot[0] = Q_angle - P[0][1] - P[1][0];
  Pdot[1] = - P[1][1];
  Pdot[2] = - P[1][1];
  Pdot[3] = Q_gyro;
  P[0][0] += Pdot[0] * dt;
  P[0][1] += Pdot[1] * dt;
  P[1][0] += Pdot[2] * dt;
  P[1][1] += Pdot[3] * dt;
  PCt_0 = C_0 * P[0][0];
  PCt_1 = C_0 * P[1][0];
  E = R_angle + C_0 * PCt_0;
  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;
  t_0 = PCt_0;
  t_1 = C_0 * P[0][1];
  P[0][0] -= K_0 * t_0;
  P[0][1] -= K_0 * t_1;
  P[1][0] -= K_1 * t_0;
  P[1][1] -= K_1 * t_1;
  angle += K_0 * angle_err; //角度
  q_bias += K_1 * angle_err;
  angle_dot = gyro_m - q_bias; //角速度
}

 void printonscreen(void)
 {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);     //IIC获取MPU6050六轴数据 ax ay az gx gy gz  
 
  Angletest();                                      //获取angle 角度和卡曼滤波
  Serial.print(ax/2048);Serial.print(",");
  Serial.print(ay/2048);Serial.print(",");
  Serial.print(az/2048);Serial.print("---");
  Serial.print(angle);Serial.print(",");
  Serial.print(angle_dot);Serial.print(",");
  Serial.println(angle6);

  ax = 0;
  ay = 0;
  az = 0;
  angle = 0;
  angle_dot = 0;
  angle6 = 0; 
 
  }
 
void setup() {
  Wire.begin();          //加入 I2C 总线序列
  Serial.begin(9600);    //开启串口，设置波特率
  delay(1000);
  mpu.initialize();//初始化MPU6050
  pinMode(C1,OUTPUT);
  pinMode(C2,OUTPUT);
  pinMode(C3,OUTPUT);
  pinMode(C4,OUTPUT);
  pinMode(C5,OUTPUT);
  pinMode(C6,OUTPUT);
  pinMode(C7,OUTPUT);
  pinMode(C8,OUTPUT);
  
  digitalWrite(C1,LOW);
  digitalWrite(C2,LOW);
  digitalWrite(C3,LOW);
  digitalWrite(C4,LOW);
  digitalWrite(C5,LOW);
  digitalWrite(C6,LOW);
  digitalWrite(C7,LOW);
  digitalWrite(C8,LOW);
  //digitalWrite(C1,HIGH);
}
 
void loop() {
  //mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);     //IIC获取MPU6050六轴数据 ax ay az gx gy gz  
 
 // Angletest();                                      //获取angle 角度和卡曼滤波
 if(i == 1)
 {
  digitalWrite(C1,LOW);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,HIGH);
  
  Serial.println("NO.1");
  
  printonscreen();
  i++;
  }
  else if(i == 2)
  {
    digitalWrite(C1,HIGH);
  digitalWrite(C2,LOW);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,HIGH);
  
  Serial.println("NO.2");
   printonscreen();
   i++;
    }
    else if(i == 3)
  {
    digitalWrite(C1,HIGH);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,LOW);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,HIGH);
  
  Serial.println("NO.3");
   printonscreen();
   i++;
    }
    else if(i == 4)
  {
    digitalWrite(C1,HIGH);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,LOW);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,HIGH);
  Serial.println("NO.4");
   printonscreen();
   i++;
    }
    else if(i == 5)
  {
    digitalWrite(C1,HIGH);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,LOW);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,HIGH);
  ;
  Serial.println("NO.5");
   printonscreen();
   //i++;
   i=1;
    }
   /* else if(i == 6)
  {
   digitalWrite(C1,HIGH);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,LOW);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,HIGH);
  
  Serial.println("NO.6");
   printonscreen();
   i++;
    }
    else if(i == 7)
  {
   digitalWrite(C1,HIGH);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,LOW);
  digitalWrite(C8,HIGH);
  Serial.println("NO.7");
   printonscreen();
   i++;
    }
    else if(i == 8)
  {
   digitalWrite(C1,HIGH);
  digitalWrite(C2,HIGH);
  digitalWrite(C3,HIGH);
  digitalWrite(C4,HIGH);
  digitalWrite(C5,HIGH);
  digitalWrite(C6,HIGH);
  digitalWrite(C7,HIGH);
  digitalWrite(C8,LOW);
  
  Serial.println("NO.8");
   printonscreen();
   i=1;
    }*/
  
  /*Serial.print(ax/2048);Serial.print(",");
  Serial.print(ay/2048);Serial.print(",");
  Serial.print(az/2048);Serial.print("---");
  Serial.print(angle);Serial.print(",");
  Serial.print(angle_dot);Serial.print(",");
  Serial.println(angle6);*/
 
  
  
  delay(500);
}

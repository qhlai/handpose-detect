/*****************************
void judge_gesture(GESTURE *g)//判断姿势
入口参数：GESTURE 结构体
void Angle_get()//计算角度
void Kalman_Filter(double angle_m, double gyro_m)//卡尔曼滤波
void tcpsend_procceed(char test[16] , float data, int i, int j, int m)//tcp传输
入口参数：**********************************需添加
void Get_GMCDATA()//获取QMC5883L的三轴磁场数据
主要参数：pitch角：angle6，roll角：angle
*****************************/


#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SoftwareSerial.h> //软件串口
 
MPU6050 mpu; //实例化一个 MPU6050 对象，对象名称为 mpu

SoftwareSerial mySerial(10, 11); ////软件串口 指定pin 对应esp8266的RX, TX
//********************传输部分参数************************//
//char posedata[8][6] = {0};//mpu数据暂存
char test[16] ={0};
char test1[2 ]={0};
int i;
int choose = 0;

#define C1 2
#define C2 3
//*********************MPU6050初始数据************************//
int16_t ax, ay, az, gx, gy, gz;
int16_t data[8][8];

float axt, ayt, azt;

//float ax, ay, az, gx, gy, gz;
 
//********************angle data*********************//
float Gyro_y; //Y轴陀螺仪数据暂存
float Gyro_x;
float Gyro_z;
float angleAx;
float angle6;
float K1 = 1.5; // 对加速度计取值的权重
//float K1 = 0.05; // 对加速度计取值的权重
float Angle; //一阶互补滤波计算出的最终倾斜角度
float accelz = 0;
float calc_yaw=0.0f;//计算所得出的偏航角
 
//********************angle data*********************//
 
//***************Kalman_Filter*********************//
float P[2][2] = {{ 1, 0 },
  { 0, 1 }
};
float Pdot[4] = { 0, 0, 0, 0};
//float Q_angle = 0.001, Q_gyro = 0.005; //角度数据置信度,角速度数据置信度
float Q_angle = 2, Q_gyro = 2; //角度数据置信度,角速度数据置信度
float R_angle = 0.5 , C_0 = 1;
float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;
float timeChange = 10; //滤波法采样时间间隔毫秒
float dt = timeChange * 0.003; //注意：dt的取值为滤波器采样时间
//***************Kalman_Filter*********************//
//******************判断姿势的一些常量*************************//
#define UP 1//定义姿势状态整形参数 1：向上 2：向下 3：中等向上 4：中等向下
#define DOWN 2
#define RIGHT 1
#define LEFT 2
//以下应用在新版本V1.1级以上
#define LITTLE_UP 10//定义可检测到的敏感角度 10°
#define LITTLE_DOWN -10
#define LITTLE_LEFT -10
#define LITTLE_RIGHT 10
/////////////////////////////////////////////////////////
float sum_pitch=0.0f;//多次pitch角的和
float ave_pitch=0.0f;//picth角的平均值
float sum_roll=0.0f;
float ave_roll=0.0f;
int count=0;//定义计数器

//*********************gy-271-QMC5883L的一些参数*********************//
#define ADDRESS 0x0D //QMC5883L的设备地址设置
int G_x,G_y,G_z;//以高斯为单位的测出的磁场强度（QMC5883L输出的原始数据）
float Mx,My,Mz;//以特斯拉为单位的转换后的磁场强度




typedef struct gesture
{
  float up_down;//上下参数
  float left_right;//左右参数
  float pitch;//俯仰角
  float roll;//翻滚角
  int equipment;//设备号
}GESTURE;
//****************MPU6050数据结构体************************//
typedef struct device
{
  float ax;//参数
  float ay;//参数
  float az;//参数
  
  float gx;//参数
  float gy;//参数
  float gz;//参数
  
  float yaw;//偏航角
  float pitch;//俯仰角
  float roll;//翻滚角
  int equipment;
  
}DEVICE;
DEVICE device[6];//定义六个MPU6050设备
void Angle_get()
{

  float heading_yaw;
  const float declinationAngle = 0.0457;//角度偏差校准(磁偏角)

  
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
//***********计算偏航角***************//
  heading_yaw = atan2(G_y,G_x);//计算yaw
   heading_yaw += declinationAngle;
   if(heading_yaw < 0)
   {
      heading_yaw += 2*PI;
   }
   if(heading_yaw > 2*PI)
   {
     heading_yaw -= 2*PI;
   }
   calc_yaw = heading_yaw * 180/PI; 
 
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

/* /////////SEND////
void tcpsend_all(int num,char test[16] , float data1, float data2, float data3,float data4,float data5,float data6)
{
 
  
 // mySerial.write(str,2);           //向esp8266写数据
 
  dtostrf(angle,3, 2, test);
  
mySerial.write(35);//ASCLL #井号
  for( i = 0; i < 4; i++)
  {
    
   
    delay(5);
  mySerial.write(test[i]);//ASCLL
  delay(5);
  }
  mySerial.write(35);//ASCLL #井号
  
  }*/

   /////////SEND////
void tcpsend_procceed(char test[16] , float data, int i, int j, int m)
{
 
        //i转换后整数部分长度
        //j转换后小数部分长度
        //m传入数据长度ascll m<8
       // mySerial.write(str,2);           
       
        dtostrf(data,i, j, test);//保存到该char数组中。
        //dtostrf(signalsign,2, 0, test1);//保存到该char数组中。
        
      
       for( i = 0; i < m; i++)
        {
        mySerial.write(test[i]);//ASCLL 向esp8266写数据
        //delay(5);
        }
        //mySerial.write(35);//ASCLL #井号,分隔符
        test[16] = {0};//reset char
}

void transmit_1(GESTURE *g)//发送数据
{
  //transmit
       mySerial.write(36);//ASCLL $号,占位符
       mySerial.write(64);//ASCLL @号,标志符
       tcpsend_procceed(test , 1.0, 1, 0, 2);//设备编号
       mySerial.write(35);//ASCLL #井号，分隔符
       tcpsend_procceed(test , g->up_down, 1, 0, 2);
       mySerial.write(35);//ASCLL #井号
       tcpsend_procceed(test , g->left_right, 1, 0, 2);
       mySerial.write(35);//ASCLL #井号
       tcpsend_procceed(test , g->pitch, 1, 0, 2);
       mySerial.write(35);//ASCLL #井号
       tcpsend_procceed(test , g->roll, 3, 2, 5);
       mySerial.write(33);//ASCLL !号，结束符
}
/*void transmit_2(DEVICE *g)//发送数据,另设一个发送函数,未修改完
{
  //transmit
       mySerial.write(36);//ASCLL $号,占位符
       mySerial.write(64);//ASCLL @号,标志符
       tcpsend_procceed(test , 1.0, 1, 0, 2);//发送数据包类型判断
       mySerial.write(35);//ASCLL #井号，分隔符
       tcpsend_procceed(test , 1.0, 1, 0, 2);//设备编号
       mySerial.write(35);//ASCLL #井号，分隔符
       tcpsend_procceed(test , g->up_down, 1, 0, 2);
       mySerial.write(35);//ASCLL #井号
       tcpsend_procceed(test , g->left_right, 1, 0, 2);
       mySerial.write(35);//ASCLL #井号
       tcpsend_procceed(test , g->roll, 1, 0, 2);
       mySerial.write(35);//ASCLL #井号
       tcpsend_procceed(test , g->pitch, 3, 2, 5);
       mySerial.write(33);//ASCLL !号，结束符
}*/
void getdata(int num)
{
  for(count = 0;count <5;count++)
    {
      mpu.getMotion6(&ax, &ay ,&az, &gx, &gy, &gz);     //IIC获取MPU6050六轴数据 ax ay az gx gy gz  
      

     
      Angle_get();                                      //获取angle 角度和卡曼滤波
    
          //求平均数，判断姿势
          //getsum(angle6,angle);
          sum_pitch+=angle6;
          sum_roll+=angle;//angle好像是偏航角
    }
      ave_pitch=sum_pitch/5;
      ave_roll=sum_roll/5;
      
      device[num].ax = ax;
      device[num].ay = ay;
      device[num].az = az;
      
      device[num].gx = gx;
      device[num].gy = gy;
      device[num].gz = gz;
      
      //device[num].yaw = angle ;
      device[num].roll = angle;
      device[num].pitch = angle6; 

      device[num].equipment = num;
}
 
void setup() 
{
    Wire.begin();                            //加入 I2C 总线序列
    Serial.begin(9600);                       //开启串口，设置波特率
    mySerial.begin(9600); 
    delay(1000);
    mpu.initialize();                       //初始化MPU6050
  
    pinMode(C1,OUTPUT);
    pinMode(C2,OUTPUT);
    
    digitalWrite(C1,LOW);//默认从手背开始
    digitalWrite(C2,HIGH);

    Wire.beginTransmission(ADDRESS);//选择QMC5883L物理地址，向其中写初始化参数
    Wire.write(0x09); //选择控制寄存器0X09
    Wire.write(0x1D); //设置0x09寄存器为：OSR:512Hz，RNG：+/-8Gauss，ODG：200Hz，MODE：连续工作模式
    Wire.endTransmission();//结束
}
 
void loop() 
{
      while(mySerial.available())           //从esp8266读数据
      {
        Serial.write(mySerial.read());
      }
      
      
           //recive
       while(mySerial.available())           //从esp8266读数据
      {
        Serial.write(mySerial.read());

      }
  
 
    GESTURE real_gesture;
      //手背
      if( choose == 0)//choose 实际是0，定义choose=0 为手背
       {
         getdata(0);
         
         axt = float(ax) / 2048 ;
         ayt = float(ay) / 2048 ;
         azt = float(az) / 2048;

 
        Serial.print("ax: ");Serial.print(axt);Serial.print(",");
        Serial.print("ay: ");Serial.print(ayt);Serial.print(",");
        Serial.print("az: ");Serial.print(azt);Serial.print("---");
         
        Serial.print("angle: ");Serial.print(angle);Serial.print(",");
        Serial.print("angle_dot: ");Serial.print(angle_dot);Serial.print(",");
        Serial.print("angle6: ");Serial.println(angle6);
         judge_gesture(&real_gesture);//获取姿势参数
         real_gesture.equipment = 0;
         count=0;//进行初始化操作
         sum_pitch=0;
         sum_roll=0;
         Serial.print(real_gesture.up_down);
         Serial.print("\t");
         Serial.print(real_gesture.left_right);
         Serial.print("\t");
         Serial.print(real_gesture.pitch);
         Serial.print("\n");

         transmit_1(&real_gesture);//调用发送数据函数
         //delay(20);
       }
    //大拇指
    else if(choose == 1)
    {
      getdata(1);
      //real_gesture.equipment = 0;
      axt = float(ax) / 2048 ;
      ayt = float(ay) / 2048 ;
      azt = float(az) / 2048;

 
      Serial.print("ax: ");Serial.print(axt);Serial.print(",");
      Serial.print("ay: ");Serial.print(ayt);Serial.print(",");
      Serial.print("az: ");Serial.print(azt);Serial.print("---");
       
      Serial.print("angle: ");Serial.print(angle);Serial.print(",");
      Serial.print("angle_dot: ");Serial.print(angle_dot);Serial.print(",");
      Serial.print("angle6: ");Serial.println(angle6);
      }
       
  //可视化分析
  /*
  axt = float(ax) / 2048 ;
  ayt = float(ay) / 2048 ;
  azt = float(az) / 2048;

 
  Serial.print("ax: ");Serial.print(axt);Serial.print(",");
  Serial.print("ay: ");Serial.print(ayt);Serial.print(",");
  Serial.print("az: ");Serial.print(azt);Serial.print("---");*/

  
 /* Serial.print("angle: ");Serial.print(angle);Serial.print(",");
  Serial.print("angle_dot: ");Serial.print(angle_dot);Serial.print(",");
  Serial.print("angle6: ");Serial.println(angle6);*/
  /*
  Serial.print("ax: ");Serial.print(ax);Serial.print(",");
  Serial.print("ay: ");Serial.print(ay);Serial.print(",");
  Serial.print("az: ");Serial.print(az);Serial.print("---");
  Serial.print("angle: ");Serial.print(angle);Serial.print(",");
  Serial.print("angle_dot: ");Serial.print(angle_dot);Serial.print(",");
  Serial.print("angle6: ");Serial.println(angle6);
  */
   if(choose == 0)//选择
  {
      digitalWrite(C1,HIGH);
      digitalWrite(C2,LOW);
      //Serial.println("NO.1");
      choose = 1;//轮换
  }
  else
  {
      digitalWrite(C1,LOW);
      digitalWrite(C2,HIGH);
      choose = 0;
  }
  Serial.println(choose);
  delay(50);

  Get_GMCDATA();//获取三轴磁场
}


/*void getsum(float fNewPitch,float fNewRoll)//求一段时间内算得的pitch角的和值
{
  sum_pitch+=fNewPitch;
  sum_roll+=fNewRoll;
  count++;
}*/
void Get_GMCDATA()
{
  Wire.beginTransmission(ADDRESS);//读写开始
  Wire.write(0x00); //选择X,Y，Z所在数据储存寄存器
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS, 6);
   if(6<=Wire.available())//注意：国产QMC5883L顺序为xyz，进口顺序为xzy
   {
     G_x = Wire.read()<<8; //X msb
     G_x |= Wire.read(); //X lsb
     G_y = Wire.read()<<8; //Z msb
     G_y |= Wire.read(); //Z lsb
     G_z = Wire.read()<<8; //Y msb
     G_z |= Wire.read(); //Y lsb
   }
   Mx=(float)G_x;
   My=(float)G_y;
   Mz=(float)G_z;
 //打印坐标到串口
  Mx/=10000;
  My/=10000;
  Mz/=10000;//转换为特斯拉
  Serial.print("x: ");
   Serial.print(Mx);
   Serial.print("  y: ");
   Serial.print(My);
   Serial.print("  z: ");
   Serial.println(Mz); 
   delay(250);
}


void judge_gesture(GESTURE *g)//判断姿势，返回值为 1234 ，1：向上，2：向下
{
  //ave_pitch=sum_pitch/count;
  //ave_roll=sum_roll/count;
  g->pitch=ave_pitch;//角度值赋值
  g->roll=ave_roll;//需要发送
  if(ave_pitch>=LITTLE_UP)//判定角度，如果大于25°,小于45,则向中上，此处可更改
  {
    g->up_down=UP;
  }
  else if(ave_pitch<=LITTLE_DOWN)
  {
    g->up_down=DOWN;
  }
  else 
  {
    g->up_down=0;
  }
  if(ave_roll>=LITTLE_RIGHT)//判定角度，如果x向右大于25°,小于45,则向中右，此处可更改
  {
    g->left_right=RIGHT;
  }
  else if(ave_roll<=LITTLE_LEFT)
  {
    g->left_right=LEFT;
  }
  else 
  {
    g->left_right=0;
  }
}

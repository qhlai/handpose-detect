#include <stdio.h>
#include <stdlib.h>

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SoftwareSerial.h> //软件串口

//5.24 修改发送程序
/*
****************************
void judge_gesture(GESTURE *g)//判断姿势
void Angle_get()//计算角度
void Kalman_Filter(double angle_m, double gyro_m)//卡尔曼滤波
void tcpsend_procceed(char test[16] , float data, int i, int j, int m)//tcp传输
****************************
*/
/*
 * reference:
 *MPU6050 I2C master模式调试记录 https://blog.csdn.net/airk000/article/details/22945573?tdsourcetag=s_pcqq_aiomsg
 *QMC5883L寄存器对比及参考设置 https://wenku.baidu.com/view/39238426b14e852459fb57e1.html
 *最详细的MPU6050寄存器说明手册-中文 https://wenku.baidu.com/view/f48294eef4335a8102d276a20029bd64793e6264.html
 *HMC5883L手册总结与经验分析 详解 https://blog.csdn.net/zsn15702422216/article/details/52223841
 */
  uint8_t bump[17]={0};//寄存器
  uint8_t temphex[14];//寄存器数据缓存
/*************************
void judge_gesture(GESTURE *g)//判断姿势
入口参数：GESTURE 结构体
void Angle_get()//计算角度
void Kalman_Filter(double angle_m, double gyro_m)//卡尔曼滤波
void tcpsend_procceed(char test[16] , float data, int i, int j, int m)//tcp传输
入口参数：**********************************需添加
void Get_GMCDATA()//获取QMC5883L的三轴磁场数据
主要参数：pitch角：angle6，roll角：angle
*****************************/



MPU6050 mpu; /*实例化一个 MPU6050 对象，对象名称为 mpu*/

SoftwareSerial mySerial(10, 11); /*软件串口 指定pin 对应esp8266的RX, TX*/


char test[16] ={0};
char test1[2 ]={0};
int i;
float velocity;

/*********选择MPU参数***********/
int choose = 0;
#define DEBUG 1
#define C1 2
#define C2 3    /*转换地址引脚*/


/**********开关参数************/
#define SW 6//定义开关转换控制机械臂引脚 为6
#define LED1 5//定义LED灯引脚 为5,7,8
#define LED2 7
#define LED3 8
boolean lastState = LOW;//记录上次的开关状态，初始化为LOW
int click1 = 0;//开关信号的改变次数，初始化为0
int open_status;
/*OPEN_GPIO_HAND 为高电平时且OPEN_GPIO为低电平时，为机械臂控制模式
 * 二者都为低时，为手背控制
 * 前高后低时，为特殊手势控制
 * 都为高时停止状态
 */


int16_t ax, ay, az, gx, gy, gz;
int16_t data[8][8];

float acc_angle[4];//向量计算出的值，三维夹角，x方向夹角，y方向，z方向

//float axt, ayt, azt;


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

//*******************************************************//

/*************************************************
      * 定义
      * 功能：卡尔曼滤波
**************************************************/
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
//**********************************************************//

 //*********************gy-271-QMC5883L的一些参数*********************//
#define ADDRESS 0x0D //QMC5883L的设备地址设置
int G_x,G_y,G_z;//以高斯为单位的测出的磁场强度（QMC5883L输出的原始数据）
float Mx,My,Mz;//以特斯拉为单位的转换后的磁场强度
 /******************************************************************/

 /*******************有关手背手势判断的参数********************/
#define UP 1//定义姿势状态整形参数 1：向上 2：向下
#define DOWN 2
#define RIGHT 1
#define LEFT 2
#define N 2//目前安装的MPU6050的个数
#define OPEN 1//定义开关开
#define UNOPEN 0//定义开关关
//以下应用在新版本V1.1级以上
#define LITTLE_UP 10//定义可检测到的敏感角度 10°
#define LITTLE_DOWN -10
#define LITTLE_LEFT -10
#define LITTLE_RIGHT 10
/////////////////////////////////////////////////////////


float pre_arr[3][11]={0};//翻手
int counter=0;//定义翻手计数器
//opisthenar 手背
typedef struct gesture
{
  float up_down;//上下参数
  float left_right;//左右参数
  float pitch;//俯仰角
  float roll;//翻滚角
  int equipment;//设备号
}GESTURE;
GESTURE real_gesture;
typedef struct twogesture
{
  //******以下均为1真0假*******//
  int none;//手平放，表示停止某个动作
  int opposite;//手心朝上
  int two_times_down;//向下翻动手指2次
  int fist;//握拳
  int thumb;//灯光(大拇指)
  float finalnum;//要发送的最终参数，由上到下分别为0，1，2，3，4。
}TWOGESTURE;
    
    TWOGESTURE two_gesture;
//****************MPU6050数据结构体************************//
typedef struct device1
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
DEVICE device[N];
void Angle_get()
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
  ///陀螺仪积分角度（先验估计）
  angle += (gyro_m - q_bias) * dt;//带q的是变量的协方差，带m的是测量值
  ///计算角度偏差
  angle_err = angle_m - angle;//偏差e
  ///先验估计误差协方差的微分
  Pdot[0] = Q_angle - P[0][1] - P[1][0];
  Pdot[1] = - P[1][1];
  Pdot[2] = - P[1][1];
  Pdot[3] = Q_gyro;
  ///先验估计误差协方差的积分
  P[0][0] += Pdot[0] * dt;
  P[0][1] += Pdot[1] * dt;
  P[1][0] += Pdot[2] * dt;
  P[1][1] += Pdot[3] * dt;
  ///卡尔曼增益计算
  PCt_0 = C_0 * P[0][0];
  PCt_1 = C_0 * P[1][0];
  E = R_angle + C_0 * PCt_0;
  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;
  ///后验估计误差协方差计算 
  t_0 = PCt_0;
  t_1 = C_0 * P[0][1];
  P[0][0] -= K_0 * t_0;
  P[0][1] -= K_0 * t_1;
  P[1][0] -= K_1 * t_0;
  P[1][1] -= K_1 * t_1;
  angle += K_0 * angle_err; ///后验估计最优角度值
  q_bias += K_1 * angle_err;///更新最优估计值的偏差
  angle_dot = gyro_m - q_bias; ///更新最优角速度值
}
void Get_QMC5883_mpu6050()
{
   writeByte(0x68, 0x6B, 0x0);//enable mpu6050 from sleep

   writeByte(0x68, 0x6A, 0x20);//enable i2c master mode
   writeByte(0x68, 0x24, 0x0D); //只设置速率 400khz
   writeByte(0x68, 0x25, 0x0D);//写(read:8d,write:0d)
   writeByte(0x68, 0x26, 0x09);//qmcmodreg
   writeByte(0x68, 0x63, 0x1D);//mode config
   writeByte(0x68, 0x27, 0x01);//will write
   delay(1);
   writeByte(0x68, 0x27, 0x81);//enable write 1 reg

   writeByte(0x68, 0x27, 0x01);//disable write
   writeByte(0x68, 0x25, 0x8D);//read mode
   writeByte(0x68, 0x26, 0x00);//slave reg begin
   writeByte(0x68, 0x27, 0x06);//will read 6 registers
   writeByte(0x68, 0x27, 0x86);//read 6 regs
   delay(1);
   writeByte(0x68, 0x27, 0x06);//disable read 6 registers
   
  Wire.beginTransmission(0x68);//读写开始
  Wire.write(0x49); //选择X,Y，Z所在数据储存寄存器
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6);
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
  /*Serial.print("x: ");
   Serial.print(Mx);
   Serial.print("  y: ");
   Serial.print(My);
   Serial.print("  z: ");
   Serial.println(Mz); */
}
void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);  // Initialize the Tx buffer
  Wire.write(subAddress);           // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
}
 uint8_t readByte(uint8_t address, uint8_t subAddress)
{
  uint8_t data; // `data` will store the register data   
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);                  // Put slave register address in Tx buffer
  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
  data = Wire.read();                      // Fill Rx buffer with result
  return data;                             // Return data read from slave register
}

void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{  //readBytes(MPU6050_ADDRESS, XA_OFFSET_H, 2, &data[0]);
  Wire.beginTransmission(address);   // Initialize the Tx buffer
  Wire.write(subAddress);            // Put slave register address in Tx buffer
  Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
  uint8_t i = 0;
        Wire.requestFrom(address, count);  // Read bytes from slave register address 
  while (Wire.available()) {
        dest[i++] = Wire.read(); }         // Put read results in the Rx buffer
}

uint8_t getAddress(uint8_t addr) 
{
   //if (num > 3) return 0;
    I2Cdev::readByte(0x68, addr, temphex);
    return temphex[0];
}
void Get_i2cdump(uint8_t num)//display all register data,(usedfor debug)
{
  
    for(int8_t count = 0; count < num; count++){
        
       Serial.print("<");
       Serial.print("0x");
       Serial.print(count,HEX);
       Serial.print("> ");
       Serial.print("0x");
         Serial.print(getAddress(count), HEX);
       //Serial.print(bump[count],HEX);
       Serial.print(" ");
       Serial.print(" ");
       if(count%8 == 0){
       Serial.println();

   }
  }
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
 /*************************************************
  * 类型：函数
  * 功能：单个数据串口发送
  * void 函数名(缓冲区 , 待发数据, 整数部分长度, 小数部分长度, 传输数据长度（包括空）)
**************************************************/
void tcpsend_procceed(char test[16] , float data, int i, int j, int m)
{

        
       // mySerial.write(str,2);
       /*m传入数据长度ascll m<8*/
       
       /*dtostrf将 float/double转换为char
        * i转换后整数部分长度
        *j转换后小数部分长度
        */
       
        dtostrf(data,i, j, test);//保存到该char数组中。
        
       for( i = 0; i < m; i++)
        {
        mySerial.write(test[i]);//ASCLL 向esp8266写数据
        }
        //mySerial.write(35);//ASCLL #井号,分隔符
        test[16] = {0};//reset char
}

//下次测试若无分隔符传输情况
 /*************************************************
  * 建立：2019.4.19；
  * 修改：2019.7.12 lqh
  * 类型：函数
  * 功能：按格式数据包串口发送，数据包类型1
  * void 函数名(结构体地址)
  * 效果：控制小车运动
**************************************************/
void transmit_1(GESTURE *g)//发送数据
{

   float t_pitch;
  float t_roll;
  
  t_pitch=fabs(g->pitch);
  t_roll=fabs(g->roll);//发送无符号数据
  /*限幅*/
  if(t_pitch>=90) t_pitch=90.00;
  if(t_roll>=90) t_roll=90.00;
  
  //transmit
       mySerial.write(36);//ASCLL $号,占位符
       
       mySerial.write(64);//ASCLL @号,通讯标志符，便于上位机确定数据包类型 0     <-----可以考虑更改@号为别的标识符
       tcpsend_procceed(test , 1.0, 1, 0, 2);//设备编号
       
       mySerial.write(35);//ASCLL #井号，分隔符 1
       tcpsend_procceed(test , g->up_down, 1, 0, 2);//2 3
       mySerial.write(35);//ASCLL #井号 4
       tcpsend_procceed(test , g->left_right, 1, 0, 2);//5 6
       mySerial.write(35);//ASCLL #井号 7
       tcpsend_procceed(test , t_pitch, 3, 2, 5);//8 9 10 11 12
       mySerial.write(35);//ASCLL #井号13
       /*tcpsend_procceed(test , velocity, 2, 0, 2);
       mySerial.write(35);//ASCLL #井号*/
       tcpsend_procceed(test , t_roll, 3, 2, 5);//13 14 15 16 17 
       mySerial.write(33);//ASCLL !号，结束符
       //Serial.println("调用第一个发送函数");
}
 /*************************************************
  * 建立：2019.；
  * 修改：？；2019.7.12 lqh； 
  * 类型：函数
  * 功能：按格式数据包串口发送，数据包类型2
  * void 函数名(指令)
  * 效果：特殊效果
**************************************************/
void transmit_2(uint8_t instruction)//发送数据,第二类发送
{
  /*0：故障
   * 1：原路返回
   * 2：开灯
   * 3：。。。
   */       
   mySerial.write(36);//ASCLL $号,占位符，没有任何意义,但不能删

   mySerial.write(63);//ASCLL ?号,通讯标志符，便于上位机确定数据包类型
 
  //transmit
        //tcpsend_procceed(test , sign, 1, 0, 1);//ASCLL 应用层标志符，自定义
        mySerial.write(37);//ASCII %百分号
       mySerial.write(35);//ASCLL #井号，分隔符
        mySerial.write(43);//ASCLL +号
         mySerial.write(35);//ASCLL #井号，分隔符
       tcpsend_procceed(test , instruction, 1, 0, 1);//发送指令
       mySerial.write(33);//ASCLL !号，结束符

        //Serial.println("调用第二个发送函数");
}
 /*************************************************
  * 建立：2019.；
  * 修改：？；2019.7.12 lqh； 
  * 类型：函数
  * 功能：按格式数据包串口发送，数据包类型3
  * void 函数名()
  * 效果：机械臂
**************************************************/
void transmit_3()//发送数据    
{

   float t_pitch;
  float t_roll;
  float t_minus;//俯仰角之差
  
  t_pitch=device[0].pitch+90.00;
  t_roll=device[0].roll+90.00;
  t_minus=fabs(device[0].pitch-device[1].pitch);

  if(t_minus>=180)
  {
    t_minus=180;
  }
  if(t_minus<=10)
  {
    t_minus=10;
  }
  if(t_pitch<=10)
  {
    t_pitch=10.0;
  }
  if(t_pitch>=180)
  {
    t_pitch=180;
  }
  if(t_roll<=10)
  {
    t_roll=10.0;
  }
  if(t_roll>=180)
  {
    t_roll=180;
  }
  t_roll/=2;
  t_pitch/=2;
  t_minus/=2;
  
  //transmit
  /************发送格式*************/
  /*
   *      $%80.00#130.00#50.00!
   *    占位符,通讯标识符,翻滚角,俯仰角,俯仰角之差,结束符
   */
       mySerial.write(36);//ASCLL $号,占位符
       
       mySerial.write(37);//ASCLL %号,通讯标志符，便于上位机确定数据包类型 2     <-----可以考虑更改@号为别的标识符
       
       mySerial.write(35);//ASCLL #井号，分隔符 1
       /*if(t_roll>0&&t_roll<100)//两位数
       {
        mySerial.write(32);
       }*/
       tcpsend_procceed(test , (int)t_roll, 2, 0, 2);
       mySerial.write(35);//ASCLL #井号 
      
       tcpsend_procceed(test , (int)t_pitch, 2, 0, 2);
       mySerial.write(35);//ASCLL #井号 
  
       tcpsend_procceed(test , (int)t_minus, 2, 0, 2);
       mySerial.write(35);//ASCLL #井号13
       /*tcpsend_procceed(test , velocity, 2, 0, 2);
       mySerial.write(35);//ASCLL #井号*/
       mySerial.write(33);//ASCLL !号，结束符
       //Serial.println("调用第三个发送函数");
}
 /*************************************************
  * 建立：2019.4；
  * 修改：？；2019.7.12 lqh； 
  * 类型：函数
  * 功能：获取mpu6050数据并存入指定数组位置
  * void 函数名(位置)
  * 效果：特殊效果
**************************************************/
void getdata(int num)
{
  float sum_pitch=0.0f;//多次pitch角的和//7.12pm lqh
  float ave_pitch=0.0f;//picth角的平均值
  float sum_roll=0.0f;
  float ave_roll=0.0f;
  //int count;
  for(int count = 0;count <5;count++)
    {
      mpu.getMotion6(&ax, &ay ,&az, &gx, &gy, &gz);     //IIC获取MPU6050六轴数据 ax ay az gx gy gz

      Angle_get();             //获取angle 角度和卡曼滤波
          //求平均数，判断姿势
          //getsum(angle6,angle);
          sum_pitch+=angle6;
          sum_roll+=angle;
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
      device[num].roll = ave_roll;
      device[num].pitch = ave_pitch;

      device[num].equipment = num;
      sum_pitch=0;//数据归零
      sum_roll=0;
      //count=0;
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

    pinMode(LED1,OUTPUT);
     pinMode(SW,INPUT_PULLUP);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);

    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);//默认都关
    lastState = digitalRead(SW);//读取开关的初始值

    digitalWrite(C1,LOW);//默认从手背开始
    digitalWrite(C2,HIGH);
    initgesture(&two_gesture);//初始化姿势库
    open_status=0;//一开始关
    
    if((mpu.getI2CMasterModeEnabled()) && !mpu.getI2CBypassEnabled())
    {
     Serial.println("Set MPU6000 Master Mode success!");
    }
    else
    {
     Serial.println("mpu6050 master iic fail!");
    }
}
 /*************************************************
  * 建立：2019.；
  * 修改： 
  * 类型：函数
  * 功能：初始化设备参数
  * void 函数名()
  * 效果：
**************************************************/
void init_device()//初始化设备参数
{
  int temp_a;
  for(temp_a=0;temp_a<N;temp_a++)
  {
    device[temp_a]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0};
  }
  choose=0;
}

void loop()
{
  int old_open_status;
  old_open_status=open_status;//记录状态
           //recive
       while(mySerial.available())           //从esp8266读数据
      {
        Serial.write(mySerial.read());//回发
      }


    boolean b1 = digitalRead(SW);//读取目前的开关状态
      if(b1 != lastState)//如果与以前的开关至不同
      {
        delay(50);//去震动
       boolean  b2 = digitalRead(SW);//再读取一次开关状态
  
      if(b2 == b1)//确认两次开关状态是否一致
      {
        lastState = b1;//存储开关状态
        click1++;//次数累计
        }
      }

      if(click1 % 2==0&&click1<=8)//如果开关状态改变2次
      {
            //click1 = 0;//归零
            if(click1==2)
            {
              //按一次，第一个灯亮，手背控制
              digitalWrite(LED1,HIGH);
              digitalWrite(LED2,LOW);
              digitalWrite(LED3,LOW);
              open_status=1;
              if(open_status!=old_open_status)//第一次变化才清零所有数据
              {
                init_device();
              }
            }
            if(click1==4)
            {
              //按两次，第二个灯亮，手势控制
              digitalWrite(LED1,LOW);
              digitalWrite(LED2,HIGH);
              digitalWrite(LED3,LOW);
              open_status=2;
              if(open_status!=old_open_status)//第一次变化才清零所有数据
              {
                init_device();
              }
            }
            if(click1==6)
            {
              //按三次，第三个灯亮，机械臂
              digitalWrite(LED1,LOW);
              digitalWrite(LED2,LOW);
              digitalWrite(LED3,HIGH);
              open_status=3;
              if(open_status!=old_open_status)//第一次变化才清零所有数据
              {
                init_device();
              }
            }
            if(click1==8)
            {
              click1=0;//归零，都关了
              digitalWrite(LED1,LOW);
              digitalWrite(LED2,LOW);
              digitalWrite(LED3,LOW);
              open_status=0;
              if(open_status!=old_open_status)//第一次变化才清零所有数据
              {
                init_device();
              }
            }
       }
       //Serial.print("开关状态：");Serial.println(open_status);
    
    /*#ifdef DEBUG//测试状态开关必为闭合，以便测试
    open_status=OPEN;
    #endif // DEBUG*/

    //getdata();
      if(open_status==0)
      {
        delay(100);//什么都不做
      }
      else
      {
      //手背
      if( choose == 0)//choose 实际是0，定义choose=0 为手背
       {
         getdata(0);

        /* axt = float(ax) / 2048 ;
         ayt = float(ay) / 2048 ;
         azt = float(az) / 2048;*/


        /*Serial.print("ax: ");Serial.print(axt);Serial.print(",");
        Serial.print("ay: ");Serial.print(ayt);Serial.print(",");
        Serial.print("az: ");Serial.print(azt);Serial.print("---");*/

        Serial.print("roll: ");Serial.print(device[0].roll);Serial.print(",");
        //Serial.print("angle_dot: ");Serial.print(angle_dot);Serial.print(",");
        Serial.print("pitch: ");Serial.println(device[0].pitch);
        
         judge_gesture(&real_gesture, device);//获取手背姿势参数
         
         real_gesture.equipment = 0;
         //count=0;//进行初始化操作 7.12pm lqh
         //sum_pitch=0;
         //sum_roll=0;
         /*Serial.print(real_gesture.up_down);
         Serial.print("\t");
         Serial.print(real_gesture.left_right);
         Serial.print("\t");
         Serial.print(real_gesture.pitch);
         Serial.print("\n");*/
        if(open_status==1)
        {
         transmit_1(&real_gesture);//调用发送数据函数
        }
       }
    //手指
    else if(choose == 1)
    {
      getdata(1);

      /*axt = float(ax) / 2048 ;
      ayt = float(ay) / 2048 ;
      azt = float(az) / 2048;*/

      /*Serial.print("ax: ");Serial.print(axt);Serial.print(",");
      Serial.print("ay: ");Serial.print(ayt);Serial.print(",");
      Serial.print("az: ");Serial.print(azt);Serial.print("---");*/

      Serial.print("roll: ");Serial.print(device[1].roll);Serial.print(",");
      //Serial.print("angle_dot: ");Serial.print(angle_dot);Serial.print(",");
      Serial.print("pitch: ");Serial.println(device[1].pitch);
      //counter++;
    }
    if(open_status==2&&choose==1)//第二个条件：进行一轮循环读取之后计算
    {
        counter++;//进行一次循环读取，计数器+1
         have_twogesture(0,1,&two_gesture);//入口参数为两台设备号,规定第一个为手背
         //**********调用发送姿势函数************//
         //Serial.println("特殊手势");
         transmit_2(two_gesture.finalnum);
         //empty(&towreal_gesture);//清零姿势库
         Serial.print("twogesture:");Serial.println(two_gesture.finalnum);
    }
    if(open_status==3&&choose==1)
    {
      //Serial.println("机械臂");
      transmit_3();//调用机械臂传输函数
    }
  //可视化分析

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
  if(open_status==1)
  {
    //Serial.println("手背");
    choose=0;//如果为手背状态，则不读取其他MPU6050
  }
  
  //counter++;//进行一次循环读取，计数器+1
  Serial.print("choose");Serial.println(choose);
  delay(50);
  }
}


/*void getsum(float fNewPitch,float fNewRoll)//求一段时间内算得的pitch角的和值
{
  sum_pitch+=fNewPitch;
  sum_roll+=fNewRoll;
  count++;
}*/
 /*************************************************
  * 建立：2019.；
  * 修改： 
  * 类型：函数
  * 功能：手背姿态判断
  * void 函数名()
  * 效果：
**************************************************/
void judge_gesture(GESTURE *g, DEVICE device[])//判断姿势，返回值为 1234 ，1：向上，2：向下
{
  g->pitch=device[0].pitch;//角度值赋值
  g->roll=device[0].roll;//需要发送
  if(device[0].pitch>=LITTLE_UP)//判定角度，如果大于25°,小于45,则向中上，此处可更改
  {
    g->up_down=UP;
  }
  else if(device[0].pitch<=LITTLE_DOWN)
  {
    g->up_down=DOWN;
  }
  else
  {
    g->up_down=0;
  }
  if(device[0].roll>=LITTLE_RIGHT)//判定角度，如果x向右大于25°,小于45,则向中右，此处可更改
  {
    g->left_right=RIGHT;
  }
  else if(device[0].roll<=LITTLE_LEFT)
  {
    g->left_right=LEFT;
  }
  else
  {
    g->left_right=0;
  }
}
 /*************************************************
  * 建立：2019.；
  * 修改： 
  * 类型：函数
  * 功能：
  * void 函数名()
  * 效果：
**************************************************/
void have_twogesture(int backhand,int dev,TWOGESTURE *tg)
{
  float D_value_pitch=0.0;//定义pitch角的差值
  int flag_move;//两次连续动手信号（判定向上翻手两次的信号）
  flag_move=judge_move();//判断是否翻手
  //status int counter;//定义计数器，用于计数手指翻动次数
  //***********构造哈夫曼树进行判定*******************//
  D_value_pitch=device[backhand].pitch-device[dev].pitch;
  if(abs(device[backhand].roll)==-1)//手侧向翻动，目前不提供这种判定
  {
    Serial.print("high_gesture:");Serial.println("-1");//提示错误
  }
  else
  {
    if(device[backhand].pitch>=-15&&device[backhand].pitch<=15)//手背平放
    {
      if((device[dev].pitch>=-15&&device[dev].pitch<=15)||abs(D_value_pitch)<=7)
      {
        tg->none=1;//手平放
        tg->finalnum=0;
      }
      if(flag_move==1)
      {
        tg->two_times_down=1;
        tg->finalnum=2;
        Serial.println("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
      }
      else if(abs(D_value_pitch)>=150)
      {
        tg->fist=1;//握拳
        tg->finalnum=3;
      }
    }
    else if(abs(device[backhand].pitch)>=170&&abs(device[backhand].pitch<=200))
    {
      tg->opposite=1;//手心向上
      tg->finalnum=1;
    }
  }
}
 /*************************************************
  * 建立：2019.；
  * 修改： 
  * 类型：函数
  * 功能：初始化双传感器判断手势数据
  * void 函数名()
  * 效果：
**************************************************/
void initgesture(TWOGESTURE *tg)
{
  tg->none=1;
  tg->opposite=0;
  tg->two_times_down;
  tg->fist=0;
  tg->thumb=0;
  tg->finalnum=0;
}
int judge_move()
{
  int it,it1;//临时循环变量
  int tnum=0;
  int finalreturn=0;//定义返回的数，0为未检测到，1为检测到
  float D_arr[10];//差值数组
  if(counter%2==0)//如果counter为偶数
  {
      pre_arr[0][counter/2]=device[0].pitch;//手背的俯仰角
      pre_arr[1][counter/2]=device[1].pitch;//手指的俯仰角
      pre_arr[2][counter/2]=abs(pre_arr[0][counter/2]-pre_arr[1][counter/2]);//手背-手指（>0）
  }
  //一共11组数据（count+1组）
  if(counter/2>=10)//进行勾画函数图像
  {
    for(it=0;it<10;it++)//11个数据10个差值
    {
      D_arr[it]=pre_arr[2][it+1]-pre_arr[2][it];
    }
    for(it1=0;it1<10;it1++)
    {
      if(D_arr[it1]>=8 && D_arr[it1]<=40)
      {
        tnum++;
      }
      if(tnum>=4)
      {
        finalreturn=1;
      }
    }
    pre_arr[3][11]={0};
    D_arr[10]={0};
    counter=0;
    tnum=0;//数据清零
  }
  return finalreturn;
}

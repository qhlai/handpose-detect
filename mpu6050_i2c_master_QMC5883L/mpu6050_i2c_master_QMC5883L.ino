#include <Wire.h>


#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"


/*
 * all the reference:
 *MPU6050 I2C master模式调试记录 https://blog.csdn.net/airk000/article/details/22945573?tdsourcetag=s_pcqq_aiomsg
 *QMC5883L寄存器对比及参考设置 https://wenku.baidu.com/view/39238426b14e852459fb57e1.html
 *最详细的MPU6050寄存器说明手册-中文 https://wenku.baidu.com/view/f48294eef4335a8102d276a20029bd64793e6264.html
 *HMC5883L手册总结与经验分析 详解 https://blog.csdn.net/zsn15702422216/article/details/52223841
 */
  uint8_t bump[17]={0};
  uint8_t temphex[14];
//readByte(0x68, HMC5883L_IDA); 
 
MPU6050 mpu; //实例化一个 MPU6050 对象，对象名称为 mpu
int16_t ax, ay, az, gx, gy, gz;
 
//********************angle data*********************//
float Gyro_y; //Y轴陀螺仪数据暂存
float Gyro_x;
float Gyro_z;
float angleAx;
float angle6;
float K1 = 0.05; // 对加速度计取值的权重
float Angle; //一阶互补滤波计算出的小车最终倾斜角度
float accelz = 0;

 //*********************gy-271-QMC5883L的一些参数*********************//
#define ADDRESS 0x0D //QMC5883L的设备地址设置
int G_x,G_y,G_z;//以高斯为单位的测出的磁场强度（QMC5883L输出的原始数据）
float Mx,My,Mz;//以特斯拉为单位的转换后的磁场强度
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
void Get_QMC5883_mpu6050()
{
  
   writeByte(0x68, 0x6B, 0x0);//enable mpu6050 from sleep
      /*mpu.setI2CMasterModeEnabled(1);
   mpu.setI2CBypassEnabled(0);*/
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
   /*******原始模式*******/
      /* Wire.write(0x68);//使能MPU6050 I2C MASTER模式
     Wire.write(0x6A);
     Wire.write(0x20);
    
     Wire.write(0x68);//设置MPU6050作为master的I2C速率（400kHz）
     Wire.write(0x24);
     Wire.write(0x0D);//0000 1101
    
     Wire.write(0x68);////读 读写是由0x25寄存器的bit7控制的
     Wire.write(0x25);
     Wire.write(0x9E );//1001 1110
    
     Wire.write(0x68);////写 写是由0x25寄存器的bit7控制的
     Wire.write(0x25);
     Wire.write(0x1E );
    
     Wire.write(0x68);//设置SLV0设备要操作的寄存器,QMC5883L的工作模式寄存器 MODE：连续工作模式
     Wire.write(0x26);
     Wire.write(0x09);
    
     Wire.write(0x68);////工作模式寄存器 MODE：连续工作模式
     Wire.write(0x63);
     Wire.write(0x01);
    
     Wire.write(0x68);//我要写一个数据到QMC5883L
     Wire.write(0x27);
     Wire.write(0x01);
    
      Wire.write(0x68);//这里8是为了使能，1还是操作个数的意思，为了保险，这样肯定行，
     Wire.write(0x27);
     Wire.write(0x81);
    
       Wire.write(0x68);//设置地址，读，
     Wire.write(0x25);
     Wire.write(0x9E);
    
       Wire.write(0x68);//设置所要读取的寄存器起始位置  
     Wire.write(0x26);
     Wire.write(0x00);
    
       Wire.write(0x68);//告诉MPU6050从这个起始位置向后读几个  
     Wire.write(0x28);
     Wire.write(0x06);
    
        Wire.write(0x68);//使能 
     Wire.write(0x28);
     Wire.write(0x83);
    
        Wire.write(0x68);//关闭使能
     Wire.write(0x28);
     Wire.write(0x03);
     
      */
  Wire.beginTransmission(0x68);//读写开始
  Wire.write(0x49); //选择mpu6050中 对应X,Y，Z所在数据储存寄存器
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
  Serial.print("x: ");
   Serial.print(Mx);
   Serial.print("  y: ");
   Serial.print(My);
   Serial.print("  z: ");
   Serial.println(Mz); 
   //delay(50);
}
void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)//write reg
{
  Wire.beginTransmission(address);  // Initialize the Tx buffer
  Wire.write(subAddress);           // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
}
 uint8_t readByte(uint8_t address, uint8_t subAddress)//read reg
{
  uint8_t data; // `data` will store the register data   
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);                  // Put slave register address in Tx buffer
  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
  data = Wire.read();                      // Fill Rx buffer with result
  return data;                             // Return data read from slave register
}

 void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)//read regs
{  
  //example：readBytes(MPU6050_ADDRESS, XA_OFFSET_H, 2, &data[0]);
  Wire.beginTransmission(address);   // Initialize the Tx buffer
  Wire.write(subAddress);            // Put slave register address in Tx buffer
  Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
  uint8_t i = 0;
        Wire.requestFrom(address, count);  // Read bytes from slave register address 
  while (Wire.available()) {
        dest[i++] = Wire.read(); }         // Put read results in the Rx buffer
}

uint8_t getAddress(uint8_t addr) //read reg
{
   //if (num > 3) return 0;
    I2Cdev::readByte(0x68, addr, temphex);
    return temphex[0];
}
void Get_i2cdump(uint8_t num)//display all register data,(debug)
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
  
  
  /*readBytes(0x68, 0x0, 17, &bump[0]);
  for(int8_t count = 0; count < 17; count++){
     Serial.print("0x");
   Serial.print(bump[count],HEX);
   Serial.print(" ");
   
   Serial.print(" ");
   if(count%8 == 0){
    
   Serial.println();
   Serial.print("<");
   Serial.print(count);
   Serial.print(" >");
   }
  }*/
 /* Wire.beginTransmission(0x68);//读写开始
  Wire.write(0x00); //选择X,Y，Z所在数据储存寄存器
  Wire.endTransmission();
  Wire.requestFrom(0x68, 5);
  
  
  
delay(20);
  
  
   if(75<=Wire.available())//注意：国产QMC5883L顺序为xyz，进口顺序为xzy
   {
    
    for(int8_t count = 0; count < 5; count++){
   
   uint8_t temp = Wire.read(); 
     Serial.print("0x");
   Serial.print(temp,HEX);
   Serial.print(" ");
   
   Serial.print(" ");
   if(count%8 == 0){
    
   Serial.println();
   Serial.print("<");
    Serial.print(count);
    Serial.print(" >");
   }
  }
   }*/
     
     
}

/*void Get_GMCDATA()
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
}*/

 
 
void setup() {
  Wire.begin();                            //加入 I2C 总线序列
  Serial.begin(9600);                       //开启串口，设置波特率
  delay(1000);
  mpu.initialize();                       //初始化MPU6050
  //mpu.setI2CMasterModeEnabled(1);
  //mpu.setI2CBypassEnabled(0);
  if((mpu.getI2CMasterModeEnabled()) && !mpu.getI2CBypassEnabled())
    {
     Serial.println("Set MPU6000 Master Mode success!");
    }
    else
    {
     Serial.println("fail!");
    }

   /*Wire.beginTransmission(ADDRESS);//选择QMC5883L物理地址，向其中写初始化参数
    Wire.write(0x09); //选择控制寄存器0X09
    Wire.write(0x1D); //设置0x09寄存器为：OSR:512Hz，RNG：+/-8Gauss，ODG：200Hz，MODE：连续工作模式
    Wire.endTransmission();//结束*/
}
 
void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);     //IIC获取MPU6050六轴数据 ax ay az gx gy gz  
 
  Angletest();//获取angle 角度和卡曼滤波

  

 

  Serial.println();
  Serial.print(ax);Serial.print(",");
  Serial.print(ay);Serial.print(",");
  Serial.print(az);Serial.print("---");
  Serial.print(angle);Serial.print(",");
  Serial.print(angle_dot);Serial.print(",");
  Serial.println(angle6);
 Get_QMC5883_mpu6050();//获取三轴磁场
 
 //Get_i2cdump(127);//调试：获取mpu6050所有寄存器数据
 /*
  //writeByte(0x68, 0x25, 0x1E);
  //writeByte(0x68, 0x49, 0x1E);
  //readByte(0x68, 0x25);
  //I2Cdev::readBit(0x68, 0x25, 8, temphex);
  //Serial.print(mpu.getSlaveRegister(0), HEX);
  //Serial.print(mpu.getSlaveAddress(0),HEX);
  */

 
  
   delay(200);
  
   
   }

  
  
 

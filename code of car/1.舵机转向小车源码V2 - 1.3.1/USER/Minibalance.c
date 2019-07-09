#include "sys.h"

/*------------------------
#define Max_BUFF_Len 18
unsigned char Uart2_Buffer[Max_BUFF_Len];
unsigned int Uart2_Rx=0;
*/
u8 Flag_Way=0,Flag_Show=0,Flag_Stop=1,Flag_Next;                 //停止标志位和 显示标志位 默认停止 显示打开
int Encoder_Left,Encoder_Right;             //左右编码器的脉冲计数
int Encoder_A_EXTI,Flag_Direction;  
int turnback;//turn back 20190630
int Encoder_Temp;
float Velocity,Velocity_Set,Angle,Angle_Set;
int Motor_A,Motor_B,Servo,Target_A,Target_B;  //电机舵机控制相关           
int Voltage;                                //电池电压采样相关的变量
float Show_Data_Mb;                         //全局显示变量，用于显示需要查看的数据
u8 delay_50,delay_flag; 										//延时变量
float Velocity_KP=12,Velocity_KI=12;	       //速度控制PID参数
int PS2_LX=128,PS2_LY=128,PS2_RX=128,PS2_RY=128,PS2_KEY;     //PS2遥控相关
u16 ADV[128]={0};              
u8 Bluetooth_Velocity=30,APP_RX,Raspberry_Angle=0;                 //蓝牙遥控速度和APP接收的数据
u8 CCD_Zhongzhi,CCD_Yuzhi,PID_Send,Flash_Send;   //线性CCD FLASH相关-------------------------
int Sensor_Left,Sensor_Middle,Sensor_Right,Sensor;//电磁巡线相关
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash相关数组
int main(void)
{ 
	Stm32_Clock_Init(9);            //=====系统时钟设置
	delay_init(72);                 //=====延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);     //=====关闭JTAG接口
	LED_Init();                     //=====初始化与 LED 连接的硬件接口
	KEY_Init();                     //=====按键初始化
	OLED_Init();                    //=====OLED初始化
	Encoder_Init_TIM2();            //=====编码器接口
	Encoder_Init_TIM3();            //=====初始化编码器 
	EXTI_Init();                    //=====外部中断
	while(select())	{	}	            //=====选择运行模式 
	//Flag_Way=4;
	//Flag_Next=1;
	Adc_Init();                     //=====电池电压采样adc初始化
	Servo_PWM_Init(9999,71);   		//=====初始化PWM50HZ驱动 舵机
	delay_ms(300);                  //=====延时启动
	uart_init(72,128000);           //=====初始化串口1
  Motor_PWM_Init(7199,0);  				//=====初始化PWM 10KHZ，用于驱动电机 
	uart3_init(36,9600); 						//=====串口3初始化 蓝牙
	
	Flash_Read();	                   //=====读取PID参数
	
	/*-----------------------------------------------
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET) //中断产生 	
	{		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志
		Uart2_Buffer[Uart2_Rx] = USART_ReceiveData(USART2);     //接收串口1数据到buff缓冲区		
		Uart2_Rx++;      		 		
		if(Uart2_Buffer[Uart2_Rx-1] == 0x0a || Uart2_Rx == Max_BUFF_Len)    //如果接收到尾标识是换行符（或者等于最大接受数就清空重新接收）		
		{			
			if(Uart2_Buffer[0] == '+')                      //检测到头标识是我们需要的 			
			{				
				printf("%s\r\n",Uart2_Buffer);        //这里我做打印数据处理				
				Uart2_Rx=0;                                   			
			} 			
			else			
			{				
			Uart2_Rx=0;                                   //不是我们需要的数据或者达到最大接收数则开始重新接收			
			}		
		}	
	}
      */
	while(1)
		{     
			  
				 	if(Flash_Send==1)        //写入PID参数到Flash,由app控制该指令
					{
          	Flash_Write();	
						Flash_Send=0;	
					}	
					if(Flag_Show==0)         //使用MiniBalance APP和OLED显示屏
					{
  						APP_Show();	
							oled_show();          //===显示屏打开
					}
					else                      //使用MiniBalance上位机 上位机使用的时候需要严格的时序，故此时关闭app监控部分和OLED显示屏
					{
				      DataScope();          //开启MiniBalance上位机
					}	
				  delay_flag=1;	
					delay_50=0;
	 	    	while(delay_flag);	     //通过定时中断实现的50ms精准延时				
	}
}



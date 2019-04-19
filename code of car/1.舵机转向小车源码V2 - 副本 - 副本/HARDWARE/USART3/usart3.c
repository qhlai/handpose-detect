
#include "usart3.h"
u8 Usart3_Receive;
/**************************************************************************
函数功能：串口3初始化
入口参数：pclk2:PCLK2 时钟频率(Mhz)    bound:波特率
返回  值：无
**************************************************************************/
void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //使能PORTB口时钟  
	RCC->APB1ENR|=1<<18;  //使能串口时钟 
	GPIOB->CRH&=0XFFFF00FF; 
	GPIOB->CRH|=0X00008B00;//IO状态设置
	GPIOB->ODR|=1<<10;	  
	RCC->APB1RSTR|=1<<18;   //复位串口1
	RCC->APB1RSTR&=~(1<<18);//停止复位	   	   
	//波特率设置
 	USART3->BRR=mantissa; // 波特率设置	 
	USART3->CR1|=0X200C;  //1位停止,无校验位.
	//使能接收中断
	USART3->CR1|=1<<8;    //PE中断使能
	USART3->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(0,0,USART3_IRQn,2);//组2
}

/**************************************************************************
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
void USART3_IRQHandler(void)
{	
		if(USART3->SR&(1<<5))//接收到数据
	{	  
			static u8 Flag_PID,i,j,Receive[4];//定义Receive为一帧数据，格式为{movement1，movement2},大括号代表开始和终止
			static u8 select_movement[5][5];//定义一个表，用于搜索命令
			//static float Data;
			u8 temp_num1,temp_num2,init=65;
			static int isinit;//定义：isinit，作用判定是否赋初值，0为假1为真
			static u8 movement1,movement2;
			if(!isinit)//如果没赋初值，则就进入条件命令
			{
				for(temp_num1=0x00;temp_num1<0x05;temp_num1++)//赋初值A~Y，急停命令：Z
				{
					for(temp_num2=0x00;temp_num2<0x05;temp_num2++)
					{
							select_movement[temp_num1][temp_num2]=init++;
					}
				}
				isinit=1;//赋初值完毕
			}
  	  Usart3_Receive=USART3->DR; //接受数据
			/*APP_RX=Usart3_Receive;
			if(Usart3_Receive>=0x41&&Usart3_Receive<=0x48)  
			Flag_Direction=Usart3_Receive-0x40;
			else 	if(Usart3_Receive<10)  
			Flag_Direction=Usart3_Receive;	
			else 	if(Usart3_Receive==0X5A)  
			Flag_Direction=0;	*/
			/*if(Usart3_Receive==0x30)       
				Flag_Direction=0;
			else if(Usart3_Receive==0x31)
				Flag_Direction=1;*/
			
				//接受一帧数据
		if(Usart3_Receive==0x7B) Flag_PID=1;   //开始接收数据
		if(Usart3_Receive==0x7D) Flag_PID=2;   //停止接收数据

		 if(Flag_PID==1)  //采集数据
		 {
			Receive[i]=Usart3_Receive;
			i++;
		 }
		 if(Flag_PID==2)//分析数据
		 {
			 movement1=Receive[1]-0x30;//第二个位置存放的数据
			 movement2=Receive[2]-0x30;//第三个位置存放的数据
			 Flag_Direction=select_movement[movement1][movement2]-64;
			 Flag_PID=0;//相关标志位清零
				i=0;
				j=0;
				memset(Receive, 0, sizeof(u8)*4);//数组清零
		 }
		 /*if(Flag_PID==2)  //分析数据
		 {
					 if(Receive[3]==0x50) 	 PID_Send=1;
					 else  if(Receive[3]==0x57) 	 Flash_Send=1;
					 else  if(Receive[1]!=0x23) 
					 {								
						for(j=i;j>=4;j--)
						{
						  Data+=(Receive[j-1]-48)*pow(10,i-j);
						}
						switch(Receive[1])
						 {
							 case 0x30:  Bluetooth_Velocity=Data;break;
							 case 0x31:  Velocity_KP=Data;break;
							 case 0x32:  Velocity_KI=Data;break;
							 case 0x33:  break;
							 case 0x34:  break;
							 case 0x35:  break;
							 case 0x36:  break;
							 case 0x37:  break; //预留
							 case 0x38:  break; //预留
						 }
					 }				 
					 Flag_PID=0;//相关标志位清零
					 i=0;
					 j=0;
					 Data=0;
					 memset(Receive, 0, sizeof(u8)*50);//数组清零
		 }*/ 	 			
	}  					 
} 

/*void USART3_IRQHandler1(void)
{
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
}*/


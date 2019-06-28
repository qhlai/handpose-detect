
#include "usart3.h"
#include <time.h>
u8 Usart3_Receive;
#define LONGTH_CAR 0.1445f
#define MAX_STACKSIZE 100
typedef struct e
{
    float angle;
    float distance;
    float sum_time;
    int flag;
}ELEM;
typedef struct//STACK
{
    ELEM *pbase;
    ELEM *ptop;
    int stacksize;
}STACK;
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
			int temp_a;
			static u8 Flag_PID,i,j,Receive[12];//定义Receive为一帧数据，格式为{movement1，movement2，xxxx},大括号代表开始和终止
			static u8 select_movement[3][3];//定义一个表，用于搜索命令
			static float Data;
			u8 temp_num1,temp_num2,init=65,temp_num3=0x04;
			static int isinit;//定义：isinit，作用判定是否赋初值，0为假1为真
			static u8 movement1,movement2;
			static u8 two_movement;
			static float speed,temp_angle;
			static int count,pre_count;//定义接收到中断的次数，和上次的次数，用于判定先后
			static clock_t start,finish,re_start,re_finish;//开始时间结束时间
			static float distance,sum_time,dot_angle,re_sum_time;//定义dx，dθ
			float during,re_during;//经过时间
			static int flag;//定义临时flag_dir
			static int is_first;//定义是否第一次进入弹出堆栈部分 
			//****定义堆栈数据**********//
			static ELEM stack[MAX_STACKSIZE],temp_data;
			static STACK stack_list;
			static ELEM now_data;//定义弹出堆栈的结构体
			
			
			
			
			if(!isinit)//如果没赋初值，则就进入条件命令
			{
				for(temp_num1=0x00;temp_num1<0x03;temp_num1++)//赋初值A~I，急停命令：Z
				{
					for(temp_num2=0x00;temp_num2<0x03;temp_num2++)
					{
							select_movement[temp_num1][temp_num2]=init++;
					}
				}
				stack_list.pbase=stack;//底指针指向100个区域
				stack_list.ptop=stack_list.pbase;//顶指针一开始=底指针
				stack_list.stacksize=MAX_STACKSIZE;
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
			 /*if((movement1<0x00||movement1>0x25)||(movement1<0x00||movement1>0x25))//判断是否接收到数据
			 {
				 for(i1=0;i1<80;i1++)
						for(j1=0;j1<0x0500;j1++) k1++;
				 count++;//进行一次延时
				 if(count>=2)
				 {
					 count=0;
					 Flag_Direction=1;
					 for(i1=0;i1<10000;i1++)
						for(j1=0;j1<0x0500;j1++) k1++;
				 }
			 }*/
			 if(Receive[1]==0x3F&&Receive[2]==0x2B)//接收到的是第二类数据包{？+信号};此处判断一定要精确否则会影响数据接收
			 {
				 two_movement=Receive[3];
				 switch(two_movement)
				 {
						case 0x30://信号为字符0
						{
							Flag_Direction=1;//停止信号
							break;
						}
						case 0x31://信号为字符1
						{
							break;//手心朝上，调头
						}
						case 0x32://信号为字符2
						{
							//******弹出堆栈********//
							if(is_first==0)
							{
								if(stack_list.pbase==stack_list.ptop)//空堆栈
								{
										Flag_Direction=1;//堆栈为空，则不做任何动作
										break;
								}
								stack_list.ptop--;
								now_data.angle=stack_list.ptop->angle;//*data=*--s.ptop;
								now_data.distance=stack_list.ptop->distance;
								now_data.flag=stack_list.ptop->flag;
								now_data.sum_time=stack_list.ptop->sum_time;
								//Bluetooth_Velocity=now_data.distance/now_data.sum_time;
								switch(now_data.flag)
								{
									case 4:
									{
										Flag_Direction=7;
										Bluetooth_Velocity=now_data.distance/now_data.sum_time;
										break;
									}
									case 7:
									{
										Flag_Direction=4;
										Bluetooth_Velocity=now_data.distance/now_data.sum_time;
										break;
									}
									case 1:
									case 2:
									case 3:
									{
										Flag_Direction=1;
										break;
									}
									case 5:
									{
										Flag_Direction=9;
										Bluetooth_Velocity=now_data.distance/now_data.sum_time;
										//Raspberry_Angle=now_data.angle/now_data.sum_time;
										break;
									}
									case 9:
									{
										Flag_Direction=5;
										Bluetooth_Velocity=now_data.distance/now_data.sum_time;
										//Raspberry_Angle=now_data.angle/now_data.sum_time;
										break;
									}
									case 6:
									{
										Flag_Direction=8;
										Bluetooth_Velocity=now_data.distance/now_data.sum_time;
										//Raspberry_Angle=now_data.angle/now_data.sum_time;
										break;
									}
									case 8:
									{
										Flag_Direction=6;
										Bluetooth_Velocity=now_data.distance/now_data.sum_time;
										//Raspberry_Angle=now_data.angle/now_data.sum_time;
										break;
									}
								}
								is_first=1;
								re_start=clock();
							}
							else if(is_first==1)
							{
								
								while(1)
								{
									for(temp_a=0;temp_a<1000000;temp_a++)//2ms
									{
										;
									}
									re_finish=clock();
									re_during=((float)(re_finish-re_start))/CLK_TCK;//计算dt
									re_sum_time+=re_during;
									if(re_sum_time>=now_data.sum_time)
									{
										re_sum_time=0.0;
										is_first=0;
										break;
									}
								}
							}
							break;//动态识别，两次向下翻手，原路返回
						}
						case 0x33://信号为字符3
						{
							break;//握拳，蔽障行驶
						}
						case 0x34://信号为字符4
						{
							break;//大拇指，开灯
						}
				 }
			 }
			 else
			 {
				 /////5.25/////////5.25/////////5.25/////////5.25/////////5.25/////////5.25/////////5.25/////////5.25////
					if(count-pre_count==1)//判定是否为先后顺序
					{
						finish=clock();
						pre_count++;
					}
					if(count!=0)//不是第一次
					{
						//计算时间！！！！！
						during=((float)(finish-start))/CLK_TCK;//计算dt
						distance+=during*Velocity;//积分计算距离
						//*************角度算法目前没有*****************//
						//**********默认角度为π/5，之后不使用倾斜角度控制小车转弯角度*********//
						dot_angle+=(during*Velocity)/(LONGTH_CAR/sin(Angle));//计算角度，dθ=dx/R
						sum_time+=during;//累计时间计算
						flag=Flag_Direction;
					}
					/////5.25/////////5.25/////////5.25/////////5.25/////////5.25/////////5.25////////
					movement1=Receive[1]-0x30;//第二个位置存放的数据
					movement2=Receive[2]-0x30;//第三个位置存放的数据
					Flag_Direction=select_movement[movement1][movement2]-64;
					if(Flag_Direction<1||Flag_Direction>9)
					{
						 Flag_Direction=1;//flag错误，默认停止
					}
					/////////////5.25//////
					if(flag!=Flag_Direction&&count!=0)//Flag_Direction变化了
					{
						//****压入堆栈****//
						temp_data.angle=dot_angle;
						temp_data.distance=distance;
						temp_data.flag=flag;
						temp_data.sum_time=sum_time;
						stack_list.ptop=&temp_data;
						stack_list.ptop++;
						//****数据清零****//
						distance=0.0;
						dot_angle=0.0;
						sum_time=0.0;
					}
					//////5.25//////
					
					for(temp_num1=0x03;temp_num1<0x07;temp_num1++)//计算速度算法
					{
							speed+=(Receive[temp_num1]-0x30)*pow(10,temp_num3-temp_num1);
							temp_angle+=(Receive[temp_num1+0x04]-0x30)*pow(10,temp_num3-temp_num1);
					}
					Bluetooth_Velocity=speed/5;
					Raspberry_Angle=temp_angle/3;
			 }
			 
				Flag_PID=0;//相关标志位清零
				i=0;
				j=0;
				speed=0;
				temp_angle=0;
				memset(Receive, 0, sizeof(u8)*12);//数组清零
			 start=clock();//开始时间记录
				count++;//执行完函数，count++
		 }
		 /////5.25////
		 
		 /////5.25////
		 
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


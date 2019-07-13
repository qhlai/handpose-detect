#include "control.h"	

  /**************************************************************************
×÷Õß£ºÆ½ºâÐ¡³µÖ®¼Ò
ÎÒµÄÌÔ±¦Ð¡µê£ºhttp://shop114407458.taobao.com/
**************************************************************************/
#define T 0.156f
#define L 0.1445f
#define K 622.8f
//
u8 Flag_Target;
int Voltage_Temp,Voltage_Count,Voltage_All,sum;
int imk = 0; //20190630
/**************************************************************************
º¯Êý¹¦ÄÜ£ºÐ¡³µÔË¶¯ÊýÑ§Ä£ÐÍ
Èë¿Ú²ÎÊý£ºËÙ¶ÈºÍ×ª½Ç
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Kinematic_Analysis(float velocity,float angle)
{
		Target_A=velocity*(1+T*tan(angle)/2/L); 
		Target_B=velocity*(1-T*tan(angle)/2/L);      //ºóÂÖ²îËÙ
		Servo=SERVO_INIT+angle*K;                    //¶æ»ú×ªÏò   
}
/**************************************************************************
º¯Êý¹¦ÄÜ£ºËùÓÐµÄ¿ØÖÆ´úÂë¶¼ÔÚÕâÀïÃæ
         5ms¶¨Ê±ÖÐ¶ÏÓÉMPU6050µÄINTÒý½Å´¥·¢
         ÑÏ¸ñ±£Ö¤²ÉÑùºÍÊý¾Ý´¦ÀíµÄÊ±¼äÍ¬²½				 
**************************************************************************/
void TIM1_UP_IRQHandler(void)  
{    
	if(TIM1->SR&0X0001)//¶¨Ê±ÖÐ¶Ï
	{   
		  TIM1->SR&=~(1<<0);                                       							//===Çå³ý¶¨Ê±Æ÷ÖÐ¶Ï±êÖ¾Î»	       
		  if(delay_flag==1)
			 {
				 if(++delay_50==5)	 delay_50=0,delay_flag=0;                      //¸øÖ÷º¯ÊýÌá¹©50msµÄ¾«×¼ÑÓÊ±
			 }
						Encoder_Left=Read_Encoder(2);                                       //===¶ÁÈ¡±àÂëÆ÷µÄÖµ							 //ÎªÁË±£Ö¤M·¨²âËÙµÄÊ±¼ä»ù×¼£¬Ê×ÏÈ¶ÁÈ¡±àÂëÆ÷Êý¾Ý
						Encoder_Right=-Read_Encoder(3);                                      //===¶ÁÈ¡±àÂëÆ÷µÄÖµ
					 if(Flag_Way==2)
					 {	 
								RD_TSL();                           		 //===¶ÁÈ¡ÏßÐÔCCDÊý¾Ý 
						 	  Find_CCD_Zhongzhi();			          		 //===ÌáÈ¡ÖÐÏß 
					 }
					 if(Flag_Way==3)		
					 {
					 			Sensor_Left=Get_Adc(3);                //²É¼¯×ó±ßµç¸ÐµÄÊý¾Ý
								Sensor_Right=Get_Adc(8);               //²É¼¯ÓÒ±ßµç¸ÐµÄÊý¾Ý
								Sensor_Middle=Get_Adc(2);              //²É¼¯ÖÐ¼äµç¸ÐµÄÊý¾Ý
					  	  sum=Sensor_Left*1+Sensor_Middle*100+Sensor_Right*199;  //¹éÒ»»¯´¦Àí
								Sensor=sum/(Sensor_Left+Sensor_Middle+Sensor_Right);   //ÇóÆ«²î
					 }						 
						Get_RC();
						Kinematic_Analysis(Velocity,Angle);     															//Ð¡³µÔË¶¯Ñ§·ÖÎö   
						if(Turn_Off(Voltage)==0)                              							 //===Èç¹û²»´æÔÚÒì³£
						{
							Motor_A=Incremental_PI_A(Encoder_Left,Target_A);                   //===ËÙ¶È±Õ»·¿ØÖÆ¼ÆËãµç»úA×îÖÕPWM
							Motor_B=Incremental_PI_B(Encoder_Right,Target_B);                  //===ËÙ¶È±Õ»·¿ØÖÆ¼ÆËãµç»úB×îÖÕPWM 
							Xianfu_Pwm();                                                      //===PWMÏÞ·ù
							Set_Pwm(Motor_A,Motor_B,Servo);                                 	 //===¸³Öµ¸øPWM¼Ä´æÆ÷  
						}
						else
						Set_Pwm(0,0,SERVO_INIT);                                 						 //===¸³Öµ¸øPWM¼Ä´æÆ÷  	
						Voltage_Temp=Get_battery_volt();		                                 //=====¶ÁÈ¡µç³ØµçÑ¹		
						Voltage_Count++;                                                     //=====Æ½¾ùÖµ¼ÆÊýÆ÷
						Voltage_All+=Voltage_Temp;                                           //=====¶à´Î²ÉÑùÀÛ»ý
						if(Voltage_Count==100) Voltage=Voltage_All/100,Voltage_All=0,Voltage_Count=0;//=====ÇóÆ½¾ùÖµ		                                   
			  
			 if(Flag_Show==0)				Led_Flash(100);
			 else if(Flag_Show==1)	Led_Flash(0);  //ledÉÁË¸
			 Key();    //===É¨Ãè°´¼ü×´Ì¬ µ¥»÷Ë«»÷¿ÉÒÔ¸Ä±äÐ¡³µÔËÐÐ×´Ì¬
	}		 
 } 

/**************************************************************************
º¯Êý¹¦ÄÜ£º¸³Öµ¸øPWM¼Ä´æÆ÷
Èë¿Ú²ÎÊý£º×óÂÖPWM¡¢ÓÒÂÖPWM
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int servo)
{
	    if(Flag_Way==2||Flag_Way==3)//Ñ²ÏßÄ£Ê½ÏÂ£¬Ö»ÔÊÐíµç»úÕý×ª
			{
			  if(motor_a<0)motor_a=0;
				if(motor_b<0)motor_b=0;
			}
    	if(motor_a<0)			PWMA1=7200,PWMA2=7200+motor_a;
			else 	            PWMA2=7200,PWMA1=7200-motor_a;
		
		  if(motor_b<0)			PWMB1=7200,PWMB2=7200+motor_b;
			else 	            PWMB2=7200,PWMB1=7200-motor_b;
     SERVO=servo;	
}

/**************************************************************************
º¯Êý¹¦ÄÜ£ºÏÞÖÆPWM¸³Öµ 
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=6900;    //===PWMÂú·ùÊÇ7200 ÏÞÖÆÔÚ6900
    if(Motor_A<-Amplitude) Motor_A=-Amplitude;	
		if(Motor_A>Amplitude)  Motor_A=Amplitude;	
	  if(Motor_B<-Amplitude) Motor_B=-Amplitude;	
		if(Motor_B>Amplitude)  Motor_B=Amplitude;		
		if(Servo<(SERVO_INIT-500))     Servo=SERVO_INIT-500;	  //¶æ»úÏÞ·ù
		if(Servo>(SERVO_INIT+500))     Servo=SERVO_INIT+500;		  //¶æ»úÏÞ·ù
}
/**************************************************************************
º¯Êý¹¦ÄÜ£º°´¼üÐÞ¸ÄÐ¡³µÔËÐÐ×´Ì¬ 
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Key(void)
{	
	u8 tmp,tmp2;
	tmp=click(); 
	if(tmp==1)Flag_Stop=!Flag_Stop;//µ¥»÷¿ØÖÆÐ¡³µµÄÆôÍ£
	//if(tmp==2)Flag_Show=!Flag_Show;//Ë«»÷¿ØÖÆÐ¡³µµÄÏÔÊ¾×´Ì¬
	tmp2=Long_Press();          
  if(tmp2==1)Flag_Show=!Flag_Show;//¿ØÖÆÐ¡³µµÄÏÔÊ¾×´Ì¬
}

/**************************************************************************
º¯Êý¹¦ÄÜ£ºÒì³£¹Ø±Õµç»ú
Èë¿Ú²ÎÊý£ºµçÑ¹
·µ»Ø  Öµ£º1£ºÒì³£  0£ºÕý³£
**************************************************************************/
u8 Turn_Off( int voltage)
{
	    u8 temp;
			if(voltage<740||Flag_Stop==1)//µç³ØµçÑ¹µÍÓÚ7.4V¹Ø±Õµç»ú
			{	                                                
      temp=1;                                            
      }
			else
      temp=0;
      return temp;			
}
/**************************************************************************
º¯Êý¹¦ÄÜ£º¾ø¶ÔÖµº¯Êý
Èë¿Ú²ÎÊý£ºint
·µ»Ø  Öµ£ºunsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
/**************************************************************************
º¯Êý¹¦ÄÜ£ºÔöÁ¿PI¿ØÖÆÆ÷
Èë¿Ú²ÎÊý£º±àÂëÆ÷²âÁ¿Öµ£¬Ä¿±êËÙ¶È
·µ»Ø  Öµ£ºµç»úPWM
¸ù¾ÝÔöÁ¿Ê½ÀëÉ¢PID¹«Ê½ 
pwm+=Kp[e£¨k£©-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)´ú±í±¾´ÎÆ«²î 
e(k-1)´ú±íÉÏÒ»´ÎµÄÆ«²î  ÒÔ´ËÀàÍÆ 
pwm´ú±íÔöÁ¿Êä³ö
ÔÚÎÒÃÇµÄËÙ¶È¿ØÖÆ±Õ»·ÏµÍ³ÀïÃæ£¬Ö»Ê¹ÓÃPI¿ØÖÆ
pwm+=Kp[e£¨k£©-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Target-Encoder;                //¼ÆËãÆ«²î
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //ÔöÁ¿Ê½PI¿ØÖÆÆ÷
	 Last_bias=Bias;	                   //±£´æÉÏÒ»´ÎÆ«²î 
	 return Pwm;                         //ÔöÁ¿Êä³ö
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Target-Encoder;                //¼ÆËãÆ«²î
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //ÔöÁ¿Ê½PI¿ØÖÆÆ÷
	 Last_bias=Bias;	                   //±£´æÉÏÒ»´ÎÆ«²î 
	 return Pwm;                         //ÔöÁ¿Êä³ö
}
/**************************************************************************
º¯Êý¹¦ÄÜ£ºÒ£¿Ø
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Get_RC(void)
{
	int Yuzhi=2;
	static float Bias,Last_Bias;
  float LY,RX;
	if(Flag_Way==0)//À¶ÑÀ¿ØÖÆ
	{
		if(Flag_Direction==0) Velocity=0,Angle=0;   //Í£Ö¹
		else if(Flag_Direction==1) Velocity=Bluetooth_Velocity,Angle=0;  //Ç°½ø
		else if(Flag_Direction==2) Velocity=Bluetooth_Velocity,Angle=PI/5;  //ÓÒÇ°
		else if(Flag_Direction==3) Velocity=0,Angle=0;   //¶æ»úÏòÓÒ
		else if(Flag_Direction==4) Velocity=-Bluetooth_Velocity,Angle=PI/5;  // ÓÒºó
		else if(Flag_Direction==5) Velocity=-Bluetooth_Velocity,Angle=0;    //ºóÍË
		else if(Flag_Direction==6) Velocity=-Bluetooth_Velocity,Angle=-PI/5;  //×óºó
		else if(Flag_Direction==7) Velocity=0,Angle=0;                       //¶æ»úÏò×ó
		else if(Flag_Direction==8) Velocity=Bluetooth_Velocity,Angle=-PI/5;  //×óÇ°
	}
	else	if(Flag_Way==1)//PS2¿ØÖÆ
	{
  	LY=PS2_LY-128;     //¼ÆËãÆ«²î
		RX=PS2_RX-128;
		if(LY>-Yuzhi&&LY<Yuzhi)LY=0;   //Ð¡½Ç¶ÈÉèÎªËÀÇø ·ÀÖ¹¶¶¶¯³öÏÖÒì³£
	  if(RX>-Yuzhi&&RX<Yuzhi)RX=0;
		 Velocity=-LY/2;	  //ËÙ¶ÈºÍÒ¡¸ËµÄÁ¦¶ÈÏà¹Ø¡£
		 Angle=RX/200; 
	}
		else	if(Flag_Way==2)//CCDÑ²Ïß
	{
		 Velocity=45;	   //CCDÑ²ÏßÄ£Ê½µÄËÙ¶È
		 Bias=CCD_Zhongzhi-64;   //ÌáÈ¡Æ«²î
		 Angle=Bias*0.010+(Bias-Last_Bias)*0.085; //PD¿ØÖÆ
		 Last_Bias=Bias;   //±£´æÉÏÒ»´ÎµÄÆ«²î
	}
		else	if(Flag_Way==3)//µç´ÅÑ²Ïß
	{
		 Velocity=45;	  //µç´ÅÑ²ÏßÄ£Ê½ÏÂµÄËÙ¶È
		 Bias=100-Sensor;  //ÌáÈ¡Æ«²î
		 Angle=myabs(Bias)*Bias*0.0002+Bias*0.001+(Bias-Last_Bias)*0.05; //
		 Last_Bias=Bias;   //ÉÏÒ»´ÎµÄÆ«²î
	}
	else if(Flag_Way==4)//Ê÷Ý®ÅÉ¿ØÖÆ
	{
		switch(Flag_Direction)
		{
			case 1:Velocity=0,Angle=0;break; 			//Í£Ö
			case 2:Velocity=0,Angle=PI/5;break; //¶æ»úÇ¿you
			case 3:Velocity=0,Angle=-PI/5;break;//¶æ»úÇ¿zuo×ª
			case 4:Velocity=Bluetooth_Velocity,Angle=0;break;//Ö±ÐÐ
			case 5:Velocity=Bluetooth_Velocity,Angle=PI/5;break;//Ç°ÓÒ
			case 6:Velocity=Bluetooth_Velocity,Angle=-PI/5;break;//Ç°×ó
			case 7:Velocity=-Bluetooth_Velocity,Angle=0;break;//µ¹³µ
			case 8:Velocity=-Bluetooth_Velocity,Angle=PI/5;break;//ºóyou
			case 9:Velocity=-Bluetooth_Velocity,Angle=-PI/5;break;//ºózuo
		}
	
		
		/*if(Flag_Direction==0) Velocity=0,Angle=0;   //Í£Ö¹
		else if(Flag_Direction==1) Velocity=Bluetooth_Velocity,Angle=0;  //Ç°½ø
		else if(Flag_Direction==2) Velocity=Bluetooth_Velocity,Angle=PI/5;  //ÓÒÇ°
		else if(Flag_Direction==3) Velocity=0,Angle=0;   //¶æ»úÏòÓÒ
		else if(Flag_Direction==4) Velocity=-Bluetooth_Velocity,Angle=PI/5;  // ÓÒºó
		else if(Flag_Direction==5) Velocity=-Bluetooth_Velocity,Angle=0;    //ºóÍË
		else if(Flag_Direction==6) Velocity=-Bluetooth_Velocity,Angle=-PI/5;  //×óºó
		else if(Flag_Direction==7) Velocity=0,Angle=0;                       //¶æ»úÏò×ó
		else if(Flag_Direction==8) Velocity=Bluetooth_Velocity,Angle=-PI/5;  //×óÇ°*/
	}
		else	if(Flag_Way==5)//turn back test
	{
			//Velocity=-10;
			//Angle=45/(180/PI);
			
			imk++;
		//Velocity=-10;
				//Angle=-45/(180/PI);
			//delay_ms(500);
		/*if(imk < 100)//ÏÈÏòÓÒÂýËÙ
			{Velocity=0;Angle=0/(180/PI);}
			else if(imk >= 100&&imk< 200)
			{Velocity=10;Angle=Raspberry_Angle;}
			else if(imk >= 200&&imk< 300)
			{Velocity=-10;Angle=-Raspberry_Angle;}
			else if(imk >= 300&&imk< 400)
			{Velocity=10;Angle=Raspberry_Angle;}
			else if(imk >= 400&&imk<= 500)
			{Velocity=-10;Angle=-Raspberry_Angle;}
			else if (imk >= 500&&imk<= 600)
			{Velocity=0;Angle=0/(180/PI);}
			else if (imk >600)
			{imk = 0;Flag_Way=4;}*/
		if(imk < 100)//ÏÈÏòÓÒÂýËÙ
			{Velocity=0;Angle=0/(180/PI);}
			else if(imk >= 100&&imk< 200)
			{Velocity=10;Angle=37/(180/PI);}
			else if(imk >= 200&&imk< 300)
			{Velocity=-10;Angle=-37/(180/PI);}
			else if(imk >= 300&&imk< 400)
			{Velocity=10;Angle=37/(180/PI);}
			else if(imk >= 400&&imk<= 500)
			{Velocity=-10;Angle=-37/(180/PI);}
			else if (imk >= 500&&imk<= 600)
			{Velocity=0;Angle=0/(180/PI);}
			else if (imk >600)
			{imk = 0;Flag_Way=4;}
			/*if(imk < 200)//ÏÈÏòÓÒÂýËÙ
			{Velocity=0;Angle=0/(180/PI);}
			else if(imk >= 200&&imk< 400)
			{Velocity=5;Angle=37/(180/PI);}
			else if(imk >= 400&&imk< 600)
			{Velocity=-5;Angle=-37/(180/PI);}
			else if(imk >= 600&&imk< 800)
			{Velocity=5;Angle=37/(180/PI);}
			else if(imk >= 800&&imk<= 1000)
			{Velocity=-5;Angle=-37/(180/PI);}
			else if (imk >= 1000&&imk<= 1200)
			{Velocity=0;Angle=0/(180/PI);}
			else if (imk >1200)
			{imk = 0;Flag_Way=4;}*/
			
				
		//
	}
}
/**************************************************************************
º¯Êý¹¦ÄÜ£ºÏßÐÔCCDÈ¡ÖÐÖµ
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void  Find_CCD_Zhongzhi(void)
{ 
	 static u16 i,j,Left,Right,Last_CCD_Zhongzhi;
	 static u16 value1_max,value1_min;
	
	   value1_max=ADV[0];  //¶¯Ì¬ãÐÖµËã·¨£¬¶ÁÈ¡×î´óºÍ×îÐ¡Öµ
     for(i=5;i<123;i++)   //Á½±ß¸÷È¥µô5¸öµã
     {
        if(value1_max<=ADV[i])
        value1_max=ADV[i];
     }
	   value1_min=ADV[0];  //×îÐ¡Öµ
     for(i=5;i<123;i++) 
     {
        if(value1_min>=ADV[i])
        value1_min=ADV[i];
     }
   CCD_Yuzhi=(value1_max+value1_min)/2;	  //¼ÆËã³ö±¾´ÎÖÐÏßÌáÈ¡µÄãÐÖµ
	 for(i = 5;i<118; i++)   //Ñ°ÕÒ×ó±ßÌø±äÑØ
	{
		if(ADV[i]>CCD_Yuzhi&&ADV[i+1]>CCD_Yuzhi&&ADV[i+2]>CCD_Yuzhi&&ADV[i+3]<CCD_Yuzhi&&ADV[i+4]<CCD_Yuzhi&&ADV[i+5]<CCD_Yuzhi)
		{	
			Left=i;
			break;	
		}
	}
	 for(j = 118;j>5; j--)//Ñ°ÕÒÓÒ±ßÌø±äÑØ
  {
		if(ADV[j]<CCD_Yuzhi&&ADV[j+1]<CCD_Yuzhi&&ADV[j+2]<CCD_Yuzhi&&ADV[j+3]>CCD_Yuzhi&&ADV[j+4]>CCD_Yuzhi&&ADV[j+5]>CCD_Yuzhi)
		{	
		  Right=j;
		  break;	
		}
  }
	CCD_Zhongzhi=(Right+Left)/2;//¼ÆËãÖÐÏßÎ»ÖÃ
//	if(myabs(CCD_Zhongzhi-Last_CCD_Zhongzhi)>90)   //¼ÆËãÖÐÏßµÄÆ«²î£¬Èç¹ûÌ«´ó
//	CCD_Zhongzhi=Last_CCD_Zhongzhi;    //ÔòÈ¡ÉÏÒ»´ÎµÄÖµ
//	Last_CCD_Zhongzhi=CCD_Zhongzhi;  //±£´æÉÏÒ»´ÎµÄÆ«²î
}

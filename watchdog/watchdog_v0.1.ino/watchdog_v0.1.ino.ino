#include <avr/wdt.h>
void setup()
{
   pinMode(13,OUTPUT);
   wdt_enable(WDTO_2S); //开启看门狗，并设置溢出时间为两秒
}
 
void loop()
{
   for(int i = 0;i < 10;i++)
   {
   digitalWrite(13,HIGH);
   delay(100);
   digitalWrite(13,LOW);
   delay(100);
   }
   
   wdt_reset(); //喂狗操作，使看门狗定时器复位
}
/*
  void setup()
{
  Serial.begin(9600);
  Serial.println("ok");//程序启动时准备就绪
}

void(* resetFunc) (void) = 0; //制造重启命令

void loop()
{
  //制造一个陷阱，让程序算好自己跳进去
  for(int i = 0;i <= 100;i++)
  {
    if(i == 100) //达到条件，中计了，嘿嘿~！
    {
      Serial.println("raset");//反馈信息给串口监视器准备重启
      delay(1000);    //延迟时间
      resetFunc();      //重启程序开始
    }
    delay(20);//延时for函数计算时间
  }
}
 * /
 */

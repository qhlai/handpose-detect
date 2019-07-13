const byte LED1 = 5;
const byte LED2 = 7;
const byte LED3 = 8;
const byte SW = 6;
boolean lastState = LOW;//记录上次的开关状态，初始化为LOW
//boolean toggle = LOW;//输出给LED的信号，初始化为LOW
int click1 = 0;//开关信号的改变次数，初始化为0

void setup() 
{
  pinMode(LED1,OUTPUT);
  pinMode(SW,INPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);//默认都关
  lastState = digitalRead(SW);//读取开关的初始值
}

void loop() 
{
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
          //按一次，第一个灯亮
          digitalWrite(LED1,HIGH);
          digitalWrite(LED2,LOW);
          digitalWrite(LED3,LOW);
        }
        if(click1==4)
        {
          //按两次，第二个灯亮
          digitalWrite(LED1,LOW);
          digitalWrite(LED2,HIGH);
          digitalWrite(LED3,LOW);
        }
        if(click1==6)
        {
          //按三次，第三个灯亮
          digitalWrite(LED1,LOW);
          digitalWrite(LED2,LOW);
          digitalWrite(LED3,HIGH);
        }
        if(click1==8)
        {
          click1=0;//归零，都关了
          digitalWrite(LED1,LOW);
          digitalWrite(LED2,LOW);
          digitalWrite(LED3,LOW);
        }
   }
}

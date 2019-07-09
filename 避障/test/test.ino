    /*通信*/
 #include <Wire.h>
#define SLAVE_ADDRESS 0x04
int data;
//int state = 0;
    /*
    IDE 版本:1.0.1
    功能：利用SR04超声波传感器进行测距，并用串口显示测出的距离值
    */
    //slam
//0：前方；1：左；2：右；3：后
    // 设定SR04连接的Arduino引脚
    const int TrigPin[6] = {A0,A2,A4,2,4,6};
    const int EchoPin[6] = {A1,A3,A5,3,5,7};
    
    float distance[5];
    float velocity[5];
    // callback for received data
    void receiveData(int byteCount)
    {
    while(Wire.available())
      {
      data = Wire.read();
      Serial.print("data received: ");
      Serial.println(data);
      }
    }
    // callback for sending data
     void sendData()
     {
     Wire.write(data);
     }

    void setup()
    {   
      Serial.begin(9600); // start serial for output 
      // initialize i2c as slave 
      Wire.begin(SLAVE_ADDRESS); 
      // define callbacks for i2c communication 
      Wire.onReceive(receiveData);
      //从机 接收 主机 发来的数据 
      Wire.onRequest(sendData);
      //从机 请求 主机 发送数据 
      Serial.println("Ready");

      // 初始化串口通信及连接SR04的引脚
            Serial.begin(9600);
            pinMode(TrigPin[0], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[0], INPUT);
             pinMode(TrigPin[1], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[1], INPUT);
             pinMode(TrigPin[2], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[2], INPUT);
             pinMode(TrigPin[3], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[3], INPUT);
             pinMode(TrigPin[4], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[4], INPUT);
             pinMode(TrigPin[5], OUTPUT);
        // 要检测引脚上输入的脉冲宽度，需要先设置为输入状态
            pinMode(EchoPin[5], INPUT);

        Serial.println("Ultrasonic sensor:");
    }
    void Get_Distance(int i, float *distance)
    {
            float tempdistance;
            // 产生一个10us的高脉冲去触发TrigPin
            digitalWrite(TrigPin[i], LOW);
            delayMicroseconds(2);
            digitalWrite(TrigPin[i], HIGH);
            delayMicroseconds(10);
            digitalWrite(TrigPin[i], LOW);
            //检测脉冲宽度，并计算出距离
            tempdistance = pulseIn(EchoPin[i], HIGH) / 58.00;
            
            if(tempdistance>400)
            {
              distance[i]= 400;
             }
             else if(tempdistance<4)
             {
                distance[i]= 4;
             }
            else
            {distance[i]= tempdistance;}
            //return tempdistance;
      }
      void Display()
      {
        int count = 0;
      for(int i = 0; i< 6; i++)
      {
       Get_Distance(i,  distance);
      Serial.print("distance[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.print(distance[i]);
      Serial.print("cm ");
      count++;
      if(count == 3)
      {
      Serial.println();
      
      }
      else if(count == 6)
      {
        Serial.println();
        Serial.println();
        count=0;
        }
       }
        }
    void loop()
    {
      int diatance_data;
     Display();
    /* if(distance[0] <30)
     {
      
      }
       if(distance[1] <30)
     {
      
      }
        if(distance[2] <30)
     {
      
      }
        if(distance[3] <30)
     {
      
      }*/
    }

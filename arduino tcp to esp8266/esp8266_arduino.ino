#include <SoftwareSerial.h>

String   char1 = "data";
String   char2 = "end";

SoftwareSerial mySerial(10, 11); // RX, TX

char c;

float posedata[8][6] = {0};

void setup() { 
  Serial.begin(9600);    
  mySerial.begin(9600); 
}

void assume(float posedata)
{
  int i;
  for(i = 0; i < 5; i++)
  {
  
  }
  
}

void loop() { 
  //从esp8266读数据
  while(mySerial.available()){
    Serial.write(mySerial.read());
  }
  //向esp8266写数据
  /*while(Serial.available()) {
    mySerial.write("123",3);
  } */
c= 1;
  mySerial.write(65);
  delay(500);
}  

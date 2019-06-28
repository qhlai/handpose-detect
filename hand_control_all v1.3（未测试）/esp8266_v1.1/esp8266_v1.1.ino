#include <ESP8266WiFi.h>

const char *ssid = "nova";
const char *password = "gogo0405";

 
const char *host = "192.168.43.234";
const int port = 4302;

WiFiClient client;
int counter = 0;
void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} 

void setup ( void ) { 
  Serial.begin(9600);
  setupWifi();
} 
int loopCount=0; 
void loop ( void ) {

/*
 ssid = "P20";
 password = "299792458";
 host = "192.168.43.94";
 port = 4302
 */;

  
  //断线重连
  while (!client.connected()){
    Serial.println("not connected");
    if (!client.connect(host, port)){
      Serial.println("connect server ....");
      delay(500);
    }
  } 
  if(counter > 0)   //显示成功信息，仅一次
  {
  Serial.println("success connect");
  counter++;
  }
  //透传
  while (client.available()){
    Serial.write(client.read());
  } 
  while (Serial.available() > 0) {
    client.write(Serial.read());
  } 
  delay(50);
  /* 
  //发送一次心跳包
  delay(250);
  loopCount++;
  if(loopCount == 20){
    client.println("ping");
    loopCount=0;
  }*/
}

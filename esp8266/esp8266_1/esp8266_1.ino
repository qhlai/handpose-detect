#include <ESP8266WiFi.h>

const char *ssid = "HIT";
const char *password = "299792458";

WiFiClient client; 
const char *host = "192.168.3.11";
const int port = 4302;

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
  //断线重连
  while (!client.connected()){
    Serial.println("not connected");
    if (!client.connect(host, port)){
      Serial.println("connect server ....");
      delay(500);
    }
  } 
  //透传
  while (client.available()){
    Serial.write(client.read());
  } 
  while (Serial.available() > 0) {
    client.write(Serial.read());
  }  
  //10s发送一次心跳包
  delay(250);
  loopCount++;
  if(loopCount == 20){
    client.println("ping");
    loopCount=0;
  }
}

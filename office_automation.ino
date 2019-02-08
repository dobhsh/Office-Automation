//BT
#include <SoftwareSerial.h>
char data=0;
int LED=D0;
SoftwareSerial s(D4,D6);//(receiver,transmitter)
int count2 = 0;

//ThingSpeak
#include "dht.h"
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
const char* ssid ="Muskan";
const char* password = "muskanjindal";
unsigned long myChannelNumber= 534386;
const char* myWriteAPIKey="LRX8LKNC5BD1QRPV";
WiFiClient WiFiclient;
dht DHT;

//Brightness Vary
int ldr=A0;
int val;

//Fire
int fire = D7;
int buzzer = D5;
int count = 0;

//Gate Close
int irsensor=D3;
int flag = 1;

//OLED Display
#include<Wire.h>
#include<Adafruit_SSD1306.h>
#include<Adafruit_GFX.h>
#define OLED_ADDR 0x3C  //OLED disply TWI Address
Adafruit_SSD1306 display(-1);
int n;
String stateprev = "OFF";
String statecurr = "OFF";

void setup() {
  //Fire
  Serial.begin(9600);
  Serial.println("start");
  
  pinMode(buzzer, OUTPUT);
  pinMode(fire,INPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(buzzer,LOW);
  pinMode(ldr,INPUT);
  digitalWrite(LED,0);
  pinMode(irsensor,INPUT);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(WiFiclient);
  
  //BT
  s.begin(9600);
  BT();

  //OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  OLED();
  
  //TS
  TS();
  
  //Brightness Vary
  
  
  //Gate Close
  
}
void BT(){
  while(s.available() <= 0 && count2 == 0){
    Fire();
    delay(1000);
  }
  count2 = 1;
   if(s.available() > 0){ //no. of bytes received
    data=s.read();//reads one byte
    Serial.print(data);
    if(data=='0'){
      digitalWrite(LED,LOW);
    }
    else{
      if(data=='1'){
        digitalWrite(LED,HIGH); 
      }
    }
  }
}

void OLED(){
  if(flag==1){
    display.setTextSize(2);
    display.setTextColor(WHITE);
    n=0;
    display.setCursor(0,n);
    display.print("Room1: ");
    display.display();
    Serial.print("Room1: ");
    if(!digitalRead(LED)){
      display.println("OFF");
      display.display();
      Serial.print("OFF");
      statecurr = "OFF";
    }
    else{
      display.println("ON");
      display.display(); 
      Serial.print("ON");
      statecurr = "ON";
    }
  }
  if((stateprev=="OFF"&&statecurr=="ON")||(stateprev=="ON"&&statecurr=="OFF")){
    display.clearDisplay();
    stateprev = statecurr;
  }
}

void TS(){
  String eno = "1";
  ThingSpeak.setField(2, eno);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void TS2(){
  String fno = "1";
  ThingSpeak.setField(1, fno);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void Brightness(){
  val=0;
  val=analogRead(ldr);//analog read range:0-1023
  Serial.println(val);
  if(val<=1024 && val>700){
    analogWrite(LED,50);//analog write range:0-255
    Serial.println("Low");
  }
  if(val<=700 && val>300){
    analogWrite(LED,130);
    Serial.println("Mid");
  }
  if(val<=300 && val>0){
    Serial.println("High");
    analogWrite(LED,255);
  }
}

void Fire(){
  if(digitalRead(fire)==0){
    Serial.println("Fire Detected");
    digitalWrite(buzzer, HIGH);
    delay(2000);
    if(count==0){
      TS2();
      count = 1;
    }
  }
   Serial.println("---");
  digitalWrite(buzzer, LOW);
  
}

void Gate(){
  if(digitalRead(irsensor)){
    digitalWrite(LED,LOW);
    flag = 0;
 }
}

void loop() {
  if(flag==1){
    BT();
    OLED();
    //Brightness();
    Fire();
    Gate();
  }
  Fire();
  OLED();
}

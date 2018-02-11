#include <LM35.h>


// Code to use SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin

LM35 temp(A0);
const  int irPin1 = A0;
const int irPin2 = A1;
const int ldrPin = A2;
int ledPin = 9;

String apiKey = "";     // replace with your channel's thingspeak WRITE API key
String ssid="";    // Wifi network SSID
String password ="";  // Wifi network password

boolean DEBUG=true;

//======================================================================== showResponce
void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========================================================================
boolean thingSpeakWrite(float value1){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  //making tcp connections
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(value1);
  
  // getStr +="&field3=";
  // getStr += String(value3);
  // ...
  getStr += "\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);   //request to the server
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}
//================================================================================ setup
void setup() {                
  DEBUG=true;           // enable debug serial
  Serial.begin(9600); 
  
   pinMode(ledPin, OUTPUT);
   pinMode(pirPin, INPUT);
   pinMode(ldrPin, INPUT);       
  
  espSerial.begin(9600);  // enable software serial
                          // Your esp8266 module's speed is probably at 115200. 
                          // For this reason the first time set the speed to 115200 or to your esp8266 configured speed 
                          // and upload. Then change to 9600 and upload again
  
  espSerial.println("AT+RST");         // Enable this line to reset the module;
  showResponse(1000);

  espSerial.println("AT+UART_CUR=9600,8,1,0,0");    // Enable this line to set esp8266 serial speed to 9600 bps
  //showResponse(1000);
  
  

  espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");
}


// ====================================================================== loop
void loop() {

  // Read sensor values
   int irStatus1 = digitalRead(irPin1);
   int ldrStatus = analogRead(ldrPin);
   int irStatus2 = digitalRead(irPin2);
   
   float mv = val* (5000/1024); 
   float celcious =( 500 - mv)/10; 
   float q = celcious;
   float t = temp.cel();
        if (isnan(t) ) {
        if (DEBUG) Serial.println("Failed to read from LM35");
      }
      else {
          if (DEBUG)  Serial.println("Temp="+String(t)+" *C");
          
           thingSpeakWrite(t);                                      // Write values to thingspeak
      }
  
  if (irStatus1 == LOW)
  {
    Serial.println("Motion detected. Turning on lights");
    digitalWrite(LED, HIGH);
  }
  else
  {
    Serial.println("clear");
  }
 }

if (irStatus2 == LOW)
  {
    Serial.println("Motion detected. Turning off lights");
    digitalWrite(LED, LOW);
  }
  else
  {
    Serial.println("clear");
  }
 }

  
  if(ldrStatus >=300) {
  digitalWrite(ledPin, LOW);
  Serial.println("LDR is DARK, LED is ON");
  }

  else {
  digitalWrite(ledPin, HIGH);
  Serial.println("---------------");
  }
    
  // thingspeak needs 15 sec delay between updates,     
  delay(6000);  
}



 

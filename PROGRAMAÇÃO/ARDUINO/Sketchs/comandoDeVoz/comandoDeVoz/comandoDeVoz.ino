#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
//VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.
SoftwareSerial porta(2, 3); // RX, TX


void setup() {
  
  // put your setup code here, to run once:
pinMode(13,OUTPUT);


delay(200);
porta.begin(9600);
Serial.begin(9600);
//myVR.write(0xAA);
//myVR.write(0x30);
//myVR.write(0x01); 
}

void loop() {
  porta.write(0xAA02000A);
  delay(200);
  porta.listen();
  String x = String(millis()/1000);
  Serial.println(porta.read() + "\t" +  x);
//  myVR.println(millis());
//  myVR.println(myVR.checkSystemSettings(0));
//  // put your main code here, to run repeatedly:
//if(myVR.available()){
//  myVR.println(myVR.read());
//  }
//  else{
//    myVR.println("Nenhum comando!");
//    myVR.write(0xAA02000A);
//    }
}

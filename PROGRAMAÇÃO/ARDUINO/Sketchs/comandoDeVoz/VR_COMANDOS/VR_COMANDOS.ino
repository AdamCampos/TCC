//Carrega as bibliotecas de classe
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

//Usa o método myVR (RX,TX) da classe VR carregada da biblioteca VoiceRecognition
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

//Declara variáveis locais. Inicia os arrays
uint8_t records[7];
uint8_t buf[64];
int led = 13;

#define comandoVozAbrir    (0)
#define comandoVozFechar   (1) 

void mostraAssinatura(uint8_t *buf, int comprimento)
{
  int i;
  for(i=0; i<comprimento; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}


/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/

void menuVR(uint8_t *buf)
{
  Serial.println("VR Indice\tGrupo\tNumeroGravacao\tAssinatura");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    mostraAssinatura(buf+4, buf[3]);
  }
  else{
    Serial.print("Nada");
  }
  Serial.println("\r\n");
}

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  
  pinMode(led, OUTPUT);
    
  if(myVR.clear() == 0){
    Serial.println("Recohecedor apagado.");
  }else{
    Serial.println("Módulo nao encontrado.");
    Serial.println("Resolver problemas de conexao com Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)comandoVozAbrir) >= 0){
    Serial.println("comandoVozAbrir carregado");
  }
  
  if(myVR.load((uint8_t)comandoVozFechar) >= 0){
    Serial.println("comandoVozFechar carregado");
  }
}

void loop()
{
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case comandoVozAbrir:
        digitalWrite(led, HIGH);
        break;
      case comandoVozFechar:
        digitalWrite(led, LOW);
        break;
      default:
        Serial.println("Comando nao reconhecido");
        break;
    }
    /** voice recognized */
    menuVR(buf);
  }
}




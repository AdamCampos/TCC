//Porta Serial extra
#include <AltSoftSerial.h>
AltSoftSerial serialExtra; //TX=9 RX=8

static int valorResposta;
static int codigoResposta;

void setup() {
Serial.begin(9600);
serialExtra.begin(9600);
pinMode(2,OUTPUT); //Led botao 2
pinMode(3,OUTPUT); //Led botao 1
pinMode(4,INPUT);  //Botao 2
pinMode(5,INPUT);  //Botao 1

digitalWrite(2,LOW);
digitalWrite(3,LOW);

}

void loop() {

//Escritas no NANO
if(digitalRead(5)){
Serial.println("Uno envia codigo 3 - descida - fila");
    //Acende o led do botao 1
    digitalWrite(3,HIGH);
    
    byte protocolo[]={255,3}; //Enviando 2 bytes ao NANO (pedido de descida- codigo 3)
    if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
    Serial.println("Uno envia codigo 3 - descida");
    serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte eh o codigo da resposra.
                                    //O segundo byte eh o valor da resposta                              
    }
}

else if(digitalRead(4)){
Serial.println("Uno envia codigo 2 - subida - fila");
    //Acende o led do botao 2
    digitalWrite(2,HIGH);
    
    byte protocolo[]={255,2}; //Enviando 2 bytes ao NANO (pedido de descida- codigo 3)
    if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
    Serial.println("Uno envia codigo 2 - subida");
    serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte eh o codigo da resposra.
                                    //O segundo byte eh o valor da resposta                              
    }
}

delay(1000);
/*-----------------------------------------------------------------------------------*/
//Leituras do NANO
  //Le a serialExtra e verifica quantos bytes estao sendo enviados pelo Nano
  if(serialExtra.available()==2){ //O byte 2 contem os valores das respostas
    codigoResposta = serialExtra.read(),DEC;
    Serial.println("Codigo lido do Nano: " + String(codigoResposta));
    
        if(serialExtra.available()==1){ //O byte 1 contem o codigo da resposta
        valorResposta = serialExtra.read(),DEC;
        Serial.println("Resposta lida do Nano: " + String(valorResposta));
      }
  }

//Tratamento das respostas dadas pelo Nano

if((codigoResposta==3) && (valorResposta==1)){
  Serial.println("Nano respondeu que terminou de descer a cabine");
  digitalWrite(3,LOW);
  }
else if((codigoResposta==2) && (valorResposta==1)){
  Serial.println("Nano respondeu que terminou de subir a cabine");
  digitalWrite(2,LOW);
  }  

}



/*
 
 
  |____________________________________________________________________________________________________.
  |                                           DO UNO PARA O NANO                                       | 
  |----------------------------------------------------------------------------------------------------|
  |   CODIGO (BYTE 1)  | VALOR (BYTE 2) |                       INSTRUCAO                              |
  |----------------------------------------------------------------------------------------------------|
  |         255        |       2        | Solcita subida da cabine e abertura das portasb              |
  |----------------------------------------------------------------------------------------------------|
  |         255        |       3        | Solcita descida da cabine e abertura das portas              |
  |----------------------------------------------------------------------------------------------------|

  

  |____________________________________________________________________________________________________.
  |                                           DO NANO PARA O UNO                                       | 
  |----------------------------------------------------------------------------------------------------|
  |   CODIGO (BYTE 1)  | VALOR (BYTE 2)  |                        INSTRUCAO                            |
  |----------------------------------------------------------------------------------------------------|
  |          1         | (int) distancia | Devolve o valor da distancia da cabine                      |
  |----------------------------------------------------------------------------------------------------|
  |          2         |        1        | Termino de subida da cabine                                 |
  |----------------------------------------------------------------------------------------------------|
  |          3         |        1        | Termino de descida da cabine                                |
  |----------------------------------------------------------------------------------------------------|


*/


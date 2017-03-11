/*O Arduino Nano Ã© o gerenciador dos motores
 * Ele recebe atravÃ©s de comunicaÃ§Ã£o serial, pedidos de abertura
 * ou fechamento das portas. alÃ©m de solicitaÃ§Ãµes de iÃ§amento e abaixamento da cabine.
 * O processamento dele Ã© simples:
 * 1) deve analisar qual foi o pedido, se Ã© de abertura ou fechamento;
 * 2) deve analisar onde estÃ¡ posicionada a cabine;
 * 3) deve analisar os estados de cada um dos quatro sensores de fim-de-curso das portas;
 * 5) se possÃ­vel deve iniciar o pedido e observÃ¡-lo atÃ© o final
 * 6) deve retornar o resultado Ã  quem fez o pedido
*/

//Porta Serial extra
#include <AltSoftSerial.h>
AltSoftSerial serialExtra; //TX=9 RX=8

//Ultrassonico
#include <Ultrasonic.h>
#define TRIGGER_PIN  6 // (3 no flat)
#define ECHO_PIN     3 // (2 no flat)
Ultrasonic sensorDistancia(TRIGGER_PIN, ECHO_PIN);
static long microsec = sensorDistancia.timing();
static float distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);

//Demais varaveis
static long tempoInicial = millis();
static long tempoFinal = millis();
boolean comandoAberturaEsquerda = false;
boolean comandoFechamentoEsquerda = false;
boolean comandoAberturaDireita = false;
boolean comandoFechamentoDireita = false;
static boolean pdf = false;
static boolean pda = false;
static boolean pea = false;
static boolean pef = false;
static boolean comandoSobe = false;
static boolean comandoDesce = false;
static boolean permissividade = false;
static boolean fechadas = false;
volatile int codigoPergunta;
volatile int valorPergunta;
//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                      Metodo setup                                         //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void setup() {

Serial.begin(9600);
serialExtra.begin(9600);   //Parametriza porta serial extra

//Sensores de fim de curso
pinMode(21,INPUT); //Sensor de porta da esquerda aberta (NF) (A02)
pinMode(22,INPUT); //Sensor de porta da esquerda fechada (NA) (A03)
pinMode(23,INPUT); //Sensor de porta da direita fechada (NA) (A04)
pinMode(24,INPUT); //Sensor de porta da direita aberta (NF) (A05)

//Os pinos A6 e A7 nÃ£o funcionaram como digitais.

//Motores
pinMode(10,OUTPUT); //Saida A do motor da direita
pinMode(11,OUTPUT); //Saida B do motor da direita
pinMode(4,OUTPUT); //Saida A do motor da esquerda
pinMode(5,OUTPUT); //Saida B do motor da esquerda
//Os pinos 2 e 7 nÃ£o funcionam.

//SaÃ­da para o circuito de iÃ§amento
pinMode(A0,OUTPUT); //Permissividade para a ponte (19)
pinMode(13,OUTPUT); //Saida A - Sentido de subida (25)
pinMode(12,OUTPUT); //Saida A' - Sentido de descida (26)

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                  Metodo fechamento                                        //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void fechamento(){

//FECHAMENTO
//Fecha ambas portas caso ambas estejam abertas e calcula o tempo

    tempoInicial = millis();

    while(!pdf || !pef) //Realiza o loop enquanto uma das portas ainda nÃ£o estiver totalmente fechada
    {
     
      pea = digitalRead(A2);
      pef = !digitalRead(A3);
      pdf = !digitalRead(A4);
      pda = digitalRead(A5);
    
      if(!pdf) //Testa se a porta da direita se fechou completamente
               //Observar que a porta quando fechada pdf vai de 1 para 0.
               //Caso nÃ£o esteja fechada ainda, mantÃ©m comando de fechamento.
     {            
                digitalWrite(10,LOW);
                digitalWrite(11,HIGH);
     }
     else
     { 
                digitalWrite(10,LOW);
                digitalWrite(11,LOW);
     }
      if(!pef) //Testa se a porta da esquerda se fechou completamente
               //Observar que a porta quando fechada pef vai de 1 para 0.
               //Caso nÃ£o esteja fechada ainda, mantÃ©m comando de fechamento.
     {         
                digitalWrite(4,LOW);
                digitalWrite(5,HIGH);
     }
     else
     {
                digitalWrite(4,LOW);
                digitalWrite(5,LOW);
     }        
    }
    tempoFinal = millis();
    Serial.println("Fechadas em: " + String((tempoFinal-tempoInicial)/1000) + " seg");

    //Garante a parada dos motores
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                   Metodo abertura                                         //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void abertura(){

//ABERTURA
//Abre ambas portas caso ambas estejam fechada e calcula o tempo

  tempoInicial = millis();
    while(!pda || !pea) //Realiza o loop enquanto uma das portas ainda nÃ£o estiver totalmente aberta
    
    {
      pea = digitalRead(A2);
      pef = !digitalRead(A3);
      pdf = !digitalRead(A4);
      pda = digitalRead(A5);
      
      if(!pda)  //Testa se a porta da direita se abriu completamente
               //Observar que a porta quando aberta pda vai de 0 para 1.
               //Caso nÃ£o esteja fechada ainda, mantÃ©m comando de abertura.
      {         
                digitalWrite(10,HIGH);
                digitalWrite(11,LOW);
      }
     else
     {
                digitalWrite(10,LOW);
                digitalWrite(11,LOW);
     }
      if(!pea) //Testa se a porta da esquerda se abriu completamente
               //Observar que a porta quando aberta pea vai de 0 para 1.
               //Caso nÃ£o esteja fechada ainda, mantÃ©m comando de abertura.
      {         
                digitalWrite(4,HIGH);
                digitalWrite(5,LOW);
      }
     else
     {
                digitalWrite(4,LOW);
                digitalWrite(5,LOW);
     }
             
    }
    tempoFinal = millis();
    Serial.println("Abertas em: " + String((tempoFinal-tempoInicial)/1000)+ " seg");
  
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
    digitalWrite(10,LOW);
    digitalWrite(11,LOW);
 
  }

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                  Metodo loopPortas                                        //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//
void loopPortas(){

delay(3000);
 
      pea = digitalRead(A2);
      pef = !digitalRead(A3);
      pdf = !digitalRead(A4);
      pda = digitalRead(A5);


boolean testeInicial = false;
if (pda==0 && pdf==0 && pea==0 && pef==0)
{testeInicial=true;
Serial.println("Condicao 0");}
if(pdf && pef || testeInicial) //se ambas as portas estiverem totalmente fechadas, entra no loop
{ abertura();}
  
if(pda && pea || testeInicial) //se ambas as portas estiverem totalmente abertas, entra no loop
{fechamento();}

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                     Metodo subir                                          //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void subir(){

    if(!fechadas){
    fechamento();
    fechadas=true;}

    //Entra no loop enquanto nÃ£o atingir a altura setada

    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);
    
    while(distancia>=30){
    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);
    Serial.println(String(distancia));
      
    digitalWrite(A0,LOW);
    digitalWrite(13,HIGH);
    digitalWrite(12,LOW);}

     
    //Bloqueia a ponte
    digitalWrite(A0,HIGH);
 
  }

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                    Metodo descer                                          //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//
  
void descer(){

    if(!fechadas){
    fechamento();
    fechadas=true;}

    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);

    //Entra no loop enquanto nÃ£o atingir a altura setada
    while(distancia<=80){
    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);

    //Serial.println(String(distancia));

    digitalWrite(A0,LOW);
    digitalWrite(13,LOW);
    digitalWrite(12,HIGH);}

    //Fim do loop
    //Coloca o permissivo em 1, bloqueando a ponte
    digitalWrite(A0,HIGH);

    Serial.println("Termino de descida");
    
  }

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                 Metodo loopMotores                                        //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void loopMotores(){

//MÃ©todo apenas para debug e comissionamento

/*-------------------------------------------------------------------------------------------------------------*/  
  //CondiÃ§Ãµes iniciais (deverÃ¡ ser usado para Warm-up)
  
  //Coloca o permissivo em 1, bloqueando a ponte
  digitalWrite(A0,HIGH);
  
  Serial.println("Distancia inicial: " + String(distancia));
  
    for(int i=6; i<1;i--){
    Serial.print(String(i) + ".");
    delay(1000);}
    Serial.println("");

/*-------------------------------------------------------------------------------------------------------------*/

  //Sobe
  tempoInicial = millis()/100;
  subir();
  tempoFinal = millis()/100;
  digitalWrite(A0,HIGH); 
  Serial.println("Subida em " + String(tempoFinal-tempoInicial));

  //Loop das portas
  loopPortas();

  //Desce
  tempoInicial = millis()/100;  
  descer();  
  tempoFinal = millis()/100;
  Serial.println("Descida em " + String(tempoFinal-tempoInicial));

  //Loop das portas
  loopPortas();

} 
  

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                   Metodo loop                                             //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void loop() {

//Mantem a ponte bloqueada e garante A diferente de A'
digitalWrite(A0,HIGH);
digitalWrite(12,LOW);
digitalWrite(13,!digitalRead(12));

//Serial.println("Incio da rotina NANO "+ String(millis()/1000));

    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);
    Serial.println(String(distancia));
    
/*-------------------------------------------------------------------------------------------------------------*/
//Inicia testes dos motores. Faz as portas se fecharem, sobe a cabine, abre as portas, fecha as portas
// desce a cabine, abre novamente as portas e torna a fechÃ¡-las em um loop infinito. Escrito para comissionamento

///loopMotores();

//Escritas no UNO
//    distancia = sensorDistancia.convert(sensorDistancia.timing(), Ultrasonic::CM);
//    byte protocolo[]={1,distancia}; //Armazena o valor da distÃ¢ncia da cabine ao sensor
//    if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
//    //Serial.println("Enviando: " + String(distancia));
//    serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte Ã© o cÃ³dgo da resposra.
//                                    //O segundo byte Ã© o valor da resposta
//
//          while(serialExtra.available()>0){
//            int t = serialExtra.read();
//            Serial.println("Consumindo buffer");
//            }
//                                    
//    }else{
//      //Serial.println("Buffer cheio");
//      }

/*-------------------------------------------------------------------------------------------------------------*/

//Leituras do UNO
  if(serialExtra.available()==2){ //O byte 2 contem os codigo das perguntas
    codigoPergunta = serialExtra.read();

        if(serialExtra.available()==1){ //O byte 1 contem o valor da pergunta
        valorPergunta = serialExtra.read();

      }
  }

if(serialExtra.available()==0){  
   if(codigoPergunta==255){
   Serial.println("UNO perguntou: " + String(codigoPergunta) + "|" + String(valorPergunta));
    
    //DESCIDA
    if(valorPergunta==3){
      //Solicita que a cabine desça ao primeiro piso
      Serial.println("Enviando fim de descida - fila");
      descer();
    while(serialExtra.available()){
    int t = serialExtra.read();
    }
      byte protocolo[]={3,1}; //Envia o código de resposta à pergunta 3 com resposta 1
          if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
          Serial.println("Enviando fim de descida");
          serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte eh o codigo da resposra.
                                          //O segundo byte e o valor da resposta
          codigoPergunta = 0;
          valorPergunta = 0;
          fechadas = false;
      delay(1000);                                    
      abertura();
      }
   }

        
    

    //SUBIDA
    else if(valorPergunta==2){
      //Solicita que a cabine suba ao segundo piso
      Serial.println("Enviando fim de subida - fila");
      subir();
    while(serialExtra.available()){
    int t = serialExtra.read();
    }
      byte protocolo[]={2,1}; //Envia o código de resposta à pergunta 3 com resposta 2
          if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
          Serial.println("Enviando fim de subida");
          serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte eh o codigo da resposra.
                                          //O segundo byte e o valor da resposta
          codigoPergunta = 0;
          valorPergunta = 0;
          fechadas = false;
      delay(1000);                                    
      abertura();
      }
   }}
    while(serialExtra.available()){
    int t = serialExtra.read();
    }
        
    }

          codigoPergunta = 0;
          valorPergunta = 0;
     
  } 





   


/*O Arduino Nano e o gerenciador dos motores
   Ele recebe atraves de comunicacao serial, pedidos de abertura
   ou fechamento das portas. alem de solicitacoes de icamento e abaixamento da cabine.
   O processamento dele e simples:
   1) deve analisar qual foi o pedido, se e de abertura ou fechamento;
   2) deve analisar onde esta posicionada a cabine;
   3) deve analisar os estados de cada um dos quatro sensores de fim-de-curso das portas;
   5) se possÃ­vel deve iniciar o pedido e observa-lo ate o final
   6) deve retornar o resultado a quem fez o pedido
*/

//Porta Serial extra
#include <AltSoftSerial.h>
AltSoftSerial serialExtra; //TX=9 RX=8

//Ultrassonico
#include <Ultrasonic.h>
#define TRIGGER_PIN  6 // (3 no flat)
#define ECHO_PIN     7 // (2 no flat)
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
int pinoPEA = 0;
int pinoPEF = 0;
int pinoPDA = 0;
int pinoPDF = 0;
int saidaAMD;
int saidaBMD;
int saidaAME;
int saidaBME;
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
  pinoPEA = 2;
  pinMode(pinoPEA, INPUT); //Sensor de porta da esquerda aberta (NF)
  pinoPEF = 3;
  pinMode(pinoPEF, INPUT); //Sensor de porta da esquerda fechada (NA)
  pinoPDF = 4;
  pinMode(pinoPDF, INPUT); //Sensor de porta da direita fechada (NA)
  pinoPDA = 5;
  pinMode(pinoPDA, INPUT); //Sensor de porta da direita aberta (NF)

  //Pinos digitais declarados acima
  /*Pino digital 6 vai para o trigger do ulrassonico superior
    Pino digital 7 vai para o echo do ulrassonico superior
    Pino digital 8 e o RX extra - Comunicacao com Uno
    Pino digital 9 e o TX extra - Comunicacao com Uno*/

  //Motores das portas
  saidaAMD = 10;
  pinMode(saidaAMD, OUTPUT); //Saida A do motor da direita
  saidaBMD = 11;
  pinMode(saidaBMD, OUTPUT); //Saida B do motor da direita
  saidaAME = 12;
  pinMode(saidaAME, OUTPUT); //Saida A do motor da esquerda
  saidaBME = 13;
  pinMode(saidaBME, OUTPUT); //Saida B do motor da esquerda

  //Saida para o circuito de icamento
  pinMode(A0, OUTPUT); //Permissividade para a ponte
  pinMode(A1, OUTPUT); //Saida A - Sentido de subida
  pinMode(A2, OUTPUT); //Saida A' - Sentido de descida


}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                  Metodo fechamento                                        //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void fechamento() {

  //FECHAMENTO
  //Fecha ambas portas caso ambas estejam abertas e calcula o tempo

  tempoInicial = millis();
  Serial.println("Fechando portas");

  pea = digitalRead(pinoPEA);
  pef = !digitalRead(pinoPEF);
  pdf = !digitalRead(pinoPDF);
  pda = digitalRead(pinoPDA);

  while (!pdf || !pef) //Realiza o loop enquanto uma das portas ainda nao estiver totalmente fechada
  {

    pea = digitalRead(pinoPEA);
    pef = !digitalRead(pinoPEF);
    pdf = !digitalRead(pinoPDF);
    pda = digitalRead(pinoPDA);

    Serial.println("PEA:= " + String(pea));
    delay(20);
    Serial.println("PEF:= " + String(pef));
    delay(20);
    Serial.println("PDF:= " + String(pdf));
    delay(20);
    Serial.println("PDA:= " + String(pda));
    delay(20);

    if (!pdf) //Testa se a porta da direita se fechou completamente
      //Observar que a porta quando fechada pdf vai de 1 para 0.
      //Caso nao esteja fechada ainda, mantem comando de fechamento.
    {
      digitalWrite(saidaAMD, LOW);
      digitalWrite(saidaBMD, HIGH);
    }
    else
    {
      digitalWrite(saidaAMD, LOW);
      digitalWrite(saidaBMD, LOW);
    }
    if (!pef) //Testa se a porta da esquerda se fechou completamente
      //Observar que a porta quando fechada pef vai de 1 para 0.
      //Caso nÃ£o esteja fechada ainda, mantÃ©m comando de fechamento.
    {
      digitalWrite(saidaAME, LOW);
      digitalWrite(saidaBME, HIGH);
    }
    else
    {
      digitalWrite(saidaAME, LOW);
      digitalWrite(saidaBME, LOW);
    }
  }
  tempoFinal = millis();
  Serial.println("Fechadas em: " + String((tempoFinal - tempoInicial) / 1000) + " seg");

  //Garante a parada dos motores
  digitalWrite(saidaAME, LOW);
  digitalWrite(saidaBME, LOW);
  digitalWrite(saidaAMD, LOW);
  digitalWrite(saidaBMD, LOW);


}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                   Metodo abertura                                         //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void abertura() {

  //ABERTURA
  //Abre ambas portas caso ambas estejam fechada e calcula o tempo

  Serial.println("Abrindo portas");

  pea = digitalRead(pinoPEA);
  pef = !digitalRead(pinoPEF);
  pdf = !digitalRead(pinoPDF);
  pda = digitalRead(pinoPDA);

  tempoInicial = millis();
  while (!pda || !pea) //Realiza o loop enquanto uma das portas ainda nao estiver totalmente aberta

  {

    pea = digitalRead(pinoPEA);
    pef = !digitalRead(pinoPEF);
    pdf = !digitalRead(pinoPDF);
    pda = digitalRead(pinoPDA);

    Serial.println("PEA:= " + String(pea));
    delay(20);
    Serial.println("PEF:= " + String(pef));
    delay(20);
    Serial.println("PDF:= " + String(pdf));
    delay(20);
    Serial.println("PDA:= " + String(pda));
    delay(20);

    if (!pda) //Testa se a porta da direita se abriu completamente
      //Observar que a porta quando aberta pda vai de 0 para 1.
      //Caso nao esteja fechada ainda, mantem comando de abertura.
    {

      digitalWrite(saidaAMD, HIGH);
      digitalWrite(saidaBMD, LOW);
    }
    else
    {
      //Ambos sinais da mesma porta em 0 desativa o lado da ponte correspondente
      //ao motor desta porta.
      digitalWrite(saidaAMD, LOW);
      digitalWrite(saidaBMD, LOW);
    }
    if (!pea) //Testa se a porta da esquerda se abriu completamente
      //Observar que a porta quando aberta pea vai de 0 para 1.
      //Caso nao esteja fechada ainda, mantem comando de abertura.
    {
      digitalWrite(saidaAME, HIGH);
      digitalWrite(saidaBME, LOW);
    }
    else
    {
      digitalWrite(saidaAME, LOW);
      digitalWrite(saidaBME, LOW);
    }

  }
  tempoFinal = millis();
  Serial.println("Abertas em: " + String((tempoFinal - tempoInicial) / 1000) + " seg");

  digitalWrite(saidaAMD, LOW);
  digitalWrite(saidaBMD, LOW);
  digitalWrite(saidaAME, LOW);
  digitalWrite(saidaBME, LOW);

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                  Metodo loopPortas                                        //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//
void loopPortas() {



  pea = digitalRead(pinoPEA);
  pef = !digitalRead(pinoPEF);
  pdf = !digitalRead(pinoPDF);
  pda = digitalRead(pinoPDA);

  boolean testeInicial = false;
  if (pda == 0 && pdf == 0 && pea == 0 && pef == 0)
  { testeInicial = true;
    Serial.println("Condicao 0");
  }
  if (pdf && pef || testeInicial) //se ambas as portas estiverem totalmente fechadas, entra no loop
  {
    abertura();
    //Atraso entre a abertura e fechamento
    delay(3000);
  }

  if (pda && pea || testeInicial) //se ambas as portas estiverem totalmente abertas, entra no loop
  {
    fechamento();
    //Atraso entre a abertura e fechamento
    delay(3000);
  }

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                     Metodo subir                                          //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void subir() {

  if (!fechadas) {
    //fechamento();
    fechadas = true;
  }

  //Entra no loop enquanto nao atingir a altura setada

  microsec = sensorDistancia.timing();
  distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);

  while (distancia >= 30) {
    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);
    Serial.println("Subindo " + String(distancia));

    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
  }


  //Bloqueia a ponte
  digitalWrite(A0, LOW);

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                    Metodo descer                                          //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void descer() {

  if (!fechadas) {
    //fechamento();
    fechadas = true;
  }

  microsec = sensorDistancia.timing();
  distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);

  //Entra no loop enquanto nao atingir a altura setada
  while (distancia <= 80) {
    microsec = sensorDistancia.timing();
    distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);

    Serial.println("Descendo " + String(distancia));

    digitalWrite(A0, HIGH); //Permissivo
    digitalWrite(A1, LOW);
    digitalWrite(A2, HIGH);
  }

  //Fim do loop
  //Coloca o permissivo em 0, bloqueando a ponte
  digitalWrite(A0, LOW);

  Serial.println("Termino de descida");

}

//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//                                                 Metodo loopMotores                                        //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

void loopMotores() {

  //Metodo apenas para debug e comissionamento
  /*-------------------------------------------------------------------------------------------------------------*/
  //Condicoes iniciais (devera ser usado para Warm-up)
  //Coloca o permissivo em 0, bloqueando a ponte
  digitalWrite(A0, LOW);
  Serial.println("Distancia inicial: " + String(distancia));

  /*-------------------------------------------------------------------------------------------------------------*/

  //Sobe
  tempoInicial = millis() / 100;
  //subir();
  tempoFinal = millis() / 100;
  digitalWrite(A0, LOW);
  ///Serial.println("Subida em " + String(tempoFinal - tempoInicial));

  //Loop das portas
  loopPortas();

  //Desce
  tempoInicial = millis() / 100;
  //descer();
  tempoFinal = millis() / 100;
  ///Serial.println("Descida em " + String(tempoFinal - tempoInicial));

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
  digitalWrite(A0, LOW); //Permissividade
  digitalWrite(A1, LOW);
  digitalWrite(A2, !digitalRead(A1));

  //Serial.println("Incio da rotina NANO "+ String(millis()/1000));

  microsec = sensorDistancia.timing();
  distancia = sensorDistancia.convert(microsec, Ultrasonic::CM);
  //Serial.println(String(distancia));

  /*-------------------------------------------------------------------------------------------------------------*/
  //Inicia testes dos motores. Faz as portas se fecharem, sobe a cabine, abre as portas, fecha as portas
  // desce a cabine, abre novamente as portas e torna a fechÃ¡-las em um loop infinito. Escrito para comissionamento
  loopMotores();

  ////Escritas no UNO
  ////    distancia = sensorDistancia.convert(sensorDistancia.timing(), Ultrasonic::CM);
  ////    byte protocolo[]={1,distancia}; //Armazena o valor da distÃ¢ncia da cabine ao sensor
  ////    if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
  ////    //Serial.println("Enviando: " + String(distancia));
  ////    serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte Ã© o cÃ³dgo da resposra.
  ////                                    //O segundo byte Ã© o valor da resposta
  ////
  ////          while(serialExtra.available()>0){
  ////            int t = serialExtra.read();
  ////            Serial.println("Consumindo buffer");
  ////            }
  ////
  ////    }else{
  ////      //Serial.println("Buffer cheio");
  ////      }
  //
  ///*-------------------------------------------------------------------------------------------------------------*/
  //
  ////Leituras do UNO
  //  if(serialExtra.available()==2){ //O byte 2 contem os codigo das perguntas
  //    codigoPergunta = serialExtra.read();
  //
  //        if(serialExtra.available()==1){ //O byte 1 contem o valor da pergunta
  //        valorPergunta = serialExtra.read();
  //
  //      }
  //  }
  //
  //if(serialExtra.available()==0){
  //   if(codigoPergunta==255){
  //   Serial.println("UNO perguntou: " + String(codigoPergunta) + "|" + String(valorPergunta));
  //
  //    //DESCIDA
  //    if(valorPergunta==3){
  //      //Solicita que a cabine desça ao primeiro piso
  //      Serial.println("Enviando fim de descida - fila");
  //      descer();
  //    while(serialExtra.available()){
  //    int t = serialExtra.read();
  //    }
  //      byte protocolo[]={3,1}; //Envia o código de resposta à pergunta 3 com resposta 1
  //          if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
  //          Serial.println("Enviando fim de descida");
  //          serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte eh o codigo da resposra.
  //                                          //O segundo byte e o valor da resposta
  //          codigoPergunta = 0;
  //          valorPergunta = 0;
  //          fechadas = false;
  //      delay(1000);
  //      abertura();
  //      }
  //   }
  //
  //
  //
  //
  //    //SUBIDA
  //    else if(valorPergunta==2){
  //      //Solicita que a cabine suba ao segundo piso
  //      Serial.println("Enviando fim de subida - fila");
  //      subir();
  //    while(serialExtra.available()){
  //    int t = serialExtra.read();
  //    }
  //      byte protocolo[]={2,1}; //Envia o código de resposta à pergunta 3 com resposta 2
  //          if(serialExtra.available()==0){ //So inicia transmissao se o buffer estiver vazio
  //          Serial.println("Enviando fim de subida");
  //          serialExtra.write(protocolo,2); //Manda 2 bytes para o Uno. O primeiro byte eh o codigo da resposra.
  //                                          //O segundo byte e o valor da resposta
  //          codigoPergunta = 0;
  //          valorPergunta = 0;
  //          fechadas = false;
  //      delay(1000);
  //      abertura();
  //      }
  //   }}
  //    while(serialExtra.available()){
  //    int t = serialExtra.read();
  //    }
  //
  //    }
  //
  //          codigoPergunta = 0;
  //          valorPergunta = 0;
  //
}








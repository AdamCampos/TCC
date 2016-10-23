//Variáveis globais

//Entradas digitais
int sensorPortaEsquerdaAberta = 2;
int sensorPortaDireitaAberta = 3;
int sensorPortaEsquerdaFechada = 4;
int sensorPortaDireitaFechada = 5;
int sensorAndar1 = 6;
int sensorAndar2 = 7;
int botaoChamadaCima = 8;
int botaoChamadaBaixo = 9;
int botaoCabineEmergencia = 10;
int botaoCabineBaixo= 11;
int botaoCabineCima = 12;

//Saídas digitais
int habilitaMotoresPorta = 13;
int ledPermissaoAbertura = 14;
int abrePortaEsquerda = 15;
int abrePortaDireita = 16;
int motorSobe = 17;
int motorDesce = 18;
int motorPara = 19;

void setup() {
//Variáveis de classe

//Seta os pinos como entradas digitais e seta-os como falso.
for(int i=2; i<=12;i++){
   pinMode(i, INPUT);
   digitalWrite(i,LOW);
  }

//Seta os pinos como saídas digitais e seta-os como falso.
for(int i=13; i<=18;i++){
   pinMode(i, OUTPUT);
   digitalWrite(i,LOW);
  }

Serial.begin(9600);
 
}

void loop() {
//Variáveis de método

  /*******************************************************************************************************/
  //Ao ser solicitada a abertura das portas o método "podeAbrirPortas" é chamado para verificação.
  //As portas só poderão ser abertas se o elevador estiver parado em um dos dois andares.
  boolean podeAbrirPortas = abrePortas();
  String permissao;
  if(podeAbrirPortas==1){
    permissao = "Abertura permitida";
  }
  else{
    permissao = "Abertura não permitida";
  }
  
  Serial.println(permissao);
   /*******************************************************************************************************/
   //Todo botão apertado sofrerá a ação de selo (latching) e será um toggle
	static int matrizBotoes[] = {8, 9, 10, 11, 12};  
	static int old8

   for(int i=0; i<=4; i++){
   
     if(digitalRead(i)==HIGH){
      toggle(i);

        if(matrizBotoes[i]==8){
        digitalWrite(15,HIGH);
        digitalWrite(16,HIGH);}
        
      unsigned long agora;
      agora = millis()/1000;
      Serial.print(matrizBotoes[i]);
      Serial.print(" ");
      Serial.println(agora);
      }
   }
   /*******************************************************************************************************/
}

//==========================================================================================================//
boolean abrePortas(){
  /*As portas só podem ser abertas se a cabine estiver parada em um dos dois andares
  Os sensores "sensorAndar1" e "sensorAndar2" ativados são permissivos para abertura das portas. */

boolean podeAbrirPortas;
	
  /* Um dos dois sensores de andar devem estar setados. 
  Eles são ativados quando o piso da cabine encosta no sensor. */
  if(sensorAndar1==HIGH || sensorAndar2==HIGH){
    podeAbrirPortas = true;
    }
  else{
    podeAbrirPortas = false;
  }
return podeAbrirPortas;
  }
//==========================================================================================================//
void toggle(int entrada){
  
}


/*   Controle de prensa para ammarrar o fardo de aparas.
  Este arquivo foi criado por Cesar Costa e Samuel Oliveira Costa
  São Paulo, 19/10/2020.

  DDR que configura o sentido da porta se ela é saída (OUTPUT) ou entrada (INPUT). Substitui a função pinMode().
      se é saída (OUTPUT), usando o 1 ou se é entrada (INPUT), usando o 0.
  PORT que controla se a porta está no estado lógico alto (HIGH) ou baixo (LOW).
  PIN que efetua a leitura das portas configuradas como INPUT pela função pinMode().

  Exemplos:
  Para definir um pino como saída devemos setar seu respectivo bit do
  registrador DDR como 1 e  para defini-lo como entrada seta-lo para 0.
  pinMode(9,OUTPUT);DDRB |= (1 << DDB1);
  pinMode(9,INPUT);DDRB &= ~(1 << DDB1);

*/



//controlar registradores

#define SetBit(RES, BIT)(RES |= (1 << BIT))   // Por BIT em nível alto
#define ClrBit(RES, BIT)(RES &= ~ (1 << BIT)) // Por BIT em nível baixo
#define TstBit(RES, BIT)(RES &  (1 << BIT)) // testar BIT, retorna 0 ou 1
#define CplBit(RES, BIT)(RES ^= (1 << BIT)) // Inverter estado do BIT

// definir portas de Sensores
//Legenda Nome    var Pino  Registrador
#define PF 1 //  Presa Frente PF  9 PB1
#define PT 2 //  Presa Traz PT  10  PB2
#define AF 3 //  Agulha Fernte  AF  11  PB3
#define AT 4 //  Agualha Traz AT  12  PB4
#define SC 2 //  Sinal Contador SC  A2  PC2
#define BI 1 //  Botão Inicio BI  A1  PC1
#define BR 0 //  Botão Reset Co BR  A0  PC0

// definir portas de Relés
//Legenda   Nome    var Pino  Registrador
#define RPF 2 //  Relé Presa Frente RPF 2 PD2
#define RPT 3 //  Relé Presa Traz RPT 3 PD3
#define RPD 4 //  Relé Presa Direção  RPD 4 PD4
#define RAF 5 //  Relé Agulha Frente  RAF 5 PD5
#define RAT 6 //  Relé Agulha Traz  RAT 6 PD6
#define RSS 7 //  Relé Sinal Sirene RSS 7 PD7
#define RBR 0 //  Relé Botão Reset Cont RBR 8 PB0

//Variaveis de Tempo
unsigned long tempoPistaoFrente = 180000;
unsigned long tempoPistaoTraz = 180000;
unsigned long tempoAgulhaFrente = 180000;
unsigned long tempoAgulhaTraz = 180000;
unsigned long tempoDirecional = 5000;
unsigned long tempoDirecionalAgulha = 1000;
unsigned long ativar;

void setup() {
  DDRB = 0x1; //Define todos os pinos PB como entrada menos 0 = RBR
  DDRC = 0x0; //Define todos os pinos PC como entrada
  DDRD = 0xFF; //Define todos como saida
  PORTB = 0xFE; //Define todos PB PullUP, menos 0 = RBR
  PORTC = 0xFF; //Define todos PC PullUp
  SetBit(PORTB, SC); // Define PullUp porta para verificar
  PORTD = 0xFF; //Define todas Saidas PD como HIGH
  SetBit(PORTB, RBR); //Define rele botão reset HIGH
  SetBit(DDRB, 5);
  SetBit(PORTB, 5);
  Emergencia();

}

void loop() {
  Emergencia();
  if (!TstBit(PINC, BI)) { // se botão iniciar por apertado
    ClrBit(PORTD, RPF); //liga rele de prensa para Frente

    ativar = millis(); // Registrar tempo de inicio do relé
    while (TstBit(PINB, PF)) {
      delay(50);
      if (!TstBit(PINB, PF))delay(50);
      if (millis() - ativar > tempoPistaoFrente)PararTudo(); //parar tudo por excesso de tempo
      Emergencia();  // se caso o sensor da agulha parar de identificar AT (agulha para traz) para os pistões
      ClrBit(PORTD, RPF); //liga rele de prensa para Frente
      delay(10);


    } // caminhar até o fim do curso do pistão
    SetBit(PORTD, RPF); // desliga o pistão da prensa
    delay(500);
    if (!TstBit(PINC, SC)) { //verifica se o contador esta ativado,
      delay(50);
      if (!TstBit(PINC, SC)) { //
        ClrBit(PORTD, RAF); //Entra direcional de agulha
        delay(150);
      }
      // if( TstBit(PINB,PF))PararTudo(); // remover não funciona

      ClrBit(PORTD, RSS); //liga a sirene
      delay(300);
      ClrBit(PORTD, RPD); //inicia o movimento da agulha
      delay(300);

      ativar = millis(); // Registrar tempo de inicio do relé
      while (TstBit(PINB, AF)) {//sensor da agulha frente
        delay(100);
        if (!TstBit(PINB, AF))delay(50); //agulha frente ler e aguardar
        if (millis() - ativar > tempoAgulhaFrente)PararTudo(); //parar tudo por excesso de tempo
        //if (!TstBit(PINB,PF))PararTudo();  //erro porque prensa voltou !!!!
        ClrBit(PORTD, RAF); //Entra com agulha
        ClrBit(PORTD, RPD);   //valvula direcional e reduz velocidade
        ClrBit(PORTD, RSS); //liga a sirene
      } // aguarda fim de curso da agulha para frente
      
      SetBit(PORTD, RAF); //Para Agulha
      delay(300);
      SetBit(PORTD, RPD); //Para Direcional
      delay(300);
      SetBit(PORTD, RSS); //para sirene
      delay(300);
      
      // if (TstBit(PINB, PF))PararTudo();// não funciona!!! 
      
      ClrBit(PORTB, RBR); // Libera Botão do reset para Contador
      delay(300);
      
      while (TstBit(PINC, BR)) {} // Aguarda amarrar fardo e press botão
      delay(800); //Delay para o contador ler o sinal do botão
      SetBit(PORTB, RBR); // Trava botão do Reset novamente
      delay(300);
      ClrBit(PORTD, RAT); // Retorna agualha para descanso
      delay(300);
      ClrBit(PORTD, RPD); // liga direcional
      delay(300);
      ativar = millis(); // Registrar tempo de inicio do relé
      while (TstBit(PINB, AT)) {
        delay(100);
        if (!TstBit(PINB, AT))delay(50);
        if (millis() - ativar > tempoAgulhaTraz)PararTudo(); //parar tudo por excesso de tempo
      } // Aguarda retorno da Agulha
      SetBit(PORTD, RAT); // Desliga agulha
      delay(300);
      SetBit(PORTD, RPD);//desliga rele da prensa direcional
      delay(300);
    }
    delay(300);
    ClrBit(PORTD, RPT); // Retorna pistão de prensa
    delay(300);

    ativar = millis(); // Registrar tempo de inicio do relé
    while (TstBit(PINB, PT)) {
      delay(100);
      if (!TstBit(PINB, PT))delay(50);
      if (millis() - ativar > tempoPistaoTraz)PararTudo(); //parar tudo por excesso de tempo
      Emergencia();
      ClrBit(PORTD, RPT); // Retorna pistão de prensa

    } // Aguarda fim de cusrso do Pistão de Prensa
  
 
    SetBit(PORTD, RPT);
    delay(300);
    
  }
}
//-----Funções----------------------------------------------------------------------
//PararTudo **** caso o acionamento de rele exceder tempo de execução
void PararTudo() {
  PORTD = 0xff; //Define todas Saidas PD como LOW
  while (1) {
    ClrBit(PORTD, RSS); //Liga sirene
    delay(500);
    SetBit(PORTD, RSS); // Liga Sirene
    delay(200);
    ClrBit(PORTD, RSS); //Liga sirene
    delay(500);
    SetBit(PORTD, RSS); // Liga Sirene
    delay(1000);
  }

}
void Emergencia() {

  while (TstBit(PINB, AT)) { // enquanto a agualha não estiver no final o sistema não inicia
    SetBit(PORTD, RPD); //Desliga pistão
    SetBit(PORTD, RPT);
    SetBit(PORTD, RPF);
    ClrBit(PORTD, RSS); //Liga sirene
    delay(400);
    SetBit(PORTD, RSS); // Liga Sirene
    delay(400);

  }
}

/*   Controle de prensa para ammarrar o fardo de aparas.
  Este arquivo foi criado por Cesar Costa e Samuel Oliveira Costa
  São Paulo, 19/10/2020.
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
#define SC 5 //  Sinal Contador SC  13  PB5
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

void setup() {
  DDRB = 0x1; //Define todos os pinos PB como entrada menos 0 = RBR
  DDRC = 0x0; //Define todos os pinos PC como entrada
  PORTB = 0xFE; //Define todos PB PullUP, menos 0 = RBR
  PORTC = 0xFF; //Define todos PC PullUp
  DDRD = 0xFF; //Define todos como saida
  PORTD = 0x0; //Define todas Saidas PD como LOW
}

void loop() {

  if (!TstBit(PINC, BI)) { // se botão iniciar por apertado
    SetBit(PORTD, RPF); //liga rele de prensa para Frente
    SetBit(PORTD, RPD);
    while (TstBit(PINB, PF)) {} // aguarda fiM do curso do pistão
    ClrBit(PORTD, RPF); // desliga o pistão da prensa
    ClrBit(PORTD, RPD);
    if (!TstBit(PINB, 5)) { //verifica se o contador esta ativado,
      SetBit(PORTD, RAF); //Entra com agulha
      while (TstBit(PINB, AF)) {} // aguarda fim de curso
      ClrBit(PORTD, RAF); // Para Agulha
      SetBit(PORTB, RBR); // Libera Botão do reset para Contador
      while (TstBit(PINC, BR)) {} // Aguarda amarrar fardo e press botão
      ClrBit(PORTB, RBR); // Trava botão do Reset novamente
      SetBit(PORTD, RAT); // Retorna agualha para descanso
      while (TstBit(PINB, AT)) {} // Aguarda retorno da Agulha
      ClrBit(PORTD, RAT); // Desliga agulha
  }
    SetBit(PORTD, RPT); // Retorna pistão de prensa
    SetBit(PORTD, RPD);
    while (TstBit(PINB, PT)) {} // Aguarda fim de cusrso do Pistão de Prensa
    ClrBit(PORTD, RPD); //Desliga pistão
    ClrBit(PORTD, RPT);
  }
}

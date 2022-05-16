/*
 * AUTOR : MARCELO MIGUEL CARDOSO
 * 
 * TESTADO EM ARDUINO LEONARDO E UNO
 * 
 * CÓDIGO QUE TEM POR FUÇÃO LER RPM BAIXA
 * POSSIBILITA MEDIR ORDEM DE 0.5 RPM ATÉ 500 COM GARANTIA
 * 
 * USAR PINO 4 DO ARDUINO LEONARDO
 * USAR PINO 8 DO ARDUINO UNO
 * 
 * TENSÃO MÁXIMA E SINAL 5V
 * 
 */

// --- Variáveis Globais --- 
unsigned int num_div = 1; //numero de divisões que terão no disco do encoder. (se tiver 10, para f=10 -> rpm=6)
float rpm; //contador de rpm
float Speed = 0;
volatile unsigned int C1, C2, capNum=0, overflowCount = 0;
volatile unsigned int ovfCountC1, ovfCountC2;
volatile bool ovfState = false, capState = false, captureDone = false, state_TM2 = false;
float Time = 0; unsigned long timer1Counts;


//=============================
//    protótipo das funções
void read_rpm();                                          //função que calcula a rpm com base no período


void setup() {
  Serial.begin(9600);                                     //inicia o serial em 9600 baud rate
   
  //configuração timer 1: 
  noInterrupts();                                         //desativa interrupçãoes para configurar
  TCCR1A = 0;                                             //initialize timer1 mode
  TCCR1B = 0;
  TCCR1B |= 0b11000101;                                   //set 1024 prescaler, rising edge, noise canceler
  TIMSK1 |= 0b00100001;                                   //enable input capture interrupt
  TCNT1 = 0;                                              //initialize timer/counter1 register
  interrupts();                                           //habilita interrupções
  
  delay(100);                                             //delay de setup
}//end setup


void loop() {
  read_rpm();                                             //cada loop calcula rpm
  Serial.print("freq: ");                                   
  Serial.print(float(1/Time),5);
  Serial.print("  rpm: ");
  Serial.println(float (rpm),5);                          //imprime valores calculados
  
  delay(100);                                             //delay para evitar travamentos
}




// --- Interrupções ---
ISR(TIMER1_CAPT_vect)
{
  capNum++;
  if(capNum == 1)
  {
    C1 = ICR1;
    ovfCountC1 = overflowCount;
  }
  if(capNum == 2)
  {
    C2 = ICR1;
    ovfCountC2 = overflowCount;
    captureDone = true;
  }
  capState = !capState;
  }

//-----------------------------
ISR(TIMER1_OVF_vect)
{         TCNT1 = 0;
          overflowCount++;
          ovfState = !ovfState;
}



//--------FUNÇÕES---------
// =================================================================================
//funções
void read_rpm(){
  if(captureDone){
  timer1Counts = 65536 - C1 + (ovfCountC2 - ovfCountC1 - 1) * (65536) + C2;//(C2-C1+(ovf2-ovf1)*65535)*64us
  Time = float(timer1Counts)*64.0e-6;
  rpm = float(60/(Time*num_div));
  captureDone = false; capNum = 0;
  }
}

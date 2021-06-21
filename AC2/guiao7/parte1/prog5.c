
#include<detpic32.h>
#define SAMPLES 8
#define VOLTAGE(V)   (V*33)/1023                                             // get voltage value 

void ConfigAll(void);
void send2displays_this(unsigned char value);
unsigned char toBcd( unsigned char value);

volatile int voltage= 0;
int main(void)
{
  ConfigAll();
  
  // Reset AD1IF, T1IF and T3IF flags 
  IFS0bits.T1IF = 0;
  IFS0bits.T3IF = 0;
  IFS1bits.AD1IF = 0;
 EnableInterrupts();

  
  while(1);
  return 0;
}

void ConfigAll(void)
{
  // A/D configs
  TRISB = TRISB & 0x00FF;
  TRISDbits.TRISD5 = 0;
  TRISDbits.TRISD6 = 0;
  TRISBbits.TRISB4 = 1;
  AD1PCFGbits.PCFG4 = 0;
  AD1CON1bits.SSRC = 7;
  AD1CON1bits.CLRASAM = 1;
  AD1CHSbits.CH0SA = 4;
  AD1CON2bits.SMPI = SAMPLES-1;
  AD1CON3bits.SAMC = 16;
  AD1CON1bits.ON = 1;
  // ISR configs
  IPC6bits.AD1IP = 2; // A/D interrupts priority 
  IEC1bits.AD1IE = 1; // A/D interrupts enabled
  IFS1bits.AD1IF = 0; // clear A/D flag

  //Timers configs
    T1CONbits.TCKPS = 7;
    PR1 = 19530;
    TMR1 = 0;
    T1CONbits.TON = 1;
    T3CONbits.TCKPS = 2;
    PR3 = 49999;
    TMR3 = 0;
    T3CONbits.TON = 1;
  //Timers interrupts
    IPC1bits.T1IP = 2;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    IPC3bits.T3IP = 2;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
  EnableInterrupts();
}
void _int_(4) isr_T1(void)       {         
    
// Start A/D conversion 
AD1CON1bits.ASAM = 1;

 // Reset T1IF flag    
IFS0bits.T1IF = 0;   
} 
// void _int_(12) isr_T3(void)       {          
//     // Send "voltage" global variable to displays   
//     send2displays_this(voltage); 
//           // Reset T3IF flag    
//     IFS0bits.T3IF = 0; 
//           } 
void _int_(27) isr_adc()
{
  int buf_avg = 0;
  int *bp = (int *)(&ADC1BUF0);
  int i;
  for(i = 0; i < SAMPLES; i++)
  {
    buf_avg += bp[i*4];
  }
  buf_avg /= SAMPLES;
  buf_avg = VOLTAGE(buf_avg);
  buf_avg;
  voltage = toBcd((char)buf_avg);

  IFS1bits.AD1IF = 0;
}
unsigned char toBcd( unsigned char value)
{
  return ((value / 10) << 4) + value % 10;
}
void send2displays_this(unsigned char value)
{
  static const char display7Scodes[] = { 
    0x3F, 0x06, 0x5B, 0X4F, 0x66, 0x6D, 0X7D, 0X07, 
    0X7F, 0X6F, 0X77, 0X7C, 0X39, 0X5E, 0X79, 0X71
  };

  static char displayFlag = 0;
  unsigned char digit_low = (value & 0x0F);
  unsigned char digit_high = (value & 0xF0) >> 4;
  char hexCode;

  if (displayFlag)
  {
    LATDbits.LATD5 = 1;
    LATDbits.LATD6 = 0;

    hexCode = display7Scodes[digit_low];
    LATB = (LATB & 0x00FF) | ((int)hexCode << 8);
  }
  else
  {
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 1;

    hexCode = display7Scodes[digit_high];
    LATB = (LATB & 0x00FF) | ((int)hexCode << 8);
    
    
  }

  displayFlag = !displayFlag;
}
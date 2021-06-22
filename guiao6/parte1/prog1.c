#include <detpic32.h>
#define SAMPLES 1

void delay(int ms);
void send2displays(unsigned char value);
void IOConfig(void);

int main(void)
{
  IOConfig();
  AD1CON1bits.ASAM = 1;
  while(1){}
  return 0;
}

void IOConfig(void)
{
  // A/D configs
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

  EnableInterrupts();
}

void _int_(27) isr_adc()
{
  printStr("\nBuffers:\n");
  int *bp = (int *)(&ADC1BUF0);
  printInt(*bp, 16 | 3 << 16);
  printStr("\n");
  AD1CON1bits.ASAM = 1;
  IFS1bits.AD1IF = 0;
}

void send2displays(unsigned char value)
{
  static const char display7Scodes[] = { 
    0x3F, 0x06, 0x5B, 0X4F, 0x66, 0x6D, 0X7D, 0X07, 
    0X7F, 0X6F, 0X77, 0X7C, 0X39, 0X5E, 0X79, 0X71
  };
  
  unsigned char digit_low = (value & 0x0F);
  unsigned char digit_high = (value & 0xF0) >> 4;
  char hexCode;

  // low part first
  LATDbits.LATD5 = 1;
  LATDbits.LATD6 = 0;

  hexCode = display7Scodes[digit_low];
  LATB = (LATB & 0x00FF) | ((int)hexCode << 8);
  delay(10);
  
  // high part second
  LATDbits.LATD5 = 0;
  LATDbits.LATD6 = 1;

  hexCode = display7Scodes[digit_high];
  LATB = (LATB & 0x00FF) | ((int)hexCode << 8);
  delay(10);
}

void delay(int ms)
{
  for (; ms > 0; ms--){
    resetCoreTimer();
    while(readCoreTimer() < 20000);
  }
}
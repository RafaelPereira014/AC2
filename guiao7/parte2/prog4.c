
#include<detpic32.h>
#define SAMPLES 8
#define VOLTAGE(V)   (V*33)/1023                                             // get voltage value 

void ConfigAll(void);
void send2displays(unsigned char value);
unsigned char toBcd( unsigned char value);
void setPWM(unsigned int dutyCycle);

volatile unsigned char  voltage= 0x00;
int main(void)
{
  int dutyCycle,portVal;
    ConfigAll();
    EnableInterrupts(); // Global Interrupt Enable 
    while(1)
    {
      // Read RB1, RB0 to the variable "portVal"
      portVal = PORTB && 0x0003;
      switch(portVal)
      {
         case 0:  // Measure input voltage
            // Enable T1 interrupts
             IEC0bits.T1IE = 1;
            setPWM(0);  // LED OFF
            break;
         case 1:  // Freeze
            // Disable T1 interrupts
            IEC0bits.T1IE = 0;
            setPWM(100); // LED ON (maximum bright)
            break;
         default: // LED brigthness control
            // Enable T1 interrupts
            IEC0bits.T1IE = 1;
            dutyCycle = voltage * 3;
            setPWM(dutyCycle);
            break;
             }
        } 
    }
void setPWM(unsigned int dutyCycle){
    if(dutyCycle <= 100 && dutyCycle >= 0){
        OC1RS = (780 * dutyCycle)/100;       // OC1RS = PRx * dutyCicle / 100 
    }
}

void ConfigAll(void)
{
// hexDisp configs
  LATB = LATB & 0x00FF;
  TRISB = TRISB & 0x00FF;
  TRISDbits.TRISD5 = 0;
  TRISDbits.TRISD6 = 0;

    // Configure the A/D module and port RB4 as analog input // potenciometro
    TRISBbits.TRISB4 = 1;           // RB4 digital output disconnected
    AD1PCFGbits.PCFG4 = 0;          // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7;           

    AD1CON1bits.CLRASAM = 1;

    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 7;           // 8 amostras consecutivas CUIDADO

    AD1CHSbits.CH0SA = 4;

    AD1CON1bits.ON = 1;             // Enable A/D converter

    // Configure interrupt system
    IPC6bits.AD1IP = 2;             // configure priority of A/D interrupts to 2

    IEC1bits.AD1IE = 1;             // enable A/D interrupts

    // Configure display
    TRISB = TRISB & 0x00FF;                  // configure RB8-RB14 as outputs         
    TRISD = TRISD & 0xFF9F;                 // configure RD5-RD6 as outputs

    // Configure RB0 e RB1 to input
    TRISB = TRISB | 0x3;
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
void _int_(12) isr_T3(void)       {          
    // Send "voltage" global variable to displays   
    send2displays(voltage); 
          // Reset T3IF flag    
    IFS0bits.T3IF = 0; 
          } 
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
void send2displays(unsigned char value){
    static const char display7Scodes[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
    static char display_flag = 0; // variavel estática: não altera o seu valor em chamadas consecutivas a uma função

    unsigned char digit_low = value % 10;           // cuidado
    unsigned char digit_high = (int) (value/10);

    if(display_flag){
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;

        LATB = (LATB & 0x80FF) | display7Scodes[digit_low] << 8;
    }else{
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;

        LATB = (LATB & 0x80FF) | display7Scodes[digit_high] << 8;
    }

    display_flag = !display_flag;
}
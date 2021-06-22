#include <detpic32.h>

volatile int voltage = 0;   // Global variable 

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

// admitindo que a configuração do módulo de PWM já foi feita
void setPWM(unsigned int dutyCycle){
    if(dutyCycle <= 100 && dutyCycle >= 0){
        OC1RS = (780 * dutyCycle)/100;       // OC1RS = PRx * dutyCicle / 100 
    }
}

void configureAll(){

    // configure Timer 1 (Type A)
    T1CONbits.TCKPS = 3;
    PR1 = 19530;
    TMR1 = 0;
    T1CONbits.TON = 1;          
    // configure interrupts for Timer 1
    IPC1bits.T1IP = 2;
    IEC0bits.T1IE = 1;


    // configure Timer 3 (Type B)
    T3CONbits.TCKPS = 7;
    PR3 = 780;
    TMR3 = 0;
    T3CONbits.TON = 1;
    // configure interrupts for Timer 3
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;

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

   

    // configure PWM
    OC1CONbits.OCM = 6;         // PWM mode on OCx; fault pin disabled    
    OC1CONbits.OCTSEL = 0;      // Use timer T2 as the time base for PWM generation    
    OC1RS = 12500;              //Ton constante defenida na função setPWM    
    OC1CONbits.ON = 1;          // Enable OC1 module 
}

// isr for timer 1 - 4 Hz, conversion frequecy
void _int_(4) isr_T1(void){
    AD1CON1bits.ASAM = 1;       // start ADC conversion
    IFS0bits.T1IF = 0;          // reset Timer1 flag
}

// isr for timer 3 - 100 Hz, display refresh time
void _int_(12) isr_T3(void){
    send2displays(voltage);

    IFS0bits.T3IF = 0;          // reset Timer3 flag
}

// Interrupction Handler
void _int_(27) isr_adc(void){

    // calculate buffer average
    int* p = (int*) (&ADC1BUF0);

    int sum = 0;
    int i;
    for(i = 0; i < 8; i++){
        sum = sum + p[i*4];
    }

    int med_voltage = sum/8;
    // printInt10(med_voltage);
    // putChar('-');
    
    // printInt10(ADC1BUF0);
    // putChar('-');
    // printInt10(med_voltage / ADC1BUF0);
    // putChar('\n');

    // med_voltage = ADC1BUF0;

    voltage = (med_voltage*33 + 511) / 1023;
    
    IFS1bits.AD1IF = 0;             // clear/reset A/D converter interrupt flag   
}

int main(void){          
    configureAll(); // Function to configure all (digital I/O, analog                           
                    // input, A/D module, timers T1 and T3, interrupts)          
    
    // Reset AD1IF, T1IF and T3IF flags
    IFS1bits.AD1IF = 0;         // clear/reset A/D interrupt flag
    IFS0bits.T3IF = 0;          // reset timer 3 flag
    IFS0bits.T1IF = 0;          // reset timer 1 flag

    EnableInterrupts();            // Global Interrupt Enable

    int dutyCycle;
    int portVal;

    while(1){
        portVal = PORTB & 0x3;

        // dprintInt10(portVal);

        switch(portVal){
        case 0:
            IEC0bits.T1IE = 1;      // enable timer 1 interrupts
            setPWM(0);              // LED OFF
            break;

        case 1:
            IEC0bits.T1IE = 0;      // disable timer 1 interrupts
            setPWM(100);            // LED ON (maximum bright)
            break;
        
        default:
            IEC0bits.T1IE = 1;      // enable timer 1 interrupts
            dutyCycle = voltage * 3;
            setPWM(dutyCycle);
            break;
        }
    }

    return 0;       
}
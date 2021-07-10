#include <detpic32.h>

//ler da adc com interrupçoes e meter no display o valor lido em hexadecimal. frequencia da adc de 10 hz e frequencia refrescamento do display 120hz atraves do timer2.


volatile int v;
void delay(int ms);
void send2displays(unsigned char value);


int main(void){
    // Displays
	TRISDbits.TRISD6 = 0;
	TRISDbits.TRISD5 = 0;
	TRISB = TRISB & 0x80FF;
	LATB = LATB & 0x80FF;
    
    //configurar ADC
    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4= 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 2-1;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    //interrupcoes ADC
    IPC6bits.AD1IP = 2;
    IFS1bits.AD1IF = 0;
    IEC1bits.AD1IE = 1;

    //configurar Timer
    T2CONbits.TCKPS = 2;
    PR2 = 41666;
    TMR2 = 0;
    T2CONbits.TON=1;
    //interrupcoes timer2
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;

    AD1CON1bits.ASAM = 1;
    IFS0bits.T2IF=0;
    EnableInterrupts();
    while (1)
    {
    
    }
    return 1;
}

void _int_(8) isr_timer2(){
    send2displays(v);
    IFS0bits.T2IF = 0;
}
void _int_(27) isr_adc(){
    v = ADC1BUF0;  
    delay(100);  
    AD1CON1bits.ASAM = 1;
    IFS1bits.AD1IF = 0; 
}
void send2displays(unsigned char value)
{
    static const char display7Scodes[] = {
                                        0x3F, //0
                                        0x06, //1
                                        0x5B, //2
                                        0x4F, //3
                                        0x66, //4
                                        0x6D, //5
                                        0x7D, //6
                                        0x07, //7
                                        0x7F, //8
                                        0x6F, //9
                                        0x77, //A
                                        0x7C, //b
                                        0x39, //C
                                        0x5E, //d
                                        0x79, //E
                                        0x71  //F
                                        };
    
    static char displayFlag = 0;

    unsigned char dh = value >> 4;      // Get the index of the decimal part
    unsigned char dl = value & 0x0F;    // Get the index of the unitary part
    
    // Get the correct hex code for the number
    dh = display7Scodes[dh];
    dl = display7Scodes[dl];
    
    if (displayFlag == 0)
    {
        LATD = (LATD | 0x0040) & 0xFFDF;    // Dipslay High active and Display Low OFF
        LATB = (LATB & 0x80FF) | ((unsigned int)(dh)) << 8; // Clean the display and set the right value
    } else {
        LATD = (LATD | 0x0020) & 0xFFBF;    // Display High OFF and Display High active
        LATB = (LATB & 0x80FF) | ((unsigned int)(dl)) << 8; // Clean the display and set the right value
    }

    displayFlag = !displayFlag;
}


void delay(int ms)
{
    while (ms > 0)
    {
        resetCoreTimer();
        while (readCoreTimer() < 20000)
        {
        }
        ms -= 1;
    }
}
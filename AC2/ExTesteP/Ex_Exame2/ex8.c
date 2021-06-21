
#include <detpic32.h>

volatile int v;
volatile int freq;


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

void printRBs(void)
{
    printStr("RB3: ");
    printInt((PORTB & 0x0008) >> 3, 2);

    printStr(", ");

    printStr("RB2: ");
    printInt((PORTB & 0x0004) >> 2, 2);

    printStr(", ");

    printStr("RB1: ");
    printInt((PORTB & 0x0002) >> 1, 2);

    printStr(", ");

    printStr("RB0: ");
    printInt(PORTB & 0x0001, 2);

    printStr("   potenciometro: ");
    printInt(v, 10);

    printStr("\r");
}

void _int_(27) is_isr(void)
{
    v = ADC1BUF0;
    AD1CON1bits.ASAM = 1; // start conversion
    IFS1bits.AD1IF = 0;   // flag
}
void sendToDisplay(int value)
{
    TRISB = TRISB & 0x00FF;
    LATB = LATB & 0x00FF;
    TRISD = TRISD & 0xFF9F;

    static unsigned char display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    static int flag = 0;

    unsigned int low = display7Scodes[value & 0x0F] << 8;
    unsigned int high = display7Scodes[value >> 4] << 8;

    if (flag == 0)
    {
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;
        LATB = LATB | low;
    }
    else
    {
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        LATB = LATB | high;
    }
    flag = !flag;
}
int to_bcd(int value)
{
    return ((value / 10) << 4) + value % 10;
}
void _int_(8) isr_T2(void){

    sendToDisplay(to_bcd(freq));
    IFS0bits.T2IF = 0; // reset interrupt flag 
}

int main(void)
{
    EnableInterrupts();
    // configure switches
    TRISB = TRISB | 0x000F;

    // configure AD
    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 1 - 1; // x - 1 conversoes consecutivas(defaul 1)
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    AD1CON1bits.ASAM = 1; // start conversion
    // configure interruptions
    IPC6bits.AD1IP = 2; // configurar prioridade
    IFS1bits.AD1IF = 0; // flag
    IEC1bits.AD1IE = 1; // enable

    //Configurar TIMER2
    T2CONbits.TCKPS = 2; // 
    PR2 = 49999;
    TMR2 = 0;
    T2CONbits.TON = 1; 
    //interrupcoes timerT2 
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;


    while (1)
    {
        freq = (1 + v/255);
        printRBs();
        delay(1000 / freq);
    }

    return 1;
}
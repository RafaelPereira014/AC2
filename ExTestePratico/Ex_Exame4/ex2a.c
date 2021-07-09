#include <detpic32.h>

volatile int v;

void configAll(void);
void sendToDisplay(int value);
void delay(int ms);
int to_bcd(int value);
int getADCvalue(void);

int main(void){
    configAll();
    while(1){
        delay(2.5);
        AD1CON1bits.ASAM = 1;
        while(IFS1bits.AD1IF == 0);
        v = getADCvalue();
        sendToDisplay(to_bcd(v));
        IFS1bits.AD1IF = 0;
		IFS0bits.T2IF = 0;

    }
    return 1;

}

void configAll(void){
    //configurar adc 
    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 1 - 1;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    AD1CON1bits.ASAM = 1;

    //configurar timer2
    T2CONbits.TCKPS = 3;
    PR2 = 312499;
    TMR2 = 0;
    T2CONbits.TON = 1;

    //interrupcoes timer2
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;

    //config PWM
    OC2CONbits.OCM = 7; //PWM mode on OC2
    OC3CONbits.OCTSEL = 0;
    OC2RS = 78125;
    OC2CONbits.ON = 1;

    EnableInterrupts();
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
int to_bcd(int value)
{
    return ((value / 10) << 4) + value % 10;
}

int getADCvalue(void)
{
    while (IFS1bits.AD1IF == 0)
    {
    }
    return ADC1BUF0;
}

void _int_(8) irc_timer2(void){

    sendToDisplay(to_bcd(v));
    IFS0bits.T2IF = 0;
}


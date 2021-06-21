#include <detpic32.h>


volatile int v;

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

int getADCvalue(void)
{
    while (IFS1bits.AD1IF == 0)
    {
    }
    return ADC1BUF0;
}
int configAdc(void){
    EnableInterrupts();
    // configurar adc
    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 1 - 1;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    AD1CON1bits.ASAM = 1;

    return 1;
}

int main (void){
    configAdc();
    TRISE = TRISE & 0xFFC3;
    LATE = LATE & 0xFFC3;
    while(1){
        while (IFS1bits.AD1IF ==0)
        {
        }
        v = getADCvalue();
        sendToDisplay(v);
        printInt(v,1);
        if(v == 0){
            LATEbits.LATE2 = 1;
        }
        
        printStr("\n");
        AD1CON1bits.ASAM = 1;

    }
    return 1;
}


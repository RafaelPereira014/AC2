#include <detpic32.h>

volatile int v;
void sendToDisplay(int value);
int getADC(void);
int configAll(void);

int main(void){
    EnableInterrupts();
    configAll();
    while(1){
        AD1CON1bits.ASAM =1;
        while(IFS1bits.AD1IF ==0);
        v = getADC();
        sendToDisplay(v);
    }

    return 1;

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
int getADC(void){
    while(IFS1bits.AD1IF == 0)
    {
    }
    return ADC1BUF0;
}

int configAll(void){

    //configurar ADC
    TRISBbits.TRISB4 = 1;     
    AD1PCFGbits.PCFG4= 0;     // RBx configured as analog input (AN4)  
    AD1CON1bits.SSRC = 7;     //
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;    // Sample time is 16 TAD (TAD = 100 ns)    
    AD1CON2bits.SMPI = 1-1;
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
    AD1CON1bits.ASAM =1;


    //Configurar TIMER
    T2CONbits.TCKPS = 3;  //? 
    PR2 = 49999;         
    TMR2 = 0;            
    T2CONbits.TON = 1;
    //interrupcoes t2
    IPC2bits.T2IP = 2;     
    IEC0bits.T2IE = 1;   
    IFS0bits.T2IF = 0; 

    //config PWM

    OC1CONbits.OCM = 6;  
    OC1CONbits.OCTSEL =0; 
    OC1RS = 15000;       
    OC1CONbits.ON = 1;

    return 1;
}

void _int_(8) isr_timer2(void){
    
        sendToDisplay(v);
	    IFS0bits.T2IF = 0;
}


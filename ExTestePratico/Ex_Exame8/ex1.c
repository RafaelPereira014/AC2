#include <detpic32.h>

volatile int prog = 0;

void configUart(void);
void putc(char byte2send);
void puts(char *str);
int to_bcd(int value);

int main(void){
    configUart();
    IPC8bits.U2IP = 1;
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2RXIE = 1;
    TRISB = TRISB | 0x0007;
    EnableInterrupts();
    while (prog == 0)
    {  
        
    }
    
    return 1;

}

void configUart(void){
    U2BRG = ((PBCLK + (8*57600))/(16*57600))-1;
    U2MODEbits.BRGH = 0;
    U2MODEbits.STSEL = 0;
    U2MODEbits.PDSEL = 0b10;

    U2STAbits.UTXEN = 1;
    U2STAbits.URXEN = 1;
    U2MODEbits.ON = 1;

}


void _int_(32) isr_uart2(void){
    TRISB = TRISB | 0x0007;
    PORTB = PORTB & 0x0007;
    

    if (IFS1bits.U2RXIF == 1)
    {
        while(U2STAbits.URXDA == 0);
        char val = U2RXREG;
        if(val == 'l'){
            puts("DipSwitches[0-2]:");
            putc((to_bcd(PORTB & 0x0004))+(to_bcd(PORTB & 0x0002))+(to_bcd(PORTB & 0x0001))+'0');
            puts("\n");
        }

        if(val == 's'){
            puts("RAFAEL PEREIRA 98354");
            puts(" \n");
        }

        if(val == 'q'){
            prog = 1;
        }
    }

    IFS1bits.U2RXIF = 0;


}

void putc(char byte2send){
    while(U2STAbits.UTXBF ==1);
    U2TXREG = byte2send;
}
void puts(char *str){
    while(*str!='\0'){
        putc(*str);
        str++;
    }
}
int to_bcd(int value)
{
    return ((value / 10) << 4) + value % 10;
}


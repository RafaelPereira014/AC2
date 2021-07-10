#include <detpic32.h>

volatile int prog = 0;
void configUart(void);
void putc(char byte2send);
void puts(char *str);

int main(void){
    TRISB = TRISB | 0x000F;
    PORTB = PORTB & 0x000F;
    //interrupcoes uart
    configUart();
    IPC8bits.U2IP=1;
    IFS1bits.U2RXIF=0;
    IEC1bits.U2RXIE=1;
    EnableInterrupts();
    while(prog==0){

    }

    return 0;



}

void configUart(void){
    U2BRG = ((PBCLK+(8*115200))/(16*115200))-1;
    U2MODEbits.BRGH=0;
    U2MODEbits.STSEL=0;
    U2MODEbits.PDSEL = 0b00;
    U2STAbits.UTXEN=1;
    U2STAbits.URXEN=1;
    U2MODEbits.ON=1;
}

void _int_(32) isr_uart2(){
    TRISB = TRISB | 0x000F;
    PORTB = PORTB & 0x000F;
    while(U2STAbits.URXDA == 0);
    char val = U2RXREG;
    char c = U2TXREG;

    if(val == 'l'){
        if((PORTB & 0x000F)%2 == 0){
            puts("Par");
            puts("\n");
        }else{
            puts("Impar");
            puts("\n");
        }
    }
    if(val == 's'){
        puts("RAFAEL PEREIRA ");
        puts("98354");
        puts("\r");
        puts("\n"); 
    }

    if(val == 'q'){
        prog = 1;
    }

    IFS1bits.U2RXIF=0;   

}
void putc(char byte2send){
    while(U2STAbits.UTXBF==1);
    U2TXREG = byte2send;
}

void puts(char *str){
    while(*str != '\0'){
        putc(*str);
        str++;
    }
}
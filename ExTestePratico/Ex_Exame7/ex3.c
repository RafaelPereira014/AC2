#include <detpic32.h>

volatile int v;
volatile int program = 0;

void putc(char byte2send)
{
    while (U2STAbits.UTXBF == 1);   // Wait until the Transmister Buffer is Full
    U2TXREG = byte2send;
}
char getc(void){
    char leitura;
    if(U2STAbits.OERR == 1){
        U2STAbits.OERR = 0;
        return 0;
    }

    while(U2STAbits.URXDA == 0);

    if (U2STAbits.FERR == 1 || U2STAbits.PERR){
        leitura = U2RXREG;
        return 0;
    }else{
        return U2RXREG;
    } 
}

int configUART(unsigned int baud,char parity,unsigned int stopbit){
    
    //configurar UART2
    U2BRG = ((PBCLK + (8*baud))/(16*baud))-1;
    U2MODEbits.BRGH = 0;
    U2MODEbits.STSEL = 0b00; //parity
    U2MODEbits.PDSEL = 1-1; //stopbits

    //config receiver & transmitter
    U2STAbits.UTXEN = 1;
    U2STAbits.URXEN = 1;

    //interrupcoes UART

     U2STAbits.URXISEL = 0; //select interrupt
    IPC8bits.U2IP = 1; //uart2 priority
    IEC1bits.U2RXIE = 1; //u2rx interrupt enable
    IFS1bits.U2RXIF = 0; //interrupt flag
return 1;
}

void _int_(32) isr_UART2(void){
    char c;
    TRISB = TRISB & 0x000F;
    PORTB = PORTB & 0x000F;
    if(IFS1bits.U2RXIF == 1){
        c = getc();
        if(c == 's'){
            printStr("Valor lido dos switches: ");
            printInt(PORTB &  0x000F,1);
            printStr("\n");
        }
        if(c == 'p'){
            putc('O');
            printStr("\n");
        }
        if(c == 't'){
            printStr("Programa terminou.");
            program = 1;
        }
    }
    IFS1bits.U2RXIF = 0;
}

int main(void){
    configUART(115200,'N',1);
    EnableInterrupts();

    while (program == 0)
    {
    }

    return 1;
    
}
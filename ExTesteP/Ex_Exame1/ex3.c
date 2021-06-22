#include <detpic32.h>

volatile int program = 1;
void configUART(unsigned int baud,char parity,unsigned int stopBits){
   //confi UART2
    U2BRG = ((PBCLK + (8*baud))/(16*baud))-1;
    U2MODEbits.BRGH = 0;
    U2MODEbits.PDSEL = 0b00; // N parity
    U2MODEbits.STSEL = 0; // 

    //enable receiver and trasmitter modules
    U2STAbits.UTXEN = 1; //Transmitter
    U2STAbits.URXEN = 1; // receiver

    //interrupcoes

    U2STAbits.URXISEL = 0; //select interrupt
    IPC8bits.U2IP = 1; //uart2 priority
    IEC1bits.U2RXIE = 1; //u2rx interrupt enable
    IFS1bits.U2RXIF = 0; //interrupt flag
}
char getc(void)    {      
    char leitura;
        
     if(U2STAbits.OERR == 1){
        U2STAbits.OERR = 0;
        return 0;

     }
      while(U2STAbits.URXDA == 0); 
           
      if(U2STAbits.FERR == 1 || U2STAbits.PERR) 
      {
          leitura = U2RXREG;            
          return 0;
      } else{               
          return U2RXREG;           
      }   
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

void _int_(32) isr_UART2(void){
    TRISB = TRISB & 0x000F;
    char c;

   if (IFS1bits.U2RXIF == 1)
    {
        c = getc();
        if (c == 'l')
        {
         if((PORTB & 0x000F)%2 == 0){
             printStr("Par\n");
         }else{
             printStr("ímpar\n");
         }

        }else if (c == 's')
         {
             printStr("Nome: RAFAEL PEREIRA");
             printStr("Nmec: 98354");
             printStr("\r");
             printStr("\n");
        
        }else if( c == 'q'){
             printStr("Terminou");
             program = 0;
         }
        }
        IFS1bits.U2RXIF = 0;
}


int main (void){
    // configure switch
    TRISB = TRISB | 0x000F;
    
    configUART(115200,'N',1);
    EnableInterrupts();
    while(program == 1){
         sendToDisplay(PORTB & 0x000F);
    }
    return 1;
}
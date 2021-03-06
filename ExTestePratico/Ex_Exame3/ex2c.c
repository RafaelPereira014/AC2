#include <detpic32.h>

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

void sendToDisplay(int value)
{
    TRISB = TRISB & 0x00FF;
    LATB = LATB & 0x00FF;
    TRISD = TRISD & 0xFF9F;

    static unsigned char display7Scodes[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    static int flag = 0;

    unsigned int high = display7Scodes[value & 0x0F] << 8;
    unsigned int low = display7Scodes[value >> 4] << 8;

    if (flag == 1)
    {
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        LATB = LATB | high;
        
    }
    else
    {
       LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 0;
        LATB = LATB | low;
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
char getc(void)    {      
    char leitura;
     // If OERR == 1 then reset OERR    
     if(U2STAbits.OERR == 1){
        U2STAbits.OERR == 1;
        return 0;

     }

      // Wait while URXDA == 0 
      while(U2STAbits.URXDA == 0); 
           
      // If FERR or PERR then  
      if(U2STAbits.FERR == 1 || U2STAbits.PERR) 
      {
          leitura = U2RXREG;        //   read UxRXREG (to discard the character) and return 0    

          return 0;
      } else{            // else      


          return U2RXREG;            //   Return U2RXREG   

      }   
 }

void _int_(32) isr_UART2(void){
    TRISB = TRISB & 0x000F;
    int count = 0;
    int num;
    int soma;
    char c;

   if (IFS1bits.U2RXIF == 1)
    {
     c = getc();
        if(count < 15){
            num = U2RXREG;
            soma = num + (PORTB & 0x000F);
            sendToDisplay(soma);
            delay(20);
            count++;
        }else if(c == 'p'){
            printStr("Aqui");  
        }else if(c == 'p'){
            printStr("Acola");
        }
    }
        IFS1bits.U2RXIF = 0;
}

int main (void){
    
    TRISB = TRISB & 0x000F;
    configUART(115200,'N',1);
    EnableInterrupts();
    while(1){
    }
    return 1;
}
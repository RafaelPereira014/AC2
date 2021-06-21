
#include <detpic32.h>

void delay(int ms);
void putc(char byte2send);
void putStr(char *str);
void configUart(unsigned int baud, char parity, unsigned int stopbits);
char getc(void) ;


int main(void)
{
    configUart(115200,'N',1);
    
    while (1)
    {
        putc(getc());
        
    }
    
    return 0;
}


void configUart(unsigned int baud, char parity, unsigned int stopbits)
{
    // Configure UART2
    if (baud >= 600 || baud <= 115200)
        U2BRG = ((PBCLK + 8 * baud) / (16 * baud)) - 1;    // Aproximate to closest baudrate
    else
        U2BRG = 10;                     // DEFAULT 115200 baudrate
    
    if (parity == 'N')
        U2MODEbits.PDSEL = 0b00;        // 00 = 8-bit data, no parity
    else if (parity == 'E')
        U2MODEbits.PDSEL = 0b01;        // 01 = 8-bit data, even parity
    else if (parity == 'O')
        U2MODEbits.PDSEL = 0b10;        // 10 = 8-bit data, odd parity
    else
        U2MODEbits.PDSEL = 0b00;        // DEFAULT NO PARITY 8 BITS

    if (stopbits == 1 || stopbits == 2)  
        U2MODEbits.STSEL = stopbits - 1;// Number of stopbits
    else
        U2MODEbits.STSEL = 0;           // DEFAULT 1 STOPBIT

    U2MODEbits.BRGH = 0;                // Divide by 16
    U2STAbits.URXEN = 1;                // Enable Receiver
    U2STAbits.UTXEN = 1;                // Enable Transmitter
    U2MODEbits.ON = 1;                  // Enable UART2
}


void putc(char byte2send)
{
    while (U2STAbits.UTXBF == 1);   // Wait until the Transmister Buffer is Full
    U2TXREG = byte2send;
}

void delay(int ms)
{   
    for (; ms > 0; ms--)
    {
        resetCoreTimer();
        while (readCoreTimer() < 20000);
    }
}

void putStr(char *str)    { 
  while (*str != '\0')
    {
        putc(*str);
        str++;
    } 
}
char getc(void)    {      
    char leitura;
     // If OERR == 1 then reset OERR    
     if(U2STAbits.OERR == 1){
        U2STAbits.OERR == 1;
        return '%';

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

#include <detpic32.h>

void putc(char byte2send);

void main(void)   
 {       
    // Configure Timer T3 (2 Hz with interrupts disabled)       
    T3CONbits.TCKPS = 7;
    PR3 = 39062;
    TMR3 = 0; // reset timer T3
    T3CONbits.TON = 1; // enable timer T3
   
    while(1)      
        {          
          while(IFS0bits.T3IF == 0); // Wait until T3IF = 1     
              // Reset T3IF        
            IFS0bits.T3IF = 0; //Reset T3IF
            putChar('.');      
        }
       
     }


void putc(char byte2send)
{
  while(U1STAbits.UTXBF == 1);
  U1TXREG = byte2send;
}
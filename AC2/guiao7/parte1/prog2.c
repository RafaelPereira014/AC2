#include <detpic32.h>

void putc(char byte2send);


void main(void)    {      
     // Configure Timer T3 with interrupts enabled       
    T3CONbits.TCKPS = 7;
  PR3 = 39062;
  TMR3 = 0;
  T3CONbits.TON = 1;
  
  IPC3bits.T3IP = 2;
  IEC0bits.T3IE = 1;
  IFS0bits.T3IF = 0;
    
     EnableInterrupts();      
      while(1);
    }

  void _int_(12) isr_T3(void)      // Replace VECTOR by the timer T3                                    //  vector number   
   {       
       
       putChar('.');      
        // Reset T3 interrupt flag 
         IFS0bits.T3IF = 0;

    }   

    void putc(char byte2send)
{
  while(U1STAbits.UTXBF == 1);
  U1TXREG = byte2send;
}


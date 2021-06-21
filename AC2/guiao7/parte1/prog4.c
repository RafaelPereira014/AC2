#include <detpic32.h>


void main()      
 {        
        T1CONbits.TCKPS = 2;
    PR1 = 39062;
    TMR1 = 0;
    T1CONbits.TON = 1;

    T3CONbits.TCKPS = 10;
    PR3 = 62499;
    TMR3 = 0;
    T3CONbits.TON = 1;

    IPC1bits.T1IP = 2;
    IPC3bits.T3IP = 2;
    IEC0bits.T1IE = 1;
    IEC0bits.T3IE = 1;
    IFS0bits.T1IF = 0;
    IFS0bits.T3IF = 0; 
    EnableInterrupts();   // Global Interrupt Enable         
     while(1);       
} 

void _int_(4) isr_T1()       
{          
    // print character '1' 
    printStr("1");         
    // Reset T1IF flag 
    IFS0bits.T1IF = 0;   
}


void _int_(12) isr_T3()       
{          
    // print character '3'          
    printStr("3");         
    // Reset T3IF flag
    IFS0bits.T3IF = 0;      
}



void putc(char byte2send)
{
  while(U1STAbits.UTXBF == 1);
  U1TXREG = byte2send;
}

void putString(char *strin)
{
  int i = 0;
  while (strin[i] != '\0')
  {
    putc(strin[i++]);
  }
}
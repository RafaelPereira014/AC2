#include <detpic32.h>




void main(void)    {      
    // Timer configs
    T3CONbits.TCKPS = 2;    // KP = 3.01 -> 4 (2)
    PR3 = 49999;            // PR = (5E6/100)-1 -> 5E4-1 
    TMR3 = 0;
    T3CONbits.TON = 1;


    // PWM configs
    OC1CONbits.OCM = 6;     
    OC1CONbits.OCTSEL = 1;
    OC1RS = 12500;          // OC1RS = ((49999 + 1) * 25)/100
    OC1CONbits.ON = 1;
     EnableInterrupts();      
      while(1);
    }

  
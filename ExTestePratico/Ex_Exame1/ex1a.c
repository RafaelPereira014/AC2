#include <detpic32.h>

int main(void)
{
    TRISB = TRISB & 0x000F;
    TRISE = TRISE & 0xFFF0;
    while (1)
    {
        if(PORTBbits.RB3 == 1){
            LATEbits.LATE0 = 1;
        }else{
            LATEbits.LATE0 = 0;
        }
        if(PORTBbits.RB2 ==1 ){
            LATEbits.LATE1 = 1;
        }else{
            LATEbits.LATE1 = 0;
        }
        if(PORTBbits.RB1 == 1){
             LATEbits.LATE2 = 1;
        }else {
             LATEbits.LATE2 = 0;
        }
        if(PORTBbits.RB0 == 1){
             LATEbits.LATE3 = 1;
        }else{
             LATEbits.LATE3 = 0;
        }
    }
    return 1;
}
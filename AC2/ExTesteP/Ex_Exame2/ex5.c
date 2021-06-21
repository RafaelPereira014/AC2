
#include <detpic32.h>

void delay(int ms)
{
    while (ms != 0)
    {
        resetCoreTimer();
        while (readCoreTimer() < 2000)
        {
        }
        ms = ms - 1;
    }
}



int main(void)
{
    // freq contagem = 10hz
    int num = 0;
    while (num < 100)
    {
        num = 0;
        int freqCounter = 10;
        int key = inkey();
        int periodo = 1/freqCounter;

        if(key == 0 || key == 1 || key == 2 || key == 3 || key == 4){
              freqCounter = 10*(1+key);
            }
            periodo = 1/freqCounter;
            while (freqCounter++ < 100)
            {
                delay(periodo);
            }
            printInt(num, 10);
            printStr("\r");
            num += 1;
        }
         return 1;
    }

  
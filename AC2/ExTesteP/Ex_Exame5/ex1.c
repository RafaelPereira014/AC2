#include <detpic32.h>

#define BUF_SIZE     16   
 #define INDEX_MASK   (BUF_SIZE – 1)


 typedef struct    {       
     unsigned char data[BUF_SIZE];       
     unsigned int head;       
     unsigned int tail;      
      unsigned int count;    
    }circularBuffer;

volatile circularBuffer txb;    // Transmission buffer   
 volatile circularBuffer rxb;    // Reception buffer  

void comDrv_flushRx()
{
  rxb.tail = 0;
  rxb.head = 0;
  rxb.count = 0;
  int i;
  for (i = 0; i < BUF_SIZE; i++)
    rxb.data[i] = 0;
}

void comDrv_flushTx()
{
  txb.tail = 0;
  txb.head = 0;
  txb.count = 0;
  int i;
  for (i = 0; i < BUF_SIZE; i++)
    txb.data[i] = 0;
}

char comDrv_getc(char *pchar)    {      
     // Test "count" variable (reception buffer) and return FALSE   //  if it is zero    
     if(rxb.count ==0){
        DisableUart1RxInterrupt();// Begin of critical section     
        *pchar = rxb.data[rxb.head];// Copy character pointed by "head" to *pchar      
        rxb.count--;// Decrement "count" variable      
        rxb.head = (rxb.head + 1 ) & (BUF_SIZE - 1);// Increment "head" variable (mod BUF_SIZE)    
        EnableUart1RxInterrupt(); //   End of critical section      
        return 0;
     }  
         
    return 1;   
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
void configUART(unsigned int baud,char parity,unsigned int stopBits){
   //confi UART2
    U1BRG = ((PBCLK + (8*baud))/(16*baud))-1;
    U1MODEbits.BRGH = 0;
    U1MODEbits.PDSEL = 0b00; // N parity
    U1MODEbits.STSEL = 0; // 

    //enable receiver and trasmitter modules
    U1STAbits.UTXEN = 1; //Transmitter
    U1STAbits.URXEN = 1; // receiver

    //interrupcoes

    U1STAbits.URXISEL = 0; //select interrupt
    IPC8bits.U2IP = 1; //uart2 priority
    IEC1bits.U2RXIE = 1; //u2rx interrupt enable
    IFS1bits.U2RXIF = 0; //interrupt flag
}

void _int_(28) isr_uart1(void){

    char c;
    while(1){
        c = getc();
        if(c == 'e'){
            
        }
    }
}
#include<detpic32.h>

// 1
#define DisableUart1RxInterrupt() IEC0bits.U1RXIE = 0
#define EnableUart1RxInterrupt() IEC0bits.U1RXIE = 1
#define DisableUart1TxInterrupt() IEC0bits.U1TXIE = 0
#define EnableUart1TxInterrupt() IEC0bits.U1TXIE = 0

// 2
#define BUF_SIZE 8
#define INDEX_MASK (BUF_SIZE-1)

typedef struct {
    unsigned char data[BUF_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int count;
} circularBuffer;

volatile circularBuffer rxb;
volatile circularBuffer txb;

char comDrv_getc(char *pchar);
void comDrv_flushRx();
void comDrv_flushTx();
void comDrv_putc(char c);
void comDrv_puts(char *str);
 void comDrv_config(unsigned int baud, char parity, unsigned int stopbits);
 char comDrv_getc(char *pchar);

int main(void)    {       
    char buf;
     comDrv_config(115200,'N',1); // default "pterm" parameters                    
                               //  with TX and RX interrupts disabled     
       comDrv_flushRx();       
       comDrv_flushTx();      
        EnableInterrupts();   
        comDrv_puts("PIC32 UART Device-driver\n"); 
 
           while(1){
               if(comDrv_getc(&buf)){// read ch from reception buffer
                   if(buf == 'S'){
                       comDrv_puts("Uma String com pelo menos 30Caracteres.");
                       comDrv_putc(buf); // send to the transmission buffer

                   }
               }
        
           }      
           return 0;   
   }

// 3
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

// 4
void comDrv_putc(char c)
{
  while (txb.count == BUF_SIZE);
  txb.data[txb.tail] = c;
  txb.tail = (txb.tail + 1) & INDEX_MASK;
  DisableUart1TxInterrupt();
  txb.count++;
  EnableUart1TxInterrupt();
}

// 5
void comDrv_puts(char *str)
{
  while((*str) != 'c')
  {
    comDrv_putc(*str);
    (str)++;
  }
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

// 6 & 9
void _int_(24) isr_UART1()
{
  // 6
  if(IFS0bits.U1TXIF)
  {
    if (txb.count > 0)
    {
      U1TXREG = txb.data[txb.head];
      txb.head = (txb.head + 1) & INDEX_MASK;
      txb.count--;
    }
    if (txb.count == 0)
      DisableUart1TxInterrupt();
    IFS0bits.U1TXIF = 0;
  }
}


void comDrv_config(unsigned int baud, char parity, unsigned int stopbits)
{
    // Configure UART2
    if (baud >= 600 && baud <= 115200)
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

    U2STAbits.URXISEL = 0b00;           // Select interrupt only for receiver
    IPC8bits.U2IP = 1;                  // UART2 Priority
    IEC1bits.U2RXIE = 1;                // U2RX Interrupt Enable
    IFS1bits.U2RXIF = 0;                // U2RX Interrupt Flag

    U2MODEbits.ON = 1;                  // Enable UART2
}
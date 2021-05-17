#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include<math.h>

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF	 // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use internal oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // internal RC
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON // allow multiple reconfigurations
#pragma config IOL1WAY = ON // allow multiple reconfigurations

#define pi 3.14 // pi
void initSPI();
unsigned char spi_io(unsigned char o);
void writebyte(unsigned char c, unsigned short V);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 1; // set B4 as an output
    TRISAbits.TRISA4 = 0; // set A4 as an output
    LATAbits.LATA4 = 0; // turn off the LED
    
    // initial SPI
    initSPI();
  
    __builtin_enable_interrupts();
    

    unsigned short i = 0;
    unsigned short j = 0;
    unsigned short k = 0;
    while (1) {
        _CP0_SET_COUNT(0);
        
        //generate 2Hz sine wave
        float V1 = 512+512*sin(i*0.002*pi); // 0 << f << 1024
        i++;
        writebyte(0, V1); // write the Vout
        if (i == 1000) {
            i = 0;
        }
        // generate 1Hz triangle wave
        k = k+1;
        int V2;
        if (k <= 1000) {  // When 0 <= k <= 1000,V2 = j++ 
            j++;
        }
        if (k > 1000) {  // When 1000 <= k < 2000, V2 = j--
            j--;
        }
        if (k == 2000) {  // When k = 2000, k = 0
            k = 0;
        }
        V2 = j;
        writebyte(1, V2);
       
        while (_CP0_GET_COUNT() < 2400 ){ // set the frequency
            ;
        }
    }
    
}

void writebyte(unsigned char c, unsigned short V){
    unsigned short P;
    
    P = c <<15;
    P = P|(0b111 <<12);
    P = P|(V<<2);
        
    // write one byte over SPI1
    LATAbits.LATA0 = 0; //bring CS low
    spi_io(P>>8); // write the byte
    spi_io(P);
    LATAbits.LATA0 = 1; //bring CS high
}

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    TRISBbits.TRISB14 = 0;
    // Turn of analog pins
    ANSELA = 0;
    // Make A0 an output pin for CS
    TRISAbits.TRISA0 = 0;  
    LATAbits.LATA0 = 1; 
    // Set A1 as SDO1
    TRISAbits.TRISA1 = 0;
    RPA1Rbits.RPA1R = 0b0011;


    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}
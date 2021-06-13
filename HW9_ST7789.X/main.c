#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "spi.h"
#include "ST7789.h"
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

void writeUART1(const char * string);
void blink_LED(void);

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
    LATAbits.LATA4 = 0; // turn off the 
    
    U1RXRbits.U1RXR = 0b0001; // U1RX is B6
    RPB7Rbits.RPB7R = 0b0001; // U1TX is B7
 
    
    // turn on UART3 without an interrupt
    U1MODEbits.BRGH = 0; // set baud to NU32_DESIRED_BAUD
    U1BRG = ((48000000 / 115200) / 16) - 1;

    // 8 bit, no parity bit, and 1 stop bit (8N1 setup)
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;

    // configure TX & RX pins as output & input pins
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;

    // enable the uart
    U1MODEbits.ON = 1;
  
    __builtin_enable_interrupts();
    
    initSPI();
    LCD_init();
    LCD_clearScreen(BLACK);
   
    char m[100];
    int i = 0;
    while (1) {
        blink_LED();
        for (i=0; i<100; i++){
            _CP0_SET_COUNT(0);
            // draw the string
            sprintf(m, "Hello World ! %d", i);
            draw_String(28, 32, WHITE, m);// 28, 32 is the (x, y) position
            // draw the bar
            draw_Bar(28, 70, i, 100, 15);// 100 is the length, 15 is the width 
        
            float FPS = 48000000.0/ _CP0_GET_COUNT();
            sprintf(m, "FPS = %.2f", FPS);
            draw_String(28, 120, WHITE, m); // write FPS on screen
            
            if (PORTBbits.RB4 == 0)
            {
                break;
            }
        }
    }
            

        // remember the core timer runs at half the sysclk
}


void blink_LED(void){
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < 6000000){
        LATAbits.LATA4 = 1; // turn on the LED
    }
    while (_CP0_GET_COUNT() > 6000000 && _CP0_GET_COUNT() < 12000000 ){
        LATAbits.LATA4 = 0; // turn off the LED
    }
    while (_CP0_GET_COUNT() == 12000000){
        _CP0_SET_COUNT(0);
    }

}
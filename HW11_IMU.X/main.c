#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "i2c_master_noint.h"
#include "spi.h"
#include "ST7789.h"
#include "imu.h"
// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

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
#pragma config USERID = 36 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

void delay(void);
void blink_LED(void);

//unsigned char Wadd=0b01000000;
//unsigned char Radd=0b01000001;

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
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    TRISBbits.TRISB4 = 1;
    // set up SPI, LCD, I2C, IMU
    initSPI();
    LCD_init();
    i2c_master_setup();
    imu_setup();
    
    __builtin_enable_interrupts();
    
    LCD_clearScreen(BLACK);
    char m[100];
    signed short data[7];
    while (1) {
        blink_LED(); // Heartbeat LED
        _CP0_SET_COUNT(0);
        
        read_Multiple(0x20, data, 14);    
        if (PORTBbits.RB4 == 0){
            LCD_clearScreen(BLACK);
            // temperature data
            float temp = (data[0]+11800)/524.28;
            sprintf(m,"Temperature is %.2f .",temp);
            draw_String(28,40,RED,m);
            // velocity data
            sprintf(m, "Velocity is %d %d %d ", data[1],data[2],data[3]);
            draw_String(28,48,GREEN,m);
            // Acceleration data
            sprintf(m, "Acceleration is %d %d %d ", data[4],data[5],data[6]);
            draw_String(28,56,YELLOW,m);
        }
        else
        {  
            LCD_clearScreen(BLACK);
            draw_X(data[5]);
            draw_Y(data[4]);
        }
        float FPS = 24000000.0/ _CP0_GET_COUNT();
        sprintf(m, "FPS = %.2f", FPS);                                                                          
        draw_String(28, 150, WHITE, m); // write FPS on screen
    }
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

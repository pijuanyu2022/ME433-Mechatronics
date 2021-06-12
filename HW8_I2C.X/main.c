#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "i2c_master_noint.h"
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

void blink_LED(void);
void i2c_init(void);
void set_Pin(unsigned char register, unsigned char value);
unsigned char read_Pin(unsigned char register);

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
    
    __builtin_enable_interrupts();
    i2c_init();
    
    while (1) {
        blink_LED();
        if (read_Pin(0x13)==0){
            set_Pin(0x14, 0xFF);
            }
        else
            set_Pin(0x14, 0x00);
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

void i2c_init(void){
    i2c_master_setup();
    // set all the A pins output
    set_Pin(0x00, 0x00); // the first 0x00 is the register for IODIRA
                        // the second 0x00 is to set IODIRA = 0x00 to set A pins output
    // set all the B pins input
    set_Pin(0x01, 0xFF); // the 0x01 is the register for IODIRB
                        // the second 0xFF is to set IODIRB = 0xFF to set B pins input
    set_Pin(0x14, 0x00);
}
void set_Pin(unsigned char register_value, unsigned char value){
    i2c_master_start();
    i2c_master_send(0b01000000);  // send address for writing
    i2c_master_send(register_value);    // send the register
    i2c_master_send(value);       // send the data value
    i2c_master_stop();
}

unsigned char read_Pin(unsigned char register_value){
    i2c_master_start();
    i2c_master_send(0b01000000);  // send address for writing
    i2c_master_send(register_value);    // send the register
    i2c_master_restart();
    i2c_master_send(0b01000001);  // send address for reading
    unsigned char A = i2c_master_recv();  // received message from chip
    i2c_master_ack(1);            // acknowledge with a 1
    i2c_master_stop();    
    return A;
}
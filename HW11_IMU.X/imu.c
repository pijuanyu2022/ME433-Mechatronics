#include "imu.h"
#include <xc.h>
#include "i2c_master_noint.h"
#include "ST7789.h"
void imu_setup(){
    int who_am_i;
    who_am_i = read_Pin(0x0f);
    if (who_am_i != 0b01101001){
        while(1){
        ;}
    }
    //initial IMU_CTRL1_XL
    set_Pin(CTRL1_XL,0b10000010); //1.66kHz sample rate 2g sensitivity 100Hz filter
    //initial IMU_CTRL2_G
    set_Pin(CTRL2_G,0b10001000); // 1.66kHz 1000 dps sensitivity
    //initial IMU_CTRL3_C
    set_Pin(CTRL3_C,0b00000100); 
}

void draw_X(int length){
    length=-length*130/16300;
    int i=0, j=0, k=1;
    for(i=0; i<abs(length); i++){
        for(j=0; j<12; j++){
            if (length<0){
                k = -1;
            }
            LCD_drawPixel(120+i*k, 118+j, YELLOW);
            LCD_drawPixel(120+length, 118+j, BLUE);
        }
    }
}

void draw_Y(int length){
    length=length*130/16300;
    int i=0, j=0, k=1;
    for(i=0; i<abs(length); i++){
        for(j=0; j<12; j++){
            if (length<0){
                k = -1;
            }
            LCD_drawPixel(118+j,120+i*k, GREEN);
            LCD_drawPixel(118+j,120+length, BLUE);
        }
    }
}

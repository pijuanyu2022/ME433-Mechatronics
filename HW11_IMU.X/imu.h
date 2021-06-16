#ifndef IMU_H__
#define IMU_H__
#include <xc.h>

#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12

void imu_setup(); // set up IMU
void draw_X(int length); // draw x bar
void draw_Y(int length); // draw y bar
#endif

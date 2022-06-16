#pragma once
#ifndef __OLED
#define __OLED

void  init_oled();
void update_temperature(int temp);
void update_frequency( int freq);
void update_duty(int mode);
void update_mode(int mode);

#endif
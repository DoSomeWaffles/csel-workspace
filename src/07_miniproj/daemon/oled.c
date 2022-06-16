#include "oled.h"
#include "../oled/ssd1306.h"
#include <stdio.h>

#define TEMP_FORMAT "Temp: %3d'C"
#define FREQ_FORMAT "Freq: %3dHz"
#define FREQ_FORMAT "Freq: %3dHz"
#define BUFFER_SIZE 30
#define AUTO_MODE 0
#define MANUAL_MODE 1

void init_oled()
{
    ssd1306_init();
    ssd1306_set_position(0, 0);
    ssd1306_puts("CSEL1 - SP.22");
    ssd1306_set_position(0, 1);
    ssd1306_puts("  Miniproj - SW");
    ssd1306_set_position(0, 2);
    ssd1306_puts("--------------");
    update_temperature(20);
    update_frequency(1);
    update_duty(50);
    update_mode(1);
}

void update_temperature(int temp)
{
    ssd1306_set_position(0, 3);
    char buf[BUFFER_SIZE] = "";
    snprintf(buf, BUFFER_SIZE, TEMP_FORMAT, temp);
    ssd1306_puts(buf);
}

void update_frequency(int freq)
{
    ssd1306_set_position(0, 4);
    char buf[BUFFER_SIZE] = "";
    snprintf(buf, BUFFER_SIZE, FREQ_FORMAT, freq);
    ssd1306_puts(buf);
}

void update_duty(int duty)
{
    ssd1306_set_position(0, 5);
    char buf[BUFFER_SIZE] = "";
    snprintf(buf, BUFFER_SIZE, FREQ_FORMAT, duty);
    ssd1306_puts(buf);
}

void update_mode(int mode)
{
    ssd1306_set_position(0, 6);
    switch (mode){
        case AUTO_MODE:
            ssd1306_puts("Mode auto");
            break;
        case MANUAL_MODE:
            ssd1306_puts("Mode manual");
            break;
        default:
            break;

    }
}

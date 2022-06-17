#include "../oled/ssd1306.h"
#include <stdio.h>
#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>
#include<grp.h>
#include<signal.h>
#include<stdint.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include <sys/epoll.h>
#include <string.h>

#define TEMP_FORMAT "Temp: %3d'C"
#define FREQ_FORMAT "Freq: %3dHz"
#define FREQ_FORMAT "Freq: %3dHz"
#define BUFFER_SIZE 30
#define AUTO_MODE 0
#define MANUAL_MODE 1

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_LED "/sys/class/gpio/gpio362"
#define LED_STATUS "10"
#define LED_POWER "362"
#define K1_PIN "0"
#define K2_PIN "2"
#define K3_PIN "3"
#define GPIO_K1 "/sys/class/gpio/gpio0"
#define GPIO_K2 "/sys/class/gpio/gpio2"
#define GPIO_K3 "/sys/class/gpio/gpio3"

#define FAN_DEVICE "/sys/class/fan_controller_class/fan_controller_device0"
#define TEMP_ATTR "/sys/class/thermal/thermal_zone0/temp"

#define PIN_NUMBER 3

static int open_led()
{
    // unexport pin out of sysfs (reinitialization)
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, LED_POWER, strlen(LED_POWER));
    close(f);

    // export pin to sysfs
    f = open(GPIO_EXPORT, O_WRONLY);
    write(f, LED_POWER, strlen(LED_POWER));
    close(f);

    // config pin
    f = open(GPIO_LED "/direction", O_WRONLY);
    write(f, "out", 3);
    close(f);

    // open gpio value attribute
    f = open(GPIO_LED "/value", O_RDWR);
    return f;
}

static void reset_gpios()
{
    // unexport pin out of sysfs (reinitialization)
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, LED_POWER, strlen(LED_POWER));
    write(f, K1_PIN, strlen(K1_PIN));
    write(f, K2_PIN, strlen(K2_PIN));
    write(f, K3_PIN, strlen(K3_PIN));
    close(f);
}

static void init_buttons()
{
    int f = open(GPIO_EXPORT, O_WRONLY);
    write(f, K1_PIN, strlen(K1_PIN));
    write(f, K2_PIN, strlen(K2_PIN));
    write(f, K3_PIN, strlen(K3_PIN));

    close(f);

    f = open(GPIO_K1 "/direction", O_WRONLY);
    write(f, "in", 2);
    close(f);

    f = open(GPIO_K2 "/direction", O_WRONLY);
    write(f, "in", 2);
    close(f);

    f = open(GPIO_K3 "/direction", O_WRONLY);
    write(f, "in", 2);
    close(f);
    // set modes
    f = open(GPIO_K1 "/edge", O_WRONLY);
    write(f, "falling", 7);
    close(f);
    f = open(GPIO_K2 "/edge", O_WRONLY);
    write(f, "falling", 7);
    close(f);
    f = open(GPIO_K3 "/edge", O_WRONLY);
    write(f, "falling", 7);
    close(f);
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
    update_mode(AUTO_MODE);
}

int main(int argc, int argv){
    //nerd stuff do it after
    /**
    pid_t pid = fork();
    if(pid == -1 ){
        syslog(LOG_ERR,"Erreur pendant le fork");
        exit(1);
    }else if(pid !=0){
        exit(1);
    }*/
    init_oled();
    char dummybuffirq[20];
    reset_gpios();
    init_buttons();
    int led_fd = open_led();
    int len;

    int k1_fd = open(GPIO_K1 "/value", O_RDWR);
    len = pread(k1_fd,dummybuffirq,20,0);
    int k2_fd = open(GPIO_K2 "/value", O_RDWR);
    len = pread(k2_fd,dummybuffirq,20,0);
    int k3_fd = open(GPIO_K3 "/value", O_RDWR);
    len = pread(k3_fd,dummybuffirq,20,0);

    int frequency_fd = open(FAN_DEVICE "/frequency",O_RDWR);
    int mode_fd = open(FAN_DEVICE "/mode",O_RDWR);
    int temp_fd = open(TEMP_ATTR,O_RDONLY);
    int epfd = epoll_create1(0);
    
    if(epfd<0)return epfd;
    struct epoll_event k1_event={
        .events = EPOLLERR,
        .data.fd = k1_fd,
    };
    
    struct epoll_event k2_event={
        .events = EPOLLERR,
        .data.fd = k2_fd,
    };
    struct epoll_event k3_event={
        .events = EPOLLERR,
        .data.fd = k3_fd,
    };
    struct epoll_event frequency_event={//trigger view update
        .events = EPOLLIN,
        .data.fd = frequency_fd,
    };
    struct epoll_event mode_event={//trigger led_pw on/off toggle
        .events = EPOLLIN,
        .data.fd = mode_fd,
    };
    struct epoll_event temp_event={//trigger view update
        .events = EPOLLIN,
        .data.fd = temp_fd,
    };
    char buff[20]="";
    int freq=0;
    int temp=0;
    int mode=0;
    pread(mode_fd,buff,20,0);   
    sscanf(buff,"%d",&mode);
    if(mode>0){//manual
        epoll_ctl(epfd,EPOLL_CTL_ADD,k1_fd,&k1_event);
        epoll_ctl(epfd,EPOLL_CTL_ADD,k3_fd,&k3_event);
        update_mode(MANUAL_MODE);
    }else{
        update_mode(AUTO_MODE);
    }
    epoll_ctl(epfd,EPOLL_CTL_ADD,k2_fd,&k2_event);
   

    epoll_ctl(epfd,EPOLL_CTL_ADD,frequency_fd,&frequency_event);
    epoll_ctl(epfd,EPOLL_CTL_ADD,mode_fd,&mode_event);
    epoll_ctl(epfd,EPOLL_CTL_ADD,temp_fd,&temp_event);
    
    struct epoll_event events[6];
    
    while(1){
        int evt_numbers = epoll_wait(epfd,events,3,-1);

        if(evt_numbers<0)return -1;
        for(int i = 0;i<evt_numbers;i++){
            if (events[i].data.fd == k1_fd){//
                len = pread(k1_fd,dummybuffirq,20,0);
                //up frequency
                //read actual hz freq
                //incr by 1 hz
                pread(frequency_fd,buff,20,0);
                sscanf(buff,"%d",&freq);
                freq++;
                sprintf(buff,"%d",freq);
                pwrite(frequency_fd,buff,sizeof(buff),0);

            }else if(events[i].data.fd == k2_fd){//rst freq
                len = pread(k2_fd,dummybuffirq,20,0);
                pread(mode_fd,buff,20,0);   
                sscanf(buff,"%d",&mode);
                if(mode<1){//auto set to manual
                    epoll_ctl(epfd,EPOLL_CTL_ADD,k1_fd,&k1_event);
                    epoll_ctl(epfd,EPOLL_CTL_ADD,k3_fd,&k3_event);
                    pwrite(mode_fd,"1",sizeof("1"),0);
                }else{//manual set to auto
                    epoll_ctl(epfd,EPOLL_CTL_DEL,k1_fd,&k1_event);
                    epoll_ctl(epfd,EPOLL_CTL_DEL,k3_fd,&k3_event);
                    pwrite(mode_fd,"0",sizeof("0"),0);
                }   
            }else if(events[i].data.fd == k3_fd){//decr freq
                len = pread(k3_fd,dummybuffirq,20,0);
                pread(frequency_fd,buff,20,0);
                sscanf(buff,"%d",&freq);
                freq--;
                if(freq<1)freq=1;//sanitize frequency
                sprintf(buff,"%d",freq);
                pwrite(frequency_fd,buff,sizeof(buff),0);
                
            }else if(events[i].data.fd == frequency_fd){//trigger view update
                pread(frequency_fd,buff,20,0);   
                sscanf(buff,"%d",&freq);
                update_frequency(freq);
            }else if(events[i].data.fd == mode_fd){//trigger pwr led toggl
                pread(mode_fd,buff,20,0);   
                sscanf(buff,"%d",&mode);
                char dummybuf[8];
                if(mode<1){//auto toggle led ON
                    pwrite(led_fd,"1",sizeof("1"),0);
                    update_mode(AUTO_MODE);
                }else{//manual toggle led OFF
                    pwrite(led_fd,"0",sizeof("0"),0);
                    update_mode(MANUAL_MODE);
                }
            }else if(events[i].data.fd == temp_fd){//trigger view update
                pread(temp_fd,buff,20,0);   
                sscanf(buff,"%d",&temp);
                temp /=1000;
                update_temperature(temp);
            }
        }
    }
}

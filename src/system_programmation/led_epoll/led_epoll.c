#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_LED "/sys/class/gpio/gpio10"
#define LED "10"

#define K1_PIN "0"
#define K2_PIN "2"
#define K3_PIN "3"
#define GPIO_K1 "/sys/class/gpio/gpio0"
#define GPIO_K2 "/sys/class/gpio/gpio2"
#define GPIO_K3 "/sys/class/gpio/gpio3"
#define PIN_NUMBER 3
#define DECREMENT_NS 100000000
#define BASE_FREQ_NS 500000000 //2HZ frequency
#define SECOND 1000000000 //1HZ frequency
#define INCREMENT_NS 100000000
int ctr=0;
int one_sec_in_millisec = 1000;
int freq = 2; // Base
// those function types will be called when
// a button is pushed.
// IN : button descriptor : nullable
typedef void (*callback_t)(char *);
// we are going to monitor the 3 buttons for input events
static int open_led()
{
    // unexport pin out of sysfs (reinitialization)
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, LED, strlen(LED));
    close(f);

    // export pin to sysfs
    f = open(GPIO_EXPORT, O_WRONLY);
    write(f, LED, strlen(LED));
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
    write(f, LED, strlen(LED));
    write(f, K1_PIN, strlen(LED));
    write(f, K2_PIN, strlen(LED));
    write(f, K3_PIN, strlen(LED));
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
void led_timer(int tfd, int led_fd){
{
    char dummybuf[8];
    if (ctr % 2 == 0)
        pwrite(led_fd, "1", sizeof("1"), 0);
    else
        pwrite(led_fd, "0", sizeof("0"), 0);
    read(tfd, dummybuf, 8);
        ctr++;
    }
}

int main(int argc, char *argv[])
{

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

    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd > 0)
    {
        char dummybuf[8];
        struct itimerspec spec =
            {
                {0, BASE_FREQ_NS}, // Set to {0, 0} if you need a one-shot timer
                {0, BASE_FREQ_NS}
            };
        timerfd_settime(tfd, 0, &spec, NULL);
    }
    int epfd = epoll_create1(0);
    if(epfd<0)return -1;
    struct epoll_event timer_event = {
        .events = EPOLLIN,
        .data.fd = tfd,
    };
    struct epoll_event k1_event={
        .events = EPOLLERR,//this is important, a gpio is not a simple EPOLLIN
        .data.fd = k1_fd,
    };
    
    struct epoll_event k2_event={
        .events = EPOLLERR,//this is important, a gpio is not a simple EPOLLIN
        .data.fd = k2_fd,
    };
    struct epoll_event k3_event={
        .events = EPOLLERR,//this is important, a gpio is not a simple EPOLLIN
        .data.fd = k3_fd,
    };

    epoll_ctl(epfd,EPOLL_CTL_ADD,tfd,&timer_event);
    epoll_ctl(epfd,EPOLL_CTL_ADD,k1_fd,&k1_event);
    epoll_ctl(epfd,EPOLL_CTL_ADD,k2_fd,&k2_event);
    epoll_ctl(epfd,EPOLL_CTL_ADD,k3_fd,&k3_event);
    struct epoll_event events[4];
    while(1){

        int evt_numbers = epoll_wait(epfd,events,4,-1);

        if(evt_numbers<0)return -2;
        for(int i = 0;i<evt_numbers;i++){
            if (events[i].data.fd == tfd){
                led_timer(tfd,led_fd);
            }else if(events[i].data.fd == k1_fd){//incr freq
                len = pread(k1_fd,dummybuffirq,20,0);
                struct itimerspec interval;
                struct timespec* newtimespec;
                timerfd_gettime(tfd,&interval);
                newtimespec = &(interval.it_interval);
                newtimespec->tv_nsec = newtimespec->tv_nsec - INCREMENT_NS;//decreasing
                if(newtimespec->tv_nsec <= 0){//
                    if(newtimespec->tv_sec==0){
                        newtimespec->tv_sec = 0;
                        newtimespec->tv_nsec = 1;
                    }else{
                        newtimespec->tv_sec = newtimespec->tv_sec--;
                    }
                }
                timerfd_settime(tfd,0,&interval,NULL);

            }else if(events[i].data.fd == k2_fd){//rst freq
                len = pread(k2_fd,dummybuffirq,20,0);
                struct itimerspec interval;
                struct timespec* newtimespec;
                timerfd_gettime(tfd,&interval);
                newtimespec = &(interval.it_interval);
                newtimespec->tv_sec =0;
                newtimespec->tv_nsec = BASE_FREQ_NS;
                timerfd_settime(tfd,0,&interval,NULL);
            }else if(events[i].data.fd == k3_fd){//decr freq
                len = pread(k3_fd,dummybuffirq,20,0);
                struct itimerspec interval;
                struct timespec* newtimespec;
                timerfd_gettime(tfd,&interval);
                newtimespec = &(interval.it_interval);
                newtimespec->tv_nsec = newtimespec->tv_nsec + INCREMENT_NS;//decreasing
                if(newtimespec->tv_nsec >= SECOND){//
                    if(newtimespec->tv_sec==0){
                        newtimespec->tv_sec++;
                        newtimespec->tv_nsec = newtimespec->tv_nsec-SECOND;
                    }
                }
                timerfd_settime(tfd,0,&interval,NULL);

            }
        }
    }

    return 1;
}

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

int main(int argc, char *argv[])
{
    reset_gpios();
    init_buttons();
    int led_fd = open_led();

    int len;
    int k1_fd = open(GPIO_K1 "/value", O_RDWR);
    int k2_fd = open(GPIO_K2 "/value", O_RDWR);
    int k3_fd = open(GPIO_K3 "/value", O_RDWR);
    if(k1_fd<0)printf("ERRRROOOOR");
    // we need to first multiplex the 3 button
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd > 0)
    {
        char dummybuf[8];
        struct itimerspec spec =
            {
                {2, 0}, // Set to {0, 0} if you need a one-shot timer
                {4, 0}
            };
        timerfd_settime(tfd, 0, &spec, NULL);
        fd_set timer_fd_set;
        fd_set except_set;
        int retval;
        FD_ZERO(&timer_fd_set);
        FD_ZERO(&except_set);

        FD_SET(0, &timer_fd_set);
        FD_SET(0, &except_set);

        FD_SET(tfd, &timer_fd_set);
        FD_SET(k1_fd, &except_set);
        FD_SET(k2_fd, &except_set);
        FD_SET(k3_fd, &except_set);

        int largest_fd_temp = (k2_fd > k3_fd) ? k2_fd :k3_fd; 
        int largest_fd_temp2 = (largest_fd_temp > k1_fd) ? largest_fd_temp :k1_fd; 

        int largest_fd_final= (largest_fd_temp2 > tfd) ? largest_fd_temp2 :tfd; 
        // FDSET add buttons
        // FDSET add buttons
        int ctr = 0;
        


        while (1)
        {
            //set buttons on exceptfds
            /**buttons have to be read*/
            retval = select(largest_fd_final + 1, &timer_fd_set, NULL, &except_set, NULL);
            printf("HELLLO");
            if (retval < 0)
            {
            }
            else if (retval == 0)
            {
            }
            else
            {
                // FD_ISSET(fd1, &fd_in))
                if (FD_ISSET(tfd, &timer_fd_set))
                {
                    if (ctr % 2 == 0)
                        pwrite(led_fd, "1", sizeof("1"), 0);
                    else
                        pwrite(led_fd, "0", sizeof("0"), 0);
                    read(tfd, dummybuf, 8);
                    ctr++;

                }
                
                if (FD_ISSET(k1_fd, &except_set))
                {
                    printf("hello 1");
                    len = read(k1_fd,dummybuf,8);
                }
                if (FD_ISSET(k2_fd, &except_set))
                {
                    printf("hello 2");
                    len = read(k2_fd,dummybuf,8);

                }
               if (FD_ISSET(k3_fd, &except_set))
                {
                    printf("hello 3");
                    len = read(k3_fd,dummybuf,8);
                    // si c'est le timer
                }
            }
        }
    }
    // then wait on
}

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


#define GPIO_EXPORT		"/sys/class/gpio/export"
#define GPIO_UNEXPORT	"/sys/class/gpio/unexport"
#define GPIO_LED		"/sys/class/gpio/gpio10"
#define LED				"10"

#define K1_PIN          "0"
#define K2_PIN          "2"
#define K3_PIN          "3"
#define GPIO_K1         "/sys/class/gpio/gpio0"
#define GPIO_K2         "/sys/class/gpio/gpio2"
#define GPIO_K3         "/sys/class/gpio/gpio3"

int one_sec_in_millisec =1000;
int freq = 2;//Base
//those function types will be called when 
//a button is pushed.
//IN : button descriptor : nullable
typedef void (*callback_t)(char*);
//we are going to monitor the 3 buttons for input events

static void reset_gpios(){
    // unexport pin out of sysfs (reinitialization)
	int f = open (GPIO_UNEXPORT, O_WRONLY);
	write (f, LED, strlen(LED));
    write (f, K1_PIN, strlen(LED));
    write (f, K2_PIN, strlen(LED));
    write (f, K3_PIN, strlen(LED));
	close (f);
}
static void init_buttons()
{
	// export pin to sysfs
	int f = open (GPIO_EXPORT, O_WRONLY);
	write (f, K1_PIN, strlen(K1_PIN));
    write (f, K2_PIN, strlen(K2_PIN));
    write (f, K3_PIN, strlen(K3_PIN));
	close (f);
	// config pin
	f = open (GPIO_K1 "/direction", O_WRONLY);
	write (f, "in", 2);
	close (f);
    f = open (GPIO_K2 "/direction", O_WRONLY);
	write (f, "in", 2);
	close (f);
    f = open (GPIO_K3 "/direction", O_WRONLY);
	write (f, "in", 2);
	close (f);
	// set modes
    f = open (GPIO_K1 "/edge", O_WRONLY);
 	write (f, "rising", 7);
    close(f);
    f = open (GPIO_K2 "/edge", O_WRONLY);
 	write (f, "rising", 7);
    close(f);
    f = open (GPIO_K3 "/edge", O_WRONLY);
 	write (f, "rising", 7);
    close(f);
}
static void init_led(){
    // export pin to sysfs
	int f = open (GPIO_EXPORT, O_WRONLY);
	write (f, LED, strlen(LED));
	close (f);

	// config pin
	f = open (GPIO_LED "/direction", O_WRONLY);
	write (f, "out", 3);
	close (f);
}

int main(int argc, char* argv[]){
    	reset_gpios();
        init_buttons();
        init_led();
        int k1_fd = open(GPIO_K1 "/value", O_RDWR);
        int k2_fd = open(GPIO_K2 "/value", O_RDWR);
        int k3_fd = open(GPIO_K3 "/value", O_RDWR);
        int led_fd = open(GPIO_LED, "/value", O_RDWR);
        int millisecs = one_sec_in_millisec/freq;//base freq = 2hz
        struct itimerspec timspec;
        bzero(&timspec, sizeof(timspec));
        timspec.it_interval.tv_sec = 0;
        timspec.it_interval.tv_nsec = millisecs * 1000000;
        timspec.it_value.tv_sec = 0;
        timspec.it_value.tv_nsec = 1;
        //we need to first multiplex the 3 button

        //then wait on

}

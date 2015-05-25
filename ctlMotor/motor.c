
#include <wiringPi.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
} while (0)
#define UP_MOTOR 0
#define DOWN_MOTOR 1
#define LEFT_MOTOR 2
#define RIGHT_MOTOR 3
#define INT_SEC 2
#define INT_MSEC 0//100
#define INT_NSEC INT_MSEC*1000000 //
#define MAX_TIMERS 4
#define MAX_MOTORS MAX_TIMERS 
#define PI_UID 1000
#define PI_GID 1000
int g_max_x;
int g_max_y;
typedef void (*timer_callback_t)(int); // Hide the ugliness
timer_t timerid[MAX_TIMERS];
#define TIME_ON  4
#define TIME_OFF 3
clock_t clocks[MAX_TIMERS];
#define MOTOR_ON   1
#define MOTOR_OFF  (!MOTOR_ON)

int	motor_stat[MAX_MOTORS];
struct sigevent sev[MAX_TIMERS];
unsigned char motor_io[MAX_TIMERS];
void * callback_timers[MAX_MOTORS];
static void
handler(int sig, siginfo_t *si, void *uc)
{
	/* Note: calling printf() from a signal handler is not
	   strictly correct, since printf() is not async-signal-safe;
	   see signal(7) */
	int timer;

	timer_callback_t tcl;
	timer = si->si_value.sival_int;
	printf("Caught signal %d\n", sig);
	tcl = callback_timers[timer];
	tcl(timer);
//	(timer_create*)callback_timers[timer](timer);
}
void defualt_callback(int motor){
	printf("// turn off gpio %d\n",motor);
	digitalWrite (motor_io[motor], LOW) ;	// On
}
static int init_timers(){
	int i;
	struct sigaction sa;
	/* Establish handler for timer signal */

	printf("Establishing handler for signal %d\n", SIG);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)
		errExit("sigaction");

	for(i=0; i<MAX_TIMERS;i++){
		sev[i].sigev_notify = SIGEV_SIGNAL;
		sev[i].sigev_signo = SIG;
		sev[i].sigev_value.sival_int = i;
		callback_timers[i] = defualt_callback;
		if (timer_create(CLOCKID, &sev[i], &timerid[i]) == -1){
			printf("timer id %d\n",i);
			errExit("timer_create");
		}


	}
	return 0;
}
static int set_timer(int timer,int sec,long nsec){
	struct itimerspec its;
	its.it_value.tv_sec = sec;
	its.it_value.tv_nsec =nsec;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec =0;
	if (timer_settime(timerid[timer], 0, &its, NULL) == -1){
		printf("timer id %d\n",timer);
		errExit("timer_settime");
	}
	return 0;

}

int init_motor(int max_x,int max_y){
	int i;
	g_max_x = max_x;
	g_max_y = max_y;	
	wiringPiSetup();
	//wiringPiSetupSys();
	
	for(i=0;i<MAX_MOTORS;i++){
		motor_io[i] = i;
		pinMode(motor_io[i], OUTPUT);
	}
	setuid(PI_UID);
	setgid(PI_GID);
	init_timers();
}
static int set_motor(int motor){
	struct itimerspec curr_vall;
	struct timespec *ts;
	long int sec_time = 0;
	//to dogpio set
	timer_gettime(timerid[motor],&curr_vall);
	ts = &(curr_vall.it_value);
	if(ts->tv_sec == (time_t)0 && ts->tv_nsec == 0){
		printf("set motor x=%d\n",motor);
		clocks[motor] = time(NULL);
		digitalWrite (motor_io[motor], HIGH) ;	// On
	}
	else{
		sec_time = (time(NULL) - clocks[motor])%(TIME_ON + TIME_OFF);
		//printf("sec_time x=%ld clock = %ld clocks_mot = %ld\n",sec_time,time(NULL), clocks[motor]);
		if(motor_stat[motor] == MOTOR_ON && sec_time >= TIME_ON){
			motor_stat[motor] = MOTOR_OFF;
			digitalWrite (motor_io[motor], LOW) ;	// Off
			
		}
		else if(motor_stat[motor] == MOTOR_OFF && sec_time < TIME_ON){
			motor_stat[motor] = MOTOR_ON;
			digitalWrite (motor_io[motor], HIGH) ;	// On
		}	
	}

	set_timer(motor,INT_SEC,INT_NSEC);
}
int set_motors(int x,int y){
	struct itimerspec curr_vall;
	if(x > g_max_x*3/4){
		set_motor(LEFT_MOTOR);
	}
	if(x < g_max_x*1/4){
		set_motor(RIGHT_MOTOR);
	}
	if(y > g_max_y*3/4){
		set_motor(UP_MOTOR);
	}
	if(y < g_max_y*1/4){
		set_motor(DOWN_MOTOR);
	}
}
int ctlMotor(char c){
	switch(c){
	case 'u':
		set_motor(UP_MOTOR);
	break;
	case 'd':
		set_motor(DOWN_MOTOR);
	break;
	case 'r':
		set_motor(RIGHT_MOTOR);
	break;
	case 'l':
		set_motor(LEFT_MOTOR);
	break;
	case 'q':
		return -1;
	break;
	defualt:
		return 1;
	break;
	}
	return 0;

}
#ifdef MOTOR_MAIN
int main(){
	char c;
	init_motor(10,10);

	while(c != 27){
		c = getchar();
		if(ctlMotor(c) < 0) break;

	}

}
#endif// MOTOR_MAIN
int exit_motors(){
	int i;
	for(i=0; i<MAX_MOTORS;i++){
		digitalWrite (motor_io[i], LOW) ;	// Off
	}
	return 0;
	
}


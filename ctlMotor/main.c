<<<<<<< HEAD
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include "motor.h"
int main(){
	char c;
	char buf[255];
	int fd,res;
	init_motor(10,10);
	fd = open("motor_pipe",O_RDONLY);
	if(fd < 0){
		perror("error opening pipe\n");
		exit(0);
	}
	
	while(c != 27){
		c = getchar();
//		res = read(fd,buf,1);
//		if(res > 0){
//			perror("error reading pipe\n");
//		}
//		c = buf[0];
		printf("char %c\n",c);
=======
#include <stdio.h>
#include "motor.h"
int main(){
	char c;
	init_motor(10,10);

	while(c != 27){
		c = getchar();
>>>>>>> 6a5f56c41d31ff2474a1964ec0f0dd36e4b46edb
		if(ctlMotor(c) < 0) break;

	}
	exit_motors();

}


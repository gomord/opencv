#include <stdio.h>
#include "motor.h"
int main(){
	char c;
	init_motor(10,10);

	while(c != 27){
		c = getchar();
		if(ctlMotor(c) < 0) break;

	}
	exit_motors();

}


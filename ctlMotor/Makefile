all:
	gcc -c -fPIC motor.c -o motor.o -lrt -lwiringPi
	ar arc motor.o -o libmotor.a 
	gcc main.c motor.o -o motor.out -lrt -lwiringPi


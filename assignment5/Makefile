
CFLAGS=-c -std=c99 -Wall -pedantic -g
EFLAGS=-Wall -pedantic
AFLAGS=-std=c99 -Wall -pedantic -g

philo: assignment5.o
	cc -o philo $(EFLAGS) assignment5.o -lm

assignment5.o: assignment5.c
	cc $(CFLAGS) assignment5.c

all:
	cc -o philo $(AFLAGS) assignment5.c -lm

clean:
	rm -f assignment5.o && rm philo && rm -R philo.dSYM

run: philo
	./philo
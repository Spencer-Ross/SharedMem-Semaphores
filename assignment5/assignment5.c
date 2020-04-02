/***********************************************************************
name: Spencer Ross
	assignment5 -- Five Dining Philosophers
description:	
	Five philosophers dine at a table. Watch as they fight to the death over
	the only chopsticks and win the chance to sit at the table to eat
	what might be their last meal. Who will win this savage feast?
note:
	I left in the grabbing and putting comments to provide clarity on what is
	happening and to show that this program works as intended. Without them,
	a few print statesments made it appear as those some philosophers were
	eating at the same time as their adjacent stick-mates.
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define NUM_PHILOS 5 //number of philosophers/chopsticks
#define MEALS 100 //number of cycles

/* Text Colors (The spice of life)*/
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//Prototypes
int randomGaussian(int mean, int stddev);
int getWait(int a, int b);
void philosopher(int i, int semLeft, int semRight);


//Create processes (philosophers), semaphores (chopsticks)
//Parent cleans everything up at the end
int main(int argc, char *argv[]){
	int status, i;
	int semID[NUM_PHILOS]; 
	struct sembuf init = {0, 1, 0};

	//make semaphore ids for chopsticks
	for(i = 0; i < NUM_PHILOS; i++) {
		semID[i] = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
		if(semID[i] < 0) {
			perror("main: semget");
			exit(1);
		}
		semop(semID[i], &init, 1);
	}

	//make philosophers and put to the table
	for(i = 1; i <= NUM_PHILOS; i++) {
		if(!fork()) {
			philosopher(i, semID[i - 1], semID[i % NUM_PHILOS]);
			exit(0);
		}
	}

	//wait for all children to end
	for(i = 1; i <= NUM_PHILOS; i++) 
		wait(&status);
	//free all semaphores
	for(i = 0; i < NUM_PHILOS; i++) {
		int ret_ctl = semctl(semID[i], 0, IPC_RMID, 0);
		if(ret_ctl < 0) {
			perror("main: semctl");
			exit(1);
		}
	}
	return 0;
}


/*
Philosopher tries to pickup a left chopstick and if it's available
it then tries to pick up the right chopstick. If it can't get both
if puts them down and waits. Once the philosopher has both, it will eat
for a random amount of time then give the sticks back and think for a 
random amount of time.
*/
void philosopher(int n, int semLeft, int semRight) {
	srand(getpid());
	int i, retL, retR;
	int eat = 0, wait = 0, think = 0;
	struct sembuf putStick = {0, 1, 0};
	struct sembuf takeStick = {0, -1, 0};

	for(i = 0; eat < MEALS; i++) {
		//THINK
		wait = getWait(11, 7); 
		think += wait;
		printf(KWHT "Philosopher %d is thinking for %d seconds (%d)\n" KNRM, n, wait, think);
		fflush(stdout); 
		sleep(wait);
		//PICKUP CHOPSTICKS
		printf(KRED "Philosopher %d reaches for left: %d, right: %d\n" KNRM, n, semLeft, semRight);
		// retL = semop(semLeft, &takeStick, 1);
		// retR = semop(semRight, &takeStick, 1);
		if((retL = semop(semLeft, &takeStick, 1)) == 0) { 
			printf(KRED "Philosopher %d has left stick: ID %d\n" KNRM, n, semLeft);
			if ((retR = semop(semRight, &takeStick, 1)) == 0) {
				printf(KRED "Philosopher %d has right stick: ID %d\n" KNRM, n, semRight);
				//EAT
				wait = getWait(9, 3);
				eat += wait;
				printf(KGRN "Philosopher %d is eating for %d seconds (%d)\n" KNRM, n, wait, eat);
				fflush(stdout);
				sleep(wait);
			} else if(retR == -1 && errno != EINTR) {
				perror("takeRight: semop");
				exit(1);
			}
		} else if(retL == -1 && errno != EINTR) {
			perror("takeLeft: semop");
			exit(1);
		} 
		

		//PUTDOWN CHOPSTICKS
		if(semop(semLeft, &putStick, 1) == -1 && errno != EINTR) { //Left Chopstick
			perror("putStick: semopLeft");
			exit(1);
		}
		printf(KRED "Philosopher %d puts left stick back: ID %d\n", n, semLeft);
		if(semop(semRight, &putStick, 1) == -1 && errno != EINTR) { //Right Chopstick
			perror("philosopher %d: semopRight");
			exit(1);
		}
		printf(KRED "Philosopher %d puts right stick back: ID %d\n", n, semRight);
	}

	printf(KNRM "Philosopher %d ate for %d seconds and", n, eat);
	printf(" thought for %d seconds, over %d cycles\n", think, i);
	return;
}

//This is a simple function to get the random wait value or 
//if the value is negative, then wait is set to 0
int getWait(int a, int b){
	int ranNum = randomGaussian(a, b);
	int ret = (ranNum > 0) ? ranNum : 0;
	return ret;
}

//Ben McCamish's random number function - ask him about it ;)
int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}

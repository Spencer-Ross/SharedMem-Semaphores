/***********************************************************************
name: Spencer Ross
	assignment5 -- 
description:	
	
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "random.h"

void handler(int nothing) {}

int main(int argc, char *argv[]){
	struct sigaction sa;
	memset(&sa, 0, NULL);
	
	return 0;
}
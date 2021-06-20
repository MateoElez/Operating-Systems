#include<stdio.h>
#include<signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>

int pid=0;

void prekidna_rutina(int sig){
   /* pošalji SIGKILL procesu 'pid'*/
   kill(pid, SIGKILL);
   exit(0);
}

int main(int argc, char *argv[]){
   pid=atoi(argv[1]);
   sigset(SIGINT, prekidna_rutina);
   int sleepVar;
   int signal;
   int sig[]={SIGUSR1, SIGUSR2, SIGHUP ,SIGTRAP , SIGINT};


   while(1){
      /* odspavaj 3-5 sekundi */
      sleepVar = 3 + (rand() % 3);
      sleep(sleepVar);
      /* slučajno odaberi jedan signal (od 4) */
      signal=sig[(rand() % 4)];
      /* pošalji odabrani signal procesu 'pid' funkcijom kill*/
      kill(pid, signal);
   }
   return 0;
}
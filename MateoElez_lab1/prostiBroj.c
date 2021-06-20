#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>


int pauza = 0;
long int broj = 1000000001;
long int zadnji = 1000000001;

void ispisi(long int zadnji){
    printf("zadnji prosti broj = %ld \n", zadnji);
}

void periodicki_ispis () {
   ispisi (zadnji);
}

void postavi_pauzu () {
   pauza = 1 - pauza;
}

void prekini () {
   ispisi (zadnji);
   exit(0);
}

int prost ( unsigned long n ) {
	unsigned long i, max;

	if ( ( n & 1 ) == 0 ) /* je li paran? */
		return 0;

	max = sqrt (n);
	for ( i = 3; i <= max; i += 2 )
		if ( ( n % i ) == 0 )
			return 0;

	return 1; /* broj je prost! */
}

int main (void) {
   //povezi_signale_s_funkcijama; // na signal SIGTERM pozovi funkciju prekini()
   sigset(SIGTERM, prekini);
   //postavi_periodicki_alarm;    // svakih 5 sekundi pozovi funkciju periodicki_ispis();
   sigset (SIGINT,postavi_pauzu);

   
   struct itimerval t;

	/* povezivanje obrade signala SIGALRM sa funkcijom "periodicki_posao" */
	sigset ( SIGALRM, periodicki_ispis );

	/* definiranje periodičkog slanja signala */
	/* prvi puta nakon: */
	t.it_value.tv_sec = 5;
	t.it_value.tv_usec = 0;
	/* nakon prvog puta, periodicki sa periodom: */
	t.it_interval.tv_sec = 5;
	t.it_interval.tv_usec = 0;

	/* pokretanje sata s pridruženim slanjem signala prema "t" */
	setitimer ( ITIMER_REAL, &t, NULL );

    while(1) {
       if(prost(broj) == 1)
            zadnji=broj;
        broj++;
        while(pauza == 1)
            pause();
   }

	return 0;
}
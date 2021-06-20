#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

//JEZGRENE STRUKTURE PODATAKA
//glonalne varijable monitor i redovi cekanja:
pthread_mutex_t monitor;
pthread_cond_t red_cekanja[5];

//KORISNICKE STRUKTURE PODATAKA
//sto radi svaki filozof?
char filozof[5]={'O','O','O','O','O'}; //'O' = razmislja

//u kakvom je "stanju" svaka vilica - u upotrebi ili ne
int vilica[5] = {1,1,1,1,1}; // vilice nisu u upotrebi - na stolu!


//Funkcija misliti() treba simulirati trošenje vremena (npr. sleep(3)).
void misliti() {
    sleep(3);
}

//Funkcija jesti(n) simulira postupak hranjenja filozofa
void jesti(n){
   /* n - redni broj filozofa */
   pthread_mutex_lock(&monitor); //uđi_u_kritični_odsječak;
      filozof[n] = 'o'; //'o' = ceka na vilice
      //filozof n ceka i lijevi (n) i desni ((n + 1) % 5) stapic
      while (vilica[n] == 0 || vilica[(n + 1) % 5] == 0)
         pthread_cond_wait(&red_cekanja[n], &monitor);
      vilica[n] = vilica[(n + 1) % 5] = 0;
      //kad dobije obe vilice, jede..
      filozof[n] = 'X'; //'X' = jede
      //ispiši_stanje(n);
      for(int i = 1; i <= 5 ; i++) {
          printf("%c", filozof[i - 1]);
      }
      printf(" (%d)", n);
      printf("\n");
   pthread_mutex_unlock(&monitor); //izađi_iz_kritičnog_odsječka

   sleep(2); // njam_njam 

   pthread_mutex_lock(&monitor); //uđi_u_kritični_odsječak;
      filozof[n] = 'O';
      //postavi vilice da su na stolu
      vilica[n] = vilica[(n + 1) % 5] = 1;
      pthread_cond_signal(&red_cekanja[(n - 1) % 5]);
      pthread_cond_signal(&red_cekanja[(n + 1) % 5]);
      //ispiši_stanje(n);
      for(int i = 1; i <= 5 ; i++) {
          printf("%c", filozof[i - 1]);
      }
      printf(" (%d)", n);
      printf("\n");
   pthread_mutex_unlock(&monitor); //izađi_iz_kritičnog_odsječka
}

//dretva filozof i
void *filozof_i(int i) {
    while (1)
    {
        misliti();
        jesti(i);
    }
}

int main(void) {
    //koristimo 5 dretvi za 5 filozofa
    pthread_t thread[5];

    //imamo 1 monitor i 5 redova cekanja za svakog filozofa
    pthread_mutex_init(&monitor, NULL);
    pthread_cond_init(&red_cekanja[0], NULL);
    pthread_cond_init(&red_cekanja[1], NULL);
    pthread_cond_init(&red_cekanja[2], NULL);
    pthread_cond_init(&red_cekanja[3], NULL);
    pthread_cond_init(&red_cekanja[4], NULL);
    
    sigset(SIGINT, SIG_DFL);

    for(int i = 1 ;i <= 5;i++){
      pthread_create(&thread[i - 1],NULL,filozof_i, (i - 1));
   }

   for(int i=1; i <= 5; i++){
      pthread_join(thread[i - 1], NULL);
   }

   //na kraju samo "ugasi" monitor i redove cekanja
   pthread_mutex_destroy(&monitor);
    for(int i = 1; i <= 5; i++) {
      pthread_mutex_destroy(&red_cekanja[i - 1]);
   }
   return 0;
}

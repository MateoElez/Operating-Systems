#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>

#define N 6    /* broj razina proriteta */
 
int OZNAKA_CEKANJA[N];
int PRIORITET[N];
int TEKUCI_PRIORITET;
 
 
int sig[]={SIGUSR1, SIGUSR2, SIGHUP ,SIGTRAP , SIGINT};

void zabrani_prekidanje(){
   int i;
   for(i=0; i<5; i++)
      sighold(sig[i]);
}
void dozvoli_prekidanje(){
   int i;
   for(i=0; i<5; i++)
      sigrelse(sig[i]);
}
 
void obrada_signala(int i){
   /* obrada se simulira trošenjem vremena,
      obrada traje 5 sekundi, ispis treba biti svake sekunde */

      switch(i){
      case 1: 
        printf(" - P - - - -\n");
          for(int j=1 ; j<5 ; j++){
           printf(" - %d - - - -\n",j);
           sleep(1);
        }
         printf(" - K - - - -\n");
         break;

      case 2: 
        printf("- - P - - -\n");
          for(int j=1 ; j<5 ; j++){
           printf(" - - %d - - -\n",j);
           sleep(1);
        }
         printf("- - K - - -\n");
         break;

       case 3: 
        printf(" - - - P - -\n");
          for(int j=0 ; j<5 ; j++){
           printf("- - - %d - -\n",j);
           sleep(1);
        }
         printf(" - - - K - -\n");
         break;

        case 4: 
         printf("- - - - P -\n");
          for(int j=0 ; j<5 ; j++){
           printf("- - - - %d-\n",j);
           sleep(1);
        }
         printf("- - - - K -\n");
         break;

      case 5: 
          printf("- - - - - P\n");
          for(int j=0 ; j<5 ; j++){
           printf("- - - - - %d\n",j);
           sleep(1);
        }
         printf("- - - - - K\n");
         break;
         
   }
}
void prekidna_rutina(int sig){
   int n=-1;
   zabrani_prekidanje();

    //odredit uzrok prekida
   switch(sig){
      case SIGUSR1: 
         n=1; 
         printf("- X - - - -\n");
         break;
      case SIGUSR2: 
         n=2; 
         printf("- - X - - -\n");
         break;
      case SIGHUP:
         n=3;
         printf("- - - X - -\n");
         break;
      case SIGTRAP:
         n=4;
         printf("- - - - X -\n");
         break;
      case SIGINT:
         n=5;
         printf("- - - - - X\n");
         break;
   }

   OZNAKA_CEKANJA[n]=1; //ako hocemo da broji vise uzastopnih oznaka_cekanja[n]++
   
//    ponavljaj{
//       ...
//       dozvoli_prekidanje();
//       obrada_prekida(n);
//       zabrani_prekidanje();
//       ...
//    }

    int x;
    do{
        /* odredi signal najveceg prioriteta koji ceka na obradu */
         x = 0;
         for(int j= TEKUCI_PRIORITET + 1; j < N; j++) {
             if(OZNAKA_CEKANJA[j] != 0)
                x=j;
         }

         /* ako postoji signal koji ceka i prioritetniji je od trenutnog posla, idi u obradu */
         if(x > 0){
            OZNAKA_CEKANJA[x] = 0; //oznaka_cekanja[n]--
            PRIORITET[x] = TEKUCI_PRIORITET;
            TEKUCI_PRIORITET = x;
            dozvoli_prekidanje();
            obrada_signala(x);
            zabrani_prekidanje();
            TEKUCI_PRIORITET = PRIORITET[x];
         }

    } while(x > 0);

   dozvoli_prekidanje();
}
 
int main ( void )
{
   sigset (SIGUSR1, prekidna_rutina);
   sigset (SIGUSR2, prekidna_rutina);
   sigset (SIGHUP, prekidna_rutina);
   sigset (SIGTRAP, prekidna_rutina);
   sigset (SIGINT, prekidna_rutina);
 
   printf("Proces obrade prekida, PID=%d\n", getpid());
 
   /* troši vrijeme da se ima šta prekinuti - 10 s */

   printf("G 1 2 3 4 \n");
   printf("----------- \n");
   for(int j=0 ; j<100 ; j++){
           printf("%d - - - - -\n",j);
           sleep(1);
      }
 

   printf ("Zavrsio osnovni program\n");
 
   return 0;
}

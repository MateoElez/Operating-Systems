#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

// N i M...
int brojDretvi, brojIteracija;
unsigned long int zajednickaVarijabla;

void *Dretva() {
    for(int i = 1; i < brojIteracija; i++) {
        //svak dretva povećava zajedničku varijablu za 1 u petlji M puta
        zajednickaVarijabla++;
    }
    return 0;
}

// void brisi (int sig){
//     //* oslobađanje zajedničke memorije */
//    exit(0);
// }

int main (int argc, char *argv[]){
    long *BR;
    //na pocetku je dretva NULL
    pthread_t *thread = NULL;

    if(argc < 3){
        fprintf(stderr, "Koristenje: %s brojDretvi brojIteracija\n", argv[0]);
    }

    //glavna dretva najprije postavlja početnu vrijednost vzajedniceke arijable na nulu
    zajednickaVarijabla = 0;

    //"Parametre N i M zadati kao argument iz komandne linije"
    brojDretvi = atoi (argv[1]);
	brojIteracija = atoi (argv[2]);
    //sigset(SIGINT, brisi);

    BR = malloc(brojDretvi * sizeof(long));
    thread = malloc (brojDretvi * sizeof(pthread_t));

    //...potom stvara zadani broj procesa
    for(int i=0; i < brojDretvi; i++){
        BR[i] = i;
        if( pthread_create( &thread[i], NULL, Dretva, &BR[i] ) ) {
            fprintf(stderr, "Stvaranje nove dretve nije moguce");
            exit(1);
        }
    }

    for(int j= 0; j < brojDretvi; j++){
        //cekanje zavrsetka druge dretve
        pthread_join(thread[j], NULL);
    }

    printf("ZajednickaVarijabla = %ld", zajednickaVarijabla);

    return 0;
}
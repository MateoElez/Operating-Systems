#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdatomic.h>
#include <signal.h>

atomic_int *PRAVO;
atomic_int *ZASTAVICA[2];
int segID; //ID segmenta

int *zajednickaVarijabla;
int brojProlaza;

void udi_u_kriticni_odsjecak(int i, int j) {
    *ZASTAVICA[i] = 1;
    while(*ZASTAVICA[j] != 0){
        if(*PRAVO == j){
            *ZASTAVICA[i] = 0;
            while(*PRAVO == j); //radno cekanje
            *ZASTAVICA[i] = 1;
        }
    }
}

void izađi_iz_kritičnog_odsječka(int i, int j){
    *PRAVO = j;
    *ZASTAVICA[i] = 0;
}

void Proces(int i){
    //petlja za prolaze..
    for(int j = 0; j < brojProlaza; j++) {
        udi_u_kriticni_odsjecak(i, 1 - i);
        *zajednickaVarijabla +=1;
        izađi_iz_kritičnog_odsječka(i, 1 - i);
    }
}

void brisi(int signal){
    (void) shmdt ((int *)zajednickaVarijabla);
    (void) shmctl (segID ,IPC_RMID,NULL);
    exit(0);
}

int main (int argc, char *argv[]) {

    if (argc != 2) {
		printf("incorrect number of elements ERROR\n");
		exit(0);
	}

    brojProlaza = atoi (argv[1]);  
    sigset(SIGINT, brisi);

    segID = shmget(IPC_PRIVATE, sizeof(int) * 4, 0600);

    //ako je ID -1, zavrsi proces
    if(segID == -1){
        exit(1); //nema zajednicke memorije...
    }

    //vraca kazaljku na zajednicki prostor
    zajednickaVarijabla = (int *) shmat (segID, NULL, 0);
    *zajednickaVarijabla = 0;

    PRAVO = (atomic_int *) (zajednickaVarijabla + 1);
    *PRAVO = 0;

    ZASTAVICA[0] = (atomic_int *) (PRAVO + 1);
    ZASTAVICA[1] = (atomic_int *) (PRAVO + 2);

    *ZASTAVICA[0] = 0;
    *ZASTAVICA[1] = 0;

 
    for(int i = 0; i < 2; i++){
        if(fork() == 0) {
            Proces(i);
            exit(0);
        }
    }

    for(int i = 0; i < 2; i++){
        //cekam bilo koje dijete...
        wait(NULL);
    }


    printf("zajednickaVarijabla = %d \n",*zajednickaVarijabla);
	brisi(0);

    return 0;
}
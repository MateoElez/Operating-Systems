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

sem_t *djedbozicnjak;
sem_t *K;
sem_t *sobovi;
sem_t *konzultacije;

int ID; //identifikacijski broj segmenta

//Varijable:
int *br_sobova; // broj sobova pred vratima
int *br_patuljaka ; //broj patuljaka pred vratima

void Djed_Bozicnjak() {
    printf("Stvorili smo Djeda Bozicnjaka. \n");
	while(1) {
		sem_wait(djedbozicnjak);
		sem_wait(K);
		if (*br_sobova == 10 && *br_patuljaka > 0) {
			sem_post(K);
            
            printf("Ispred ulaza je 10 sobova i minimalno 1 patuljak i oni cekaju \n");
            printf("Djed Bozicnjak ukrca poklone i raznosi ih. \n");
			sleep(2); //ukrcaj_poklone_i_raznosi 
            printf("Djed Bozicnjak salje sobove na odmor. \n");
			sem_wait(K);

            //povećaj semafor za 10
            for(int i = 0; i < 10; i++) {
                sem_post(sobovi); 
            }
			*br_sobova = 0;
		}
		if (*br_sobova == 10 && *br_patuljaka == 0) {
			sem_post(K);
            printf("Ispred ulaza je 10 sobova i nema patuljaka pa se ne ranose poklomni. \n");
            printf("Djed Bozicnjak hrani sobove. \n");
			sleep(2); //nahrani_sobove
            //printf("Nahranio je sobove. \n");
			sem_wait(K);
		}
		//ako je samo_tri_patuljka_pred_vratima 
		while (*br_patuljaka >= 3) {
			sem_post(K);
            //printf("ispred ulaza je 3 ili vise patuljaka pa se Djed Bozicnjak budi. \n");
            //printf("Djed Bozicnjak rijesava njihov problem. \n");
			sleep(2); //riješi_njihov_problem
            //printf("Rijesio im je problem. \n");
			sem_wait(K);

            //povećaj semafor za 3
            for(int i = 0;i < 3 ; i++){
                sem_post(konzultacije); 
            }
            printf("Rijesio im je problem. \n");
			*br_patuljaka = *br_patuljaka - 3;
		}
		sem_post(K);
	}
}
void patuljak() {
	sem_wait(K);
	*br_patuljaka = *br_patuljaka + 1;
    printf("Stvorili smo Patuljka. \n");
    printf("Ispred ulaza je dosao %d. patuljak. \n", *br_patuljaka);
	if (*br_patuljaka == 3) {//ovaj je treći
        printf("ISpred ulaza su 3 patuljka. \n");
        printf("Budim Djeda. \n");
        printf("Djed Bozicnjak rijesava njihov problem. \n");
		sem_post(djedbozicnjak);
	} 
	sem_post(K);
	sem_wait(konzultacije);
}

void Sob() {
   sem_wait(K);
   *br_sobova = *br_sobova + 1;
   printf("Stvorili smo Soba. \n");
   printf("Ispred ulaza je dosao %d. sob. \n", *br_sobova);
   if(*br_sobova == 10) {
       printf("Ispred ulaza je 10 sobova. \n");
       printf("Budim Djeda. \n");
      sem_post(djedbozicnjak);
   }
   sem_post(K);
   sem_wait(sobovi);
}

void Sjeverni_pol(){
    printf("Krecemo! Stvorili smo Sjeverni Pol \n");
	while(1) {
		//čekaj_slučajno_vrijeme od 1 do 3 sekunde
        int slučajno_vrijeme = 1 + rand() % 3;
        sleep(slučajno_vrijeme);
        //ako je vjerojatnos veca od 50% i nisu se vratili svi sobovi
		if ((rand() > 0.50) && (*br_sobova != 10)) {
			//stvori dretvu Sob unutar novog procesa
            //printf("Stvorili smo Soba. \n");
            if(fork() == 0) {
                Sob();
                exit(0);
            }
		}
		if (rand() > 0.50){
            //printf("Stvorili smo Patuljka. \n");
			if(fork() == 0) {
                patuljak();
                exit(0);
            }
		}
	}
}

int main(void) {

	ID = shmget (IPC_PRIVATE, sizeof(int)*2 + sizeof(sem_t)*4, 0600);
    if(ID == -1){
        exit(1);
    }

    //"spoji" br_patuljaka i br_sobova na zajednicke varijable
    br_patuljaka = (int *) shmat(ID, NULL, 0);
    //br_sobova postavi na sljedecu adresu => +1
    br_sobova = (int *) (br_patuljaka + 1);

    //semafore takoder stavi u zajednicku memoriju... za +1 u odnosu na prethodni
    djedbozicnjak = (sem_t *) (br_sobova + 1);
    K =(sem_t *) (djedbozicnjak + 1);
    sobovi = (sem_t *) (K + 1);
    konzultacije=(sem_t *) (sobovi + 1);
    

	shmctl (ID, IPC_RMID, NULL); //moze odmah ovdje, nakon shmat, ili na kraju nakon shmdt jer IPC_RMID oznacava da segment treba izbrisati nakon sto se zadnji proces odijeli od tog segmenta (detach)
	
    //postavi pocetne vrijednosti na 0
    *br_patuljaka = 0;
    *br_sobova = 0;

    //na drugo mjesto ide 1 jer semafori nisu predvideni za sinkronizaciju dretvi istog procesa
    //a treci broj oznacava pocetno stanje semafora (p.v.)
    sem_init (djedbozicnjak, 1, 0); 
    sem_init (K, 1, 1);
    sem_init (sobovi, 1, 0);
    sem_init (konzultacije, 1, 0);

    //kind of recapp semafora
    // koristimo BSem za Djeda Bozicnjaka i Sjeverni Pol,
    // a OSem za patuljke (3) i sobove (10)

    //stvori proces u kojem ces pokrenit dretvu Sjeverni_pol
	if(fork () == 0){
        Sjeverni_pol();
        exit(0);
    }
    //stvori proces u kojem ces pokrenit dretvu Djed_Bozicnjak
    if(fork () == 0){
        Djed_Bozicnjak();
        exit(0);
    }

    for(int i = 0; i < 2; i++) {
        wait (NULL);
    }
	
    shmdt (br_patuljaka); //detachaj zajednicku memoriju za br patuljaka i time se detachaju i ostali

	sem_destroy (djedbozicnjak);
    sem_destroy (K);
    sem_destroy (sobovi);
    sem_destroy (konzultacije);
	
	return 0;
} 
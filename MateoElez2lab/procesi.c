//Proces veže segment na svoj adresni prostor

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>


//varijablama u zajednickom spremniku se nuzno pristupa pokazivacima!!!
int segID;
int *ZajednickaVarijabla;
// N i M...
int brojProcesa, brojProlaza;

void Proces(int i){
    //petlja za prolaze..
    for(int j = 0; j < brojProlaza; j++) {
        //svaki proces povećava zajedničku varijablu za 1 u petlji M puta
        //M = brojProlaza
        *ZajednickaVarijabla += 1;
    }
}

void brisi (int sig){
    //* oslobađanje zajedničke memorije */
   (void) shmdt((char *) ZajednickaVarijabla);
   (void) shmctl(segID, IPC_RMID, NULL);
   exit(0);
}

int main(int argc, char *argv[]){
    int i;

    //vraca ID segmenta potreban za shmat ili -1 ako je greska
    segID = shmget(IPC_PRIVATE, sizeof(int) * sizeof(int), 0600);

    //ako je ID -1, zavrsi proces
    if(segID == -1){
        exit(1);
    }

    //"Parametre N i M zadati kao argument iz komandne linije"
    brojProcesa = atoi(argv[1]);
    brojProlaza = atoi(argv[2]);
    sigset(SIGINT, brisi);


    // kao argument dajemo NULL da jezgra sama odabere adresu...
    //vraca kazaljku na zajednicki prostor
    ZajednickaVarijabla = (int *) shmat (segID, NULL, 0);
    //glavni proces najprije postavlja početnu vrijednost vzajedniceke arijable na nulu
    *ZajednickaVarijabla = 0;

    //...potom stvara zadani broj procesa
    for(i = 0; i < brojProcesa; i++){
        if(fork() == 0) {
            Proces(i);
            exit(0);
        }
    }
    for(i = 0; i < brojProcesa; i++){
        //cekam bilo koje dijete...
        wait(NULL);
    }

    //...te kada sve dretve/procesi završe ispisuje konačnu vrijednost zaj. varijabe...
    printf("zajednickaVarijabla = %d \n", *ZajednickaVarijabla);

    //otpustanje segmenta...
    shmdt((char *)ZajednickaVarijabla);

    //unistavanje segmenta
    shmctl(segID, IPC_RMID, NULL) ;
}
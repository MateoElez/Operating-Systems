#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>


int velicinaPolja;
int brojZahtjeva;

void ispis(char polje[]) {
    //prvi red ispisa - pozicije...
    for(int i = 0; i < velicinaPolja; i++) {
        int iNew = i % 10;
        printf("%d", iNew);
    }
    printf("\n");
    //drugi red ispisa - sto je u memoriji
    for(int  i = 0; i < velicinaPolja; i++) {
        if(polje[i] == '-')
            printf("-");
        else printf("%c", polje[i] + '0');
    }
    printf("\n");
}

void generirajZahtjev(char polje[], int brojZahtjeva) {
    int slucajniBroj = rand() % 10 + 1; //generiraj slucajni broj od 1 do 10
    printf("Novi zahtjev %d za %d spremnička mjesta\n", brojZahtjeva, slucajniBroj);
    int praznogMjesta = 0;
    int minVelicina = 1000000;
    int minID = -1;
    int pocetak = -1;
    //kad generiramo zahtjev imamo 3 slucaja:
    // (1) ide po redu, opce nema rupa
    // (2) ima rupa, on bira prvu po redu najmanje velicine i upada tu
    // (3) nema dovoljno mjesta

    int znakKrajaPolja = 0;
    int kraj = -1;
   while (polje[znakKrajaPolja] != '\0')
    {      
        praznogMjesta = 0;
        //printf("USa san");
        for(int i = kraj + 1; i < velicinaPolja; i++) {
            //ako je prazno, zapamti prvog
            if(polje[i] == '-' && praznogMjesta == 0){
                praznogMjesta++;
                pocetak = i;
                //printf("POCETAK: %d, ", pocetak);
                kraj = i;
            }
            //ako je prazni, ali nije prvi prazni...
            else if(polje[i] == '-' && praznogMjesta != 0){
                praznogMjesta++;
                kraj = i;
                //printf("Kraj trenutni: %d\n", kraj);
            }
            //ako nije prazno i velicina slucajno generirana je manja od trenutne velicine praznogMjesta
            if((polje[i] != '-' || polje[i + 1] == '\0') && slucajniBroj <= praznogMjesta) {
                //printf("USA SAN U ELSE IF AJDEEEEE\n");
                if(praznogMjesta < minVelicina) {
                    minVelicina = praznogMjesta;
                    minID = pocetak;
                }
            }
            if(polje[i] != '-') {
                kraj = i;
                break;
            }
            
            //i na kraju ako to mjesto nije prazno, a velicina je u redu tj. stane .. onda resetiraj varijable
            // else if(polje[i] != '-' && slucajniBroj >= praznogMjesta){
            //     pocetak = -1;
            //     praznogMjesta = 0;
            // } 

            if(polje[i + 1] == '\0') {
                znakKrajaPolja = i + 1;
                break;
            }
        }  
        //printf("KRAJ: %d\n", kraj);      
    }
    // printf("praznog mista: %d\n", praznogMjesta);
    // printf("min velicina: %d\n", minVelicina);
    // printf("min ID: %d\n", minID);
    // printf("POCETAK: %d\n", pocetak);
    
    
    //triba provjerit ima li opce mjesta za toliko brojeva...
    if(slucajniBroj <= praznogMjesta && minID != -1) {
        for(int i = minID; i < (minID + slucajniBroj); i++) {
            polje[i] = brojZahtjeva;
        }
        ispis(polje);
    } else {
        printf("Nema dovoljno mjesta...\n");
        exit(1);
    }

}

void oslobodiZahtjevFunction(char polje[], int oslobodiZahtjev) {
    for(int i = 0;i < velicinaPolja; i++) {
        if(polje[i] == oslobodiZahtjev) {
            polje[i] = '-';
        }
    }
    ispis(polje);
}

int main(int argc, char *argv[]){

    velicinaPolja = atoi(argv[1]);
    char polje[velicinaPolja];
    //pocetak - stavi na svako misto crticu kao znak praznog mjesta
    for(int i = 0 ; i < velicinaPolja ; i++) {
        polje[i] = '-';
    }    
    polje[velicinaPolja + 1] = '\0';
    ispis(polje);

    while(1) {
        char uneseniZnak;
        scanf("%c", &uneseniZnak);

        if(uneseniZnak == 'z' || uneseniZnak == 'Z') {
            brojZahtjeva++;
            generirajZahtjev(polje, brojZahtjeva);
        }
        if(uneseniZnak == 'o' || uneseniZnak == 'O') {
            printf("Koji zahtjev treba osloboditi?\n");
            int oslobodiZahtjev;
            scanf("%d", &oslobodiZahtjev);
            printf("Oslobađa se zahtjev %d.\n", oslobodiZahtjev);
            oslobodiZahtjevFunction(polje, oslobodiZahtjev);
        }
        if(uneseniZnak == 'g' || uneseniZnak == 'G') {
            printf("Garbage Collecting - zaustavljanje programa i preslagivanje spremnika...");
            int var;
            for(int i=0 ;i < velicinaPolja; i++) {
                for (int j = i; j < velicinaPolja; j++)
                {
                    if (polje[i] == '-' && polje[j] != '-')             
                    {
                        var = polje[i];
                        polje[i] = polje[j];
                        polje[j] = var;
                    }
                }
            }
            ispis(polje);
            //printf("Prosa san fju");
        }
        //else printf("Nepostojeci znak unesen!!");
    }
    return 0;
}

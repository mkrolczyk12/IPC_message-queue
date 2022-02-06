#include "announcement.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

int queue_id;    // zmienna globalna do przechowywania identyfikatora kolejki

void signal_handler(int sig);

int main() {
        key_t key;
        announ announ_received;         // struktura do przechowywania otrzymanego komunikatu

        key=ftok(".",'M');
        if(key == -1) {
                printf("serwer: Blad tworzenia klucza\n");
                exit(1);
        }

        queue_id = msgget(key, IPC_CREAT|0666);    // uzyskanie dostepu do kolejki
        if(queue_id == -1) {
                perror("serwer: Blad uzyskania dostepu do kolejki komunikatow\n");
                exit(1);
        } else printf("\nserwer: Uzyskano dostep do kolejki o ID '%d'\n", queue_id);
        
        signal(SIGINT, signal_handler);     // instalacja nowej obslugi dla sygnalu SIGINT (ctrl + c)
        printf("INFO: aby zakonczyc prace serwera nacisnij ^C\n");

        while(1) {
                printf("\nserwer: Oczekiwanie na komunikaty od klientow...\n");

                announ_received.type = 1; // serwer jako odbiorca komunikatu

                memset(announ_received.message, 0, MESSAGE_SIZE);       // wyzerowanie tablicy
                if(msgrcv(queue_id, (announ*)&announ_received, MESSAGE_SIZE + sizeof(long), announ_received.type, 0) == -1) {  // pobieranie wiadomosci z kolejki
                        perror("serwer: Wystapil blad w trakcie pobierania komunikatu od klienta\n");
                        signal_handler(SIGINT);
                } else printf("serwer: Odebrano komunikat od klienta o procesie '%ld' zawierajacy nastepujaca tresc: '%s'\n", announ_received.sender, announ_received.message);

                int message_length = strlen(announ_received.message);

                for(int i = 0; i < message_length; i++) announ_received.message[i] = toupper(announ_received.message[i]);

                announ_received.type = announ_received.sender;

                printf("serwer: Wysylanie komunikatu do klienta z ID '%ld' o tresci: '%s'\n", announ_received.sender, announ_received.message);
                
                if(msgsnd(queue_id, (announ*)&announ_received, strlen(announ_received.message) + 1 + sizeof(long), IPC_NOWAIT) == -1) { // przepelnienie kolejki
                        perror("serwer: Wystapil blad w trakcie wysylania komunikatu");
                        signal_handler(SIGINT);
                }
        }
        return 0;
}

void signal_handler(int sig) {
        if(sig == SIGINT) {
                printf("\n\nserwer: Konczenie pracy serwera...\n");

                if(queue_id == -1) {
                        perror("serwer: Blad uzyskania dostepu do kolejki komunikatow\n");
                        exit(1);
                }

                if(msgctl(queue_id, IPC_RMID, 0) == -1) {       // usuwanie kolejki
                        perror("serwer: Blad usuwania kolejki komunikatow: ");
                        exit(1);
                } else printf("kolejka o ID '%d' zostala pomyslnie usunieta\n", queue_id);
                
                printf("\nINFO: Pomyslnie zakonczono prace serwera\n\n");
                exit(0);
        }
}
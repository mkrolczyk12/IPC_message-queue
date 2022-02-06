#include "announcement.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

void *send_announcement();
void *receive_announcement();

int i=0;
int queue_id;
char mess[MESSAGE_SIZE];        // tablica do przechowywania wprowadzonej wiadomosci
announ announ_1;        // struktura do przechowywania komunikatu do wyslania
announ announ_2;        // struktura do przechowywania komunikatu otrzymanego z kolejki

int main() {
        key_t key = ftok(".",'M');
        if(key == -1) {
                printf("klient %ld: Blad tworzenia klucza\n", getpid());
                exit(1);
        }

        queue_id = msgget(key, IPC_CREAT|0666);    // uzyskanie dostepu do kolejki
        if(queue_id == -1) {
                perror("klient: Blad uzyskania dostepu do kolejki komunikatow\n");
                exit(1);
        } else printf("klient: Uzyskano dostep do kolejki o ID '%d' \n", queue_id);
        
        pthread_t thread_1, thread_2;
        
        if(pthread_create(&thread_1, NULL, receive_announcement, NULL) == -1) {      // utworzenie watku do odbierania komunikatu
                perror("klient: Blad tworzenia watku do odbierania komunikatu\n");
                pthread_exit((void*)0);
        }

        if(pthread_create(&thread_2, NULL, send_announcement, NULL) == -1) {         // uworzenie watku do wysylania komunikatu
                perror("klient: Blad tworzenia watku do wysylania komunikatu\n");
                pthread_exit((void*)0);
        }

        if(pthread_join(thread_1, NULL) == -1) {
                perror("klient: Blad przylaczania watku zwiazanego z odbieraniem komunikatu\n");
                pthread_exit((void*)0);
        }

        if(pthread_join(thread_2, NULL) == -1) {
                perror("klient: Blad przylaczania watku zwiazanego z wysylaniem komunikatu\n");
                pthread_exit((void*)0);
        }
}

void *send_announcement() {
        while(1) {
                pid_t client_pid = getpid();
                announ_1.type = 1;      // adresat
                announ_1.sender = client_pid;
                sleep(1);
                i=0;

                printf("\nklient %ld: Podaj tekst do wyslania: ", client_pid);
                while(1) {
                        mess[i]=getchar();
                        if((mess[i] == '\n') || (i > MESSAGE_SIZE - 1)) {
                                mess[i] = '\0';
                                break;
                        }
                        i++;
                }

                if(i > MESSAGE_SIZE - 1) {
                        printf("klient %ld ERROR: Podana wiadomosc jest zbyt dluga\n", client_pid);
                        pthread_exit(*send_announcement);
                }

                strcpy(announ_1.message, mess);

                if(msgsnd(queue_id, (announ*)&announ_1, strlen(announ_1.message) + 1 + sizeof(long), IPC_NOWAIT) == -1) {	// przepelnienie kolejki
                        perror("klient: Wystapil blad w trakcie wysylania komunikatu");
                        pthread_exit(*send_announcement);
                } else printf("klient %ld: Wyslano komunikat do serwera\n", client_pid);
        }
}

void *receive_announcement() {
        while(1) {
                pid_t client_pid = getpid();
                announ_2.type = client_pid;     // adresat

                memset(announ_2.message, 0, MESSAGE_SIZE);      // wyzerowanie tablicy

                if(msgrcv(queue_id, (announ*)&announ_2, MESSAGE_SIZE + sizeof(long), announ_2.type, 0) == -1) {       // odbieranie informacji z tablicy
                        perror("\nklient: Blad w trakcie odbierania komunikatu z serwera");
                        exit(1);
                } else printf("klient %ld: Odebrano komunikat o tresci: '%s' \n", client_pid, announ_2.message);
        }
}

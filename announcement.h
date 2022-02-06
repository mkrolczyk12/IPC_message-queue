#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MESSAGE_SIZE 128 // max. rozmiar (dlugosc) wiadomosci zawartej w komunikacie

// struktura komunikatu
typedef struct announcement {
        long type;
        long sender;
        char message[MESSAGE_SIZE];
} announ;
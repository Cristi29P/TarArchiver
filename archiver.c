// PARIS CRISTIAN-TANASE 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "functii.h"

int main() {
    char input[511];

    fgets(input, 511, stdin);
    input[strlen(input) - 1] = '\0';

    while (strcmp(input, "exit")) {
        verificare_input(input);
        fgets(input, 511, stdin);
        input[strlen(input) - 1] = '\0';
    }

    return 0;
}

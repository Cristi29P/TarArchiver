// PARIS CRISTIAN-TANASE 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "functii.h"

int determinare_nr_argumente(char input[]) {
    char copie[511];
    char *argument;
    int nr_argumente = -1;

    strncpy(copie, input, 511);
    argument = strtok(copie, " ");

    while (argument != NULL) {
        nr_argumente++;
        argument = strtok(NULL, " ");
    }

    return nr_argumente;
}

int calcul_perm(char perm[]) {
    unsigned long i;
    int suma = 0;

    for (i = 0; i < strlen(perm); i++)
        if (perm[i] == '-') {
            perm[i] = '0';
        }

    for (i = 1; i <= 3; i++) {
        if (perm[i] == 'r') {
            suma += 00400;
        }
        if (perm[i] == 'w') {
            suma += 00200;
        }
        if (perm[i] == 'x') {
            suma += 00100;
        }
    }

    for (i = 4; i <= 6; i++) {
        if (perm[i] == 'r') {
            suma += 00040;
        }
        if (perm[i] == 'w') {
            suma += 00020;
        }
        if (perm[i] == 'x') {
            suma += 00010;
        }
    }

    for (i = 7; i <= 9; i++) {
        if (perm[i] == 'r') {
            suma += 00004;
        }
        if (perm[i] == 'w') {
            suma += 00002;
        }
        if (perm[i] == 'x') {
            suma += 00001;
        }
    }

    return suma;
}

int nr_cifre(long result) {
    int k = 0;

    while (result != 0) {
        result = result / 10;
        k++;
    }

    return k;
}

// Functie care imi converteste un numar dat in baza 8
long octal(long result) {
    long numar_octal = 0, temp = 1;

    while (result != 0) {
        numar_octal = numar_octal + (result % 8) * temp;
        result = result / 8;
        temp = temp * 10;
    }

    return numar_octal;
}

long decToOct(char size[]) {
    char *eptr;
    long result;

    result = strtol(size, &eptr, 10);
    result = octal(result);
    return result;
}

int stringToDecimal(char size[]) {
    char *eptr;
    int result;

    result = strtol(size, &eptr, 10);
    return result;
}

long stringToOctal(char size[]) {
    char *eptr;
    long result;

    result = strtol(size, &eptr, 8);
    return result;
}

long file_size(char *filename) {
    FILE *f;
    long size = -1;
    if ((f = fopen(filename, "rb")) == NULL) {
        return size;
    }

    size = 0;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);

    return size;
}

int exista_fisier(char argument[]) {
    FILE *f1;
    if ((f1 = fopen(argument, "rb"))) {
        fclose(f1);
        return 1;
    }
    return 0;
}

void scriere_fisier(char size[], FILE *f3, FILE *f4, record *header_date) {
    int cat, rest, nr_blocuri = 0;

    cat = stringToDecimal(size) / 512;
    rest = stringToDecimal(size) % 512;

    if (rest == 0) {
        nr_blocuri = cat;
        while (nr_blocuri != 0) {
            fread(header_date, sizeof(record), 1, f3);
            fwrite(header_date, sizeof(record), 1, f4);
            memset(header_date, '\0', 512);
            nr_blocuri--;
        }
    } else if (rest != 0) {
        nr_blocuri = cat + 1;
            while (nr_blocuri != 0) {
            fread(header_date, sizeof(record), 1, f3);
            fwrite(header_date, sizeof(record), 1, f4);
            memset(header_date, '\0', 512);
            nr_blocuri--;
        }
    }
}

void suma_header(record *header_date) {
    int i, poz;
    long suma = 0;

    memset(header_date->header.chksum, ' ', 8);
    suma = 0;

    for (i = 0; i < 512; i++) {
        suma += header_date->charptr[i];
    }

    poz = 7 - nr_cifre(octal(suma));
    memset(header_date->header.chksum, '0', 8);
    sprintf(header_date->header.chksum + poz , "%ld", octal(suma));
}

// Functie care calculeaza si returneaza timestamp-ul (epoch time-ul in unix)
void calculare_timestamp(char timp[], record *header_date) {
    int an, poz;

    long timestamp;

    struct tm t;
    time_t day_t;

    sscanf(timp, "%d-%d-%d %d:%d:%d",
        &an, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
    t.tm_year = an - 1900;
    t.tm_mon -= 1;
    t.tm_isdst = -1;
    day_t = mktime(&t);

    timestamp = (long) day_t;
    poz = 11 - nr_cifre(octal(timestamp));
    memset(header_date->header.mtime, '0', 12);
    sprintf(header_date->header.mtime + poz, "%ld", octal(timestamp));
}

void create(char input[]) {
    char *p, *q, comanda[10], argument1[100], argument2[100], linie_text[511];
    char linie_user[511], perm[12], size[12], no_links[2], owner_name[32];
    char owner_group[32], uid[8], gid[8], timp[30], name[100];

    int poz = 0;

    FILE *f1, *f2, *f3, *f4;

    record *header_date;

    header_date = (record *)calloc(1, sizeof(record));

    p = strtok(input, " ");
    strcpy(comanda, p);
    p = strtok(NULL, " ");
    strcpy(argument1, p);
    p = strtok(NULL, " ");
    strcpy(argument2, p);

    f1 = fopen("files.txt", "rt");
    f2 = fopen("usermap.txt", "rt");
    f4 = fopen(argument1, "wb");

    while (fgets(linie_text, 511, f1) != NULL) {
        memset(header_date, '\0', 512);
        linie_text[strlen(linie_text) - 1] = '\0';

        p = strtok(linie_text, " ");
        strcpy(perm, p);

        p = strtok(NULL, " ");
        strcpy(no_links, p);

        p = strtok(NULL, " ");
        strcpy(owner_name, p);

        p = strtok(NULL, " ");
        strcpy(owner_group, p);

        p = strtok(NULL, " ");
        strcpy(size, p);

        p = strtok(NULL, ".");
        strcpy(timp, p);

        p = strtok(NULL, " ");
        p = strtok(NULL, " ");
        p = strtok(NULL, " ");
        strcpy(name, p);

        while (fgets(linie_user, 511, f2) != NULL) {
            linie_user[strlen(linie_user) - 1] = '\0';
            q = strtok(linie_user, ":");
            if (!(strcmp(q, owner_name))) {
                q = strtok(NULL, ":");
                q = strtok(NULL, ":");
                strcpy(uid, q);
                q = strtok(NULL, ":");
                strcpy(gid, q);
                break;
            }
        }
        // Scriere header
        sprintf(header_date->header.mode, "0000%o", calcul_perm(perm)); // mode
        sprintf(header_date->header.magic, "GNUtar "); // magic
        sprintf(header_date->header.uname, "%s", owner_name); // uname
        sprintf(header_date->header.gname, "%s", owner_group); // gname
        sprintf(header_date->header.name, "%s", name); // name
        sprintf(header_date->header.linkname, "%s", name); // linkname

        memset(header_date->header.size, '0', 11);
        if (stringToDecimal(size) != 0) {
            poz = 11 - nr_cifre(decToOct(size));
            memset(header_date->header.size, '0', 12);
            sprintf(header_date->header.size + poz, "%ld", decToOct(size));
        }

        poz = 7 - nr_cifre(decToOct(uid));
        memset(header_date->header.uid, '0', 8);
        sprintf(header_date->header.uid + poz, "%ld", decToOct(uid)); // uid

        poz = 7 - nr_cifre(decToOct(gid));
        memset(header_date->header.gid, '0', 8);
        sprintf(header_date->header.gid + poz, "%ld", decToOct(gid)); // gid

        calculare_timestamp(timp, header_date);

        suma_header(header_date);

        fwrite(header_date, sizeof(record), 1, f4);

        // Scrierea datelor
        // Deschidere
        memset(header_date, '\0', 512);
        f3 = fopen(name, "rb");

        scriere_fisier(size, f3, f4, header_date);

        fclose(f3);
    }
    // Final de arhiva
    memset(header_date, '\0', 512);
    fwrite(header_date, sizeof(record), 1, f4);

    printf("> Done!\n");

    fclose(f1);
    fclose(f2);
    fclose(f4);
    free(header_date);
}

void list(char input[]) {
    FILE *f1;
    record *header_date;

    char *p, comanda[10], argument[100], name[100], size[12];

    long marime_arhiva = 0;
    long marime_fisier = 0;
    long marime_parcursa = 0;

    header_date = (record *)calloc(1, sizeof(record));

    p = strtok(input, " ");
    strcpy(comanda, p);
    p = strtok(NULL, " ");
    strcpy(argument, p);

    if (exista_fisier(argument) == 1) {
        f1 = fopen(argument, "rb");

        marime_arhiva = file_size(argument);
        marime_parcursa = 0;

        while (marime_parcursa < marime_arhiva - 512) {
            memset(header_date, '\0', 512);
            fread(header_date, sizeof(record), 1, f1);
            marime_parcursa += 512;
            strcpy(name, header_date->header.name);
            strcpy(size, header_date->header.size);

            printf("> %s\n", name);

            marime_fisier = stringToOctal(size);
            marime_fisier = marime_fisier + (512 - (marime_fisier % 512));
            marime_parcursa += marime_fisier;
            fseek(f1, marime_fisier, SEEK_CUR);
        }
        fclose(f1);
    } else {
        printf("> File not found!\n");
    }
    free(header_date);
}

void scriere_extract(char arg1[], char arg2[], long poz_act, long size) {
    char nume_nou[100] = "extracted_", *record;

    record = (char *)calloc(size, sizeof(char));

    FILE *f2, *f3;

    strcat(nume_nou, arg1);

    f2 = fopen(nume_nou, "wb");
    f3 = fopen(arg2, "rb");

    fseek(f3, poz_act, SEEK_SET);

    fread(record, size, 1, f3);
    fwrite(record, size, 1, f2);

    free(record);
    fclose(f2);
    fclose(f3);
}

void extract(char input[]) {
    FILE *f1;
    record *header_date;

    header_date = (record *)calloc(1, sizeof(record));

    char *p, comanda[10], arg1[100], arg2[100], name[100], size[12];

    long marime_arhiva = 0, marime_fisier = 0, marime_parcursa = 0, poz_act;

    int ok = 0;

    p = strtok(input, " ");
    strcpy(comanda, p);
    p = strtok(NULL, " ");
    strcpy(arg1, p);
    p = strtok(NULL, " ");
    strcpy(arg2, p);

    if (exista_fisier(arg2) == 1) {
        f1 = fopen(arg2, "rb");

        marime_arhiva = file_size(arg2);
        marime_parcursa = 0;

        while ((marime_parcursa < marime_arhiva - 512) && (ok == 0)) {
            memset(header_date, '\0', 512);
            fread(header_date, sizeof(record), 1, f1);
            marime_parcursa += 512;
            strcpy(name, header_date->header.name);
            strcpy(size, header_date->header.size);

            if (!(strcmp(name, arg1))) {
                ok = 1;
                poz_act = ftell(f1);
                scriere_extract(arg1, arg2, poz_act, stringToOctal(size));
            }

            marime_fisier = stringToOctal(size);
            marime_fisier = marime_fisier + (512 - (marime_fisier % 512));
            marime_parcursa += marime_fisier;
            fseek(f1, marime_fisier, SEEK_CUR);
        }
        if (ok == 0) {
            printf("> File not found!\n");
        }
        fclose(f1);
    } else {
        printf("> File not found!\n");
    }
    free(header_date);
}

void verificare_input(char input[]) {
    long nr_argumente = determinare_nr_argumente(input);
    char copie_input[511];

    strncpy(copie_input, input, 511);

    if (nr_argumente == 1) {
        if (!(strcmp(strtok(copie_input, " "), "list"))) {
            list(input);
        } else {
            printf("> Wrong command!\n");
        }
    } else if (nr_argumente == 2) {
        if (!(strcmp(strtok(copie_input, " "), "create"))) {
            create(input);
        } else if (!(strcmp(strtok(copie_input, " "), "extract"))) {
            extract(input);
        } else {
            printf("> Wrong command!\n");
        }
    } else {
         printf("> Wrong command!\n");
    }
}

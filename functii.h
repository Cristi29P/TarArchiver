// PARIS CRISTIAN-TANASE 311CA
#ifndef FUNCTII_H
#define FUNCTII_H

typedef union {
    char charptr[512];
    struct header {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char chksum[8];
        char typeflag;
        char linkname[100];
        char magic[8];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
    } header;
} record;

int determinare_nr_argumente(char input[]);

int calcul_perm(char perm[]);

int nr_cifre(long result);

long octal(long result);

long decToOct(char size[]);

int stringToDecimal(char size[]);

long stringToOctal(char size[]);

long file_size(char *filename);

int exista_fisier(char argument[]);

void scriere_fisier(char size[], FILE *f3, FILE *f4, record *header_date);

void suma_header(record *header_date);

void calculare_timestamp(char timp[], record *header_date);

void create(char input[]);

void list(char input[]);

void scriere_extract(char arg1[], char arg2[], long poz_act, long size);

void extract(char input[]);

void verificare_input(char input[]);

#endif

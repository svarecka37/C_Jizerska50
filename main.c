#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define IN "jizerska50.txt"
#define OUT "vysledkova_listina.txt"
#define BUFSIZE 200
#define DELIMITERS " \t\n:"

typedef struct CAS {
    int hodiny;
    int minuty;
    int sekundy;
} CAS;

typedef struct ZAVODNIK {
    int poradi;
    int cislo;
    char prijmeni[30];
    char jmeno[30];
    int rocnik;
    char stat[5];
    CAS cas;
    int totalSec;   // celkový čas v sekundách
} ZAVODNIK;

int casNaSekundy(const CAS time) {
    return time.hodiny * 3600 + time.minuty * 60 + time.sekundy;
}

CAS sekundyNaCas(int sec) {
    CAS t;
    t.hodiny = sec / 3600;
    sec %= 3600;
    t.minuty = sec / 60;
    t.sekundy = sec % 60;
    return t;
}

void swap(ZAVODNIK *a, ZAVODNIK *b) {
    ZAVODNIK tmp = *a;
    *a = *b;
    *b = tmp;
}

void bubbleSort(ZAVODNIK *zav, int pocet) {
    for (int i = 0; i < pocet - 1; i++) {
        for (int j = 0; j < pocet - i - 1; j++) {
            if (zav[j].totalSec > zav[j+1].totalSec) {
                swap(&zav[j], &zav[j+1]);
            }
        }
    }
}

int readFile(ZAVODNIK *zav) {
    FILE *fr;
    char buffer[BUFSIZE];
    int pocet = 0;

    if ((fr = fopen(IN, "r")) == NULL) {
        printf("Chyba při otevírání souboru %s.\n", IN);
        return EXIT_FAILURE;
    }

    while (fgets(buffer, BUFSIZE, fr) != NULL) {
        if (strlen(buffer) < 5) continue;
        ZAVODNIK z;
        int hh, mm, ss;
        if (sscanf(buffer, "%d %d %s %s %d %s %d:%d:%d",
                   &z.poradi, &z.cislo, z.prijmeni, z.jmeno,
                   &z.rocnik, z.stat, &hh, &mm, &ss) == 9) {
            z.cas.hodiny = hh;
            z.cas.minuty = mm;
            z.cas.sekundy = ss;
            z.totalSec = casNaSekundy(z.cas);
            zav[pocet++] = z;
        }
    }

    fclose(fr);
    return pocet;
}

void vypisStartovniListinu(ZAVODNIK *zav, int pocet) {
    printf("VYSLEDKOVA LISTINA - JIZERSKA 50\n");
    printf("Poradi | Cislo | Prijmeni | Jmeno | Rocnik | Stat | Cas\n");
    for (int i = 0; i < pocet; i++) {
        printf("%6d %6d %-15s %-15s %6d %4s %02d:%02d:%02d\n",
               zav[i].poradi,
               zav[i].cislo,
               zav[i].prijmeni,
               zav[i].jmeno,
               zav[i].rocnik,
               zav[i].stat,
               zav[i].cas.hodiny,
               zav[i].cas.minuty,
               zav[i].cas.sekundy);
    }
    printf("Celkový počet závodníků: %d\n", pocet);
}

void vypisNejmladsiho(ZAVODNIK *zav, int pocet) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int currentYear = t->tm_year + 1900;

    ZAVODNIK nejml = zav[0];
    for (int i = 1; i < pocet; i++) {
        if (zav[i].rocnik > nejml.rocnik) {
            nejml = zav[i];
        }
    }

    int vek = currentYear - nejml.rocnik;
    printf("Nejmladší závodník: %s %s, rok %d, věk %d let.\n",
           nejml.jmeno, nejml.prijmeni, nejml.rocnik, vek);
}

void pocetCechu(ZAVODNIK *zav, int pocet) {
    int count = 0;
    for (int i = 0; i < pocet; i++) {
        if (strcmp(zav[i].stat, "CZE") == 0) count++;
    }
    printf("Počet českých závodníků: %d\n", count);
}

void writeToFile(ZAVODNIK *zav, int pocet) {
    FILE *fw;
    if ((fw = fopen(OUT, "w")) == NULL) {
        printf("Chyba při otevírání výstupního souboru %s.\n", OUT);
        return;
    }

    fprintf(fw, "VYSLEDKOVA LISTINA - JIZERSKA 50\n");
    fprintf(fw, "Poradi | Cislo | Prijmeni | Jmeno | Rocnik | Stat | Cas | Ztrata\n");

    int best = zav[0].totalSec;
    for (int i = 0; i < pocet; i++) {
        int diff = zav[i].totalSec - best;
        CAS c = sekundyNaCas(diff);
        fprintf(fw, "%6d %6d %-15s %-15s %6d %4s %02d:%02d:%02d %+02d:%02d:%02d\n",
                i+1,
                zav[i].cislo,
                zav[i].prijmeni,
                zav[i].jmeno,
                zav[i].rocnik,
                zav[i].stat,
                zav[i].cas.hodiny,
                zav[i].cas.minuty,
                zav[i].cas.sekundy,
                c.hodiny, c.minuty, c.sekundy);
    }

    fclose(fw);
    printf("Soubor %s byl vytvořen.\n", OUT);
}

int main() {
    ZAVODNIK zav[BUFSIZE];
    int pocet = readFile(zav);
    if (pocet <= 0) return EXIT_FAILURE;

    vypisStartovniListinu(zav, pocet);
    vypisNejmladsiho(zav, pocet);
    pocetCechu(zav, pocet);

    bubbleSort(zav, pocet);
    writeToFile(zav, pocet);

    return EXIT_SUCCESS;
}

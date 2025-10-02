#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IN "jizerska50.txt"
#define OUT "vysledkova_listina.txt"
#define BUFSIZE 200
#define MAX 200
#define DELIM ";"

typedef struct {
    int cislo;
    char prijmeni[30];
    char jmeno[30];
    int rocnik;
    char stat[5];
    int casSec;   // čas v sekundách
} ZAVODNIK;

// převod HH:MM:SS -> sekundy
int casNaSekundy(const char *cas) {
    int h, m, s;
    sscanf(cas, "%d:%d:%d", &h, &m, &s);
    return h * 3600 + m * 60 + s;
}

// převod sekund -> h,m,s
void sekundyNaCas(int sec, int *h, int *m, int *s) {
    *h = sec / 3600;
    *m = (sec / 60) % 60;
    *s = sec % 60;
}

// načtení dat ze souboru
int nactiSoubor(ZAVODNIK *zav) {
    FILE *fr;
    char buffer[BUFSIZE];
    int pocet = 0;

    if ((fr = fopen(IN, "r")) == NULL) {
        printf("Chyba při otevírání souboru %s.\n", IN);
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), fr) != NULL) {
        char *tok = strtok(buffer, DELIM);
        if (!tok) continue;
        zav[pocet].cislo = atoi(tok);

        tok = strtok(NULL, DELIM);
        sscanf(tok, "%[^,], %s (%d)", zav[pocet].prijmeni, zav[pocet].jmeno, &zav[pocet].rocnik);

        tok = strtok(NULL, DELIM);
        strcpy(zav[pocet].stat, tok);

        tok = strtok(NULL, ";\n");
        zav[pocet].casSec = casNaSekundy(tok);

        pocet++;
    }
    fclose(fr);
    return pocet;
}

// výpis startovní listiny
void vypisStartovniListinu(ZAVODNIK *zav, int n) {
    printf("STARTOVNI LISTINA - JIZERSKA 50\n");
    printf("Cislo | Prijmeni | Jmeno | Rocnik | Stat | Cas\n");
    for (int i = 0; i < n; i++) {
        int h,m,s;
        sekundyNaCas(zav[i].casSec, &h, &m, &s);
        printf("%5d %-12s %-12s %6d %4s %02d:%02d:%02d\n",
               zav[i].cislo, zav[i].prijmeni, zav[i].jmeno,
               zav[i].rocnik, zav[i].stat, h, m, s);
    }
    printf("Celkový počet závodníků: %d\n", n);
}

// nejmladší závodník
void vypisNejmladsiho(ZAVODNIK *zav, int n) {
    int idx = 0;
    for (int i = 1; i < n; i++) {
        if (zav[i].rocnik > zav[idx].rocnik) idx = i;
    }
    int rok = localtime(&(time_t){time(NULL)})->tm_year + 1900;
    printf("Nejmladší závodník: %s %s (%d), %d let\n",
           zav[idx].jmeno, zav[idx].prijmeni,
           zav[idx].rocnik, rok - zav[idx].rocnik);
}

// počet českých závodníků
void pocetCechu(ZAVODNIK *zav, int n) {
    int count = 0;
    for (int i = 0; i < n; i++)
        if (strcmp(zav[i].stat, "CZE") == 0) count++;
    printf("Počet českých závodníků: %d\n", count);
}

// prohození
void swap(ZAVODNIK *a, ZAVODNIK *b) {
    ZAVODNIK tmp = *a;
    *a = *b;
    *b = tmp;
}

// bublinové třídění podle času
void bblSort(ZAVODNIK *zav, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (zav[j].casSec > zav[j+1].casSec) {
                swap(&zav[j], &zav[j+1]);
            }
        }
    }
}

// zápis výsledků do souboru
void ulozVysledky(ZAVODNIK *zav, int n) {
    FILE *fw = fopen(OUT, "w");
    if (!fw) {
        printf("Chyba při zápisu do souboru %s.\n", OUT);
        return;
    }

    fprintf(fw, "VYSLEDKOVA LISTINA - JIZERSKA 50\n");
    fprintf(fw, "Poradi | Cislo | Prijmeni | Jmeno | Rocnik | Stat | Cas | Ztrata\n");

    int best = zav[0].casSec;
    for (int i = 0; i < n; i++) {
        int h,m,s,dh,dm,ds;
        sekundyNaCas(zav[i].casSec, &h, &m, &s);
        sekundyNaCas(zav[i].casSec - best, &dh, &dm, &ds);
        fprintf(fw, "%5d %6d %-12s %-12s %6d %4s %02d:%02d:%02d +%02d:%02d:%02d\n",
                i+1, zav[i].cislo, zav[i].prijmeni, zav[i].jmeno,
                zav[i].rocnik, zav[i].stat, h, m, s, dh, dm, ds);
    }
    fclose(fw);
    printf("Soubor %s byl vytvořen.\n", OUT);
}

int main() {
    ZAVODNIK zav[MAX];
    int n = nactiSoubor(zav);
    if (n == 0) return 1;

    vypisStartovniListinu(zav, n);
    vypisNejmladsiho(zav, n);
    pocetCechu(zav, n);

    bblSort(zav, n);
    ulozVysledky(zav, n);

    return 0;
}


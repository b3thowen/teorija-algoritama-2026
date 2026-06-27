#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INICIJALNI_KAPACITET 7
#define FAKTOR_OPTERECENJA 0.5

typedef struct {
    char jmbg[14];
    char ime[50];
    char prezime[50];
    char zanimanje[50];
    int zauzet; /* 1 = zauzet, 0 = prazan, -1 = obrisan */
} Slot;

typedef struct {
    Slot *tabela;
    int kapacitet;
    int velicina;
} HashMap;

int je_prost(int n) {
    int i;
    if (n < 2) return 0;
    for (i = 2; i * i <= n; i++)
        if (n % i == 0) return 0;
    return 1;
}

int sledeci_prost(int n) {
    while (!je_prost(n))
        n++;
    return n;
}

/* djb2 hash funkcija */
unsigned int djb2_hash(HashMap *mapa, const char *kljuc) {
    unsigned long hash = 5381;
    int i = 0;
    int c;
    while ((c = kljuc[i++]))
        hash = hash * 33 + c;
    return hash % mapa->kapacitet;
}

HashMap *kreiraj_mapu(int kapacitet) {
    HashMap *mapa = (HashMap *)malloc(sizeof(HashMap));
    if (mapa == NULL) {
        printf("Greska pri alokaciji mape.\n");
        return NULL;
    }
    mapa->tabela = (Slot *)calloc(kapacitet, sizeof(Slot));
    if (mapa->tabela == NULL) {
        printf("Greska pri alokaciji tabele.\n");
        free(mapa);
        return NULL;
    }
    mapa->kapacitet = kapacitet;
    mapa->velicina = 0;
    return mapa;
}

void oslobodi_mapu(HashMap *mapa) {
    free(mapa->tabela);
    free(mapa);
}

void rehash(HashMap *mapa);

void ubaci(HashMap *mapa, const char *jmbg, const char *ime,
           const char *prezime, const char *zanimanje) {
    unsigned int indeks, i;
    int probe;

    if ((double)(mapa->velicina + 1) / mapa->kapacitet > FAKTOR_OPTERECENJA)
        rehash(mapa);

    indeks = djb2_hash(mapa, jmbg);

    for (i = 0; i < (unsigned int)mapa->kapacitet; i++) {
        /* kvadratno probiranje: i + 1, i + 4, i + 9, ... */
        probe = (indeks + (i + 1) * (i + 1)) % mapa->kapacitet;

        /* provjeri je li slot slobodan ili obrisan */
        if (mapa->tabela[probe].zauzet != 1) {
            strcpy(mapa->tabela[probe].jmbg, jmbg);
            strcpy(mapa->tabela[probe].ime, ime);
            strcpy(mapa->tabela[probe].prezime, prezime);
            strcpy(mapa->tabela[probe].zanimanje, zanimanje);
            mapa->tabela[probe].zauzet = 1;
            mapa->velicina++;
            return;
        }

        /* azuriraj ako jmbg vec postoji */
        if (strcmp(mapa->tabela[probe].jmbg, jmbg) == 0) {
            strcpy(mapa->tabela[probe].ime, ime);
            strcpy(mapa->tabela[probe].prezime, prezime);
            strcpy(mapa->tabela[probe].zanimanje, zanimanje);
            return;
        }
    }

    printf("Greska: tabela je puna.\n");
}

void rehash(HashMap *mapa) {
    int stari_kapacitet = mapa->kapacitet;
    int novi_kapacitet = sledeci_prost(stari_kapacitet * 2);
    Slot *stara_tabela = mapa->tabela;
    Slot *nova_tabela = (Slot *)calloc(novi_kapacitet, sizeof(Slot));
    int i, j;
    unsigned int novi_indeks;
    int probe;

    if (nova_tabela == NULL) {
        printf("Greska pri rehashingu.\n");
        return;
    }

    mapa->tabela = nova_tabela;
    mapa->kapacitet = novi_kapacitet;
    mapa->velicina = 0;

    for (i = 0; i < stari_kapacitet; i++) {
        if (stara_tabela[i].zauzet != 1)
            continue;

        novi_indeks = djb2_hash(mapa, stara_tabela[i].jmbg);

        for (j = 0; j < novi_kapacitet; j++) {
            probe = (novi_indeks + (j + 1) * (j + 1)) % novi_kapacitet;
            if (mapa->tabela[probe].zauzet != 1) {
                mapa->tabela[probe] = stara_tabela[i];
                mapa->velicina++;
                break;
            }
        }
    }

    free(stara_tabela);
    printf("[rehash] novi kapacitet: %d\n", mapa->kapacitet);
}

Slot *trazi(HashMap *mapa, const char *jmbg) {
    unsigned int indeks = djb2_hash(mapa, jmbg);
    int i, probe;

    for (i = 0; i < mapa->kapacitet; i++) {
        probe = (indeks + (i + 1) * (i + 1)) % mapa->kapacitet;

        if (mapa->tabela[probe].zauzet == 0)
            return NULL; /* prazan slot, nema dalje */

        if (mapa->tabela[probe].zauzet == 1 &&
            strcmp(mapa->tabela[probe].jmbg, jmbg) == 0)
            return &mapa->tabela[probe];
    }
    return NULL;
}

void obrisi(HashMap *mapa, const char *jmbg) {
    unsigned int indeks = djb2_hash(mapa, jmbg);
    int i, probe;

    for (i = 0; i < mapa->kapacitet; i++) {
        probe = (indeks + (i + 1) * (i + 1)) % mapa->kapacitet;

        if (mapa->tabela[probe].zauzet == 0) {
            printf("JMBG '%s' nije pronadjen.\n", jmbg);
            return;
        }

        if (mapa->tabela[probe].zauzet == 1 &&
            strcmp(mapa->tabela[probe].jmbg, jmbg) == 0) {
            mapa->tabela[probe].zauzet = -1; /* lazy delete */
            mapa->velicina--;
            printf("Obrisano: %s\n", jmbg);
            return;
        }
    }
    printf("JMBG '%s' nije pronadjen.\n", jmbg);
}

void ispisi_mapu(HashMap *mapa) {
    int i;
    printf("\n--- HashMap (kapacitet: %d, velicina: %d) ---\n",
           mapa->kapacitet, mapa->velicina);
    for (i = 0; i < mapa->kapacitet; i++) {
        printf("[%2d]: ", i);
        if (mapa->tabela[i].zauzet == 0)
            printf("(prazno)");
        else if (mapa->tabela[i].zauzet == -1)
            printf("(obrisan)");
        else
            printf("JMBG: %-14s | %s %s | %s",
                   mapa->tabela[i].jmbg,
                   mapa->tabela[i].ime,
                   mapa->tabela[i].prezime,
                   mapa->tabela[i].zanimanje);
        printf("\n");
    }
    printf("-------------------------------------------\n\n");
}

int main() {
    HashMap *mapa = kreiraj_mapu(INICIJALNI_KAPACITET);
    Slot *rezultat;

    ubaci(mapa, "1234567890123", "Petar",  "Petrovic", "inzenjer");
    ubaci(mapa, "9876543210987", "Marija", "Markovic", "lekar");
    ubaci(mapa, "1111111111111", "Ana",    "Anic",     "ucitelj");
    ubaci(mapa, "2222222222222", "Jovan",  "Jovic",    "pravnik");
    /* sledeca ubacivanja ce okidati rehash (faktor > 0.5) */
    ubaci(mapa, "3333333333333", "Milica", "Milic",    "arhitekta");
    ubaci(mapa, "4444444444444", "Stefan", "Stefanovic","programer");

    ispisi_mapu(mapa);

    /* pretraga */
    rezultat = trazi(mapa, "9876543210987");
    if (rezultat != NULL)
        printf("Pronadjen: %s %s, %s\n",
               rezultat->ime, rezultat->prezime, rezultat->zanimanje);
    else
        printf("Nije pronadjen.\n");

    /* azuriranje */
    ubaci(mapa, "1234567890123", "Petar", "Petrovic", "direktor");
    rezultat = trazi(mapa, "1234567890123");
    printf("Azurirano zanimanje: %s\n", rezultat->zanimanje);

    /* brisanje */
    obrisi(mapa, "1111111111111");
    obrisi(mapa, "0000000000000"); /* ne postoji */

    ispisi_mapu(mapa);

    oslobodi_mapu(mapa);
    return 0;
}
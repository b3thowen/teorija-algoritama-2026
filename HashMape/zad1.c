#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INICIJALNI_KAPACITET 4
#define FAKTOR_OPTERECENJA 0.75

typedef struct Cvor {
    char *kljuc;
    double vrednost;
    struct Cvor *sled;
} Cvor;

typedef struct {
    Cvor **tabela;
    int kapacitet;
    int velicina;
} HashMap;

/* sdbm hash funkcija */
unsigned int sdbm_hash(HashMap *mapa, const char *kljuc) {
    unsigned long hash = 0;
    int i = 0;
    int c;
    while ((c = kljuc[i++]))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % mapa->kapacitet;
}

HashMap *kreiraj_mapu(int kapacitet) {
    HashMap *mapa = (HashMap *)malloc(sizeof(HashMap));
    if (mapa == NULL) {
        printf("Greska pri alokaciji mape.\n");
        return NULL;
    }
    mapa->tabela = (Cvor **)calloc(kapacitet, sizeof(Cvor *));
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
    int i;
    Cvor *trenutni, *sledeci;
    for (i = 0; i < mapa->kapacitet; i++) {
        trenutni = mapa->tabela[i];
        while (trenutni != NULL) {
            sledeci = trenutni->sled;
            free(trenutni->kljuc);
            free(trenutni);
            trenutni = sledeci;
        }
    }
    free(mapa->tabela);
    free(mapa);
}

void rehash(HashMap *mapa);

void ubaci(HashMap *mapa, const char *kljuc, double vrednost) {
    unsigned int indeks;
    Cvor *trenutni, *novi;

    /* rehash ako je faktor opterecenja premasен */
    if ((double)(mapa->velicina + 1) / mapa->kapacitet > FAKTOR_OPTERECENJA)
        rehash(mapa);

    indeks = sdbm_hash(mapa, kljuc);
    trenutni = mapa->tabela[indeks];

    /* ako kljuc vec postoji, azuriraj vrednost */
    while (trenutni != NULL) {
        if (strcmp(trenutni->kljuc, kljuc) == 0) {
            trenutni->vrednost = vrednost;
            return;
        }
        trenutni = trenutni->sled;
    }

    novi = (Cvor *)malloc(sizeof(Cvor));
    if (novi == NULL) {
        printf("Greska pri alokaciji cvora.\n");
        return;
    }
    novi->kljuc = (char *)malloc(strlen(kljuc) + 1);
    if (novi->kljuc == NULL) {
        printf("Greska pri alokaciji kljuca.\n");
        free(novi);
        return;
    }
    strcpy(novi->kljuc, kljuc);
    novi->vrednost = vrednost;
    novi->sled = mapa->tabela[indeks];
    mapa->tabela[indeks] = novi;
    mapa->velicina++;
}

void rehash(HashMap *mapa) {
    int novi_kapacitet = mapa->kapacitet * 2;
    Cvor **nova_tabela = (Cvor **)calloc(novi_kapacitet, sizeof(Cvor *));
    int i;
    Cvor *trenutni, *sledeci;
    unsigned int novi_indeks;

    if (nova_tabela == NULL) {
        printf("Greska pri rehashingu.\n");
        return;
    }

    for (i = 0; i < mapa->kapacitet; i++) {
        trenutni = mapa->tabela[i];
        while (trenutni != NULL) {
            sledeci = trenutni->sled;

            /* racunamo novi indeks sa novim kapacitetom */
            unsigned long hash = 0;
            int j = 0, c;
            while ((c = trenutni->kljuc[j++]))
                hash = c + (hash << 6) + (hash << 16) - hash;
            novi_indeks = hash % novi_kapacitet;

            trenutni->sled = nova_tabela[novi_indeks];
            nova_tabela[novi_indeks] = trenutni;
            trenutni = sledeci;
        }
    }

    free(mapa->tabela);
    mapa->tabela = nova_tabela;
    mapa->kapacitet = novi_kapacitet;

    printf("[rehash] novi kapacitet: %d\n", mapa->kapacitet);
}

double *trazi(HashMap *mapa, const char *kljuc) {
    unsigned int indeks = sdbm_hash(mapa, kljuc);
    Cvor *trenutni = mapa->tabela[indeks];
    while (trenutni != NULL) {
        if (strcmp(trenutni->kljuc, kljuc) == 0)
            return &trenutni->vrednost;
        trenutni = trenutni->sled;
    }
    return NULL;
}

void obrisi(HashMap *mapa, const char *kljuc) {
    unsigned int indeks = sdbm_hash(mapa, kljuc);
    Cvor *trenutni = mapa->tabela[indeks];
    Cvor *prethodni = NULL;

    while (trenutni != NULL) {
        if (strcmp(trenutni->kljuc, kljuc) == 0) {
            if (prethodni != NULL)
                prethodni->sled = trenutni->sled;
            else
                mapa->tabela[indeks] = trenutni->sled;
            free(trenutni->kljuc);
            free(trenutni);
            mapa->velicina--;
            return;
        }
        prethodni = trenutni;
        trenutni = trenutni->sled;
    }
    printf("Kljuc '%s' nije pronadjen.\n", kljuc);
}

void ispisi_mapu(HashMap *mapa) {
    int i;
    Cvor *trenutni;
    printf("\n--- HashMap (kapacitet: %d, velicina: %d) ---\n",
           mapa->kapacitet, mapa->velicina);
    for (i = 0; i < mapa->kapacitet; i++) {
        printf("[%2d]: ", i);
        trenutni = mapa->tabela[i];
        if (trenutni == NULL) {
            printf("(prazno)");
        } else {
            while (trenutni != NULL) {
                printf("(\"%s\" -> %.2f) ", trenutni->kljuc, trenutni->vrednost);
                if (trenutni->sled != NULL)
                    printf("-> ");
                trenutni = trenutni->sled;
            }
        }
        printf("\n");
    }
    printf("-------------------------------------------\n\n");
}

int main() {
    HashMap *mapa = kreiraj_mapu(INICIJALNI_KAPACITET);
    double *rezultat;

    ubaci(mapa, "jabuka", 1.50);
    ubaci(mapa, "banana", 0.99);
    ubaci(mapa, "visnja", 3.20);
    ubaci(mapa, "kruska", 2.10);   /* ovde ce se okidati rehash */
    ubaci(mapa, "grozdje", 4.50);
    ubaci(mapa, "breskva", 1.80);

    ispisi_mapu(mapa);

    /* pretraga */
    rezultat = trazi(mapa, "banana");
    if (rezultat != NULL)
        printf("banana: %.2f\n", *rezultat);
    else
        printf("banana nije pronadjena.\n");

    /* azuriranje */
    ubaci(mapa, "jabuka", 9.99);
    rezultat = trazi(mapa, "jabuka");
    printf("jabuka (azurirano): %.2f\n", *rezultat);

    /* brisanje */
    obrisi(mapa, "visnja");
    printf("visnja obrisana.\n");
    obrisi(mapa, "mango");  /* ne postoji */

    ispisi_mapu(mapa);

    oslobodi_mapu(mapa);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_CVOROVA   20
#define MAX_GRANA     50
#define BESKONACNO    INT_MAX

typedef struct CvorListe {
    int              dest;
    int              tezina;
    struct CvorListe *sledeci;
} CvorListe;

typedef struct {
    int       brojCvorova;
    char      oznaka[MAX_CVOROVA][4];
    CvorListe *listaSledecih[MAX_CVOROVA];
} Graf;

typedef struct {
    int cvor;
    int kljuc;
} CvorGomile;

typedef struct {
    CvorGomile podaci[MAX_CVOROVA];
    int        pozicija[MAX_CVOROVA];
    int        velicina;
} MinGomila;

void inicijalizujGraf(Graf *g) {
    g->brojCvorova = 0;
    for (int i = 0; i < MAX_CVOROVA; i++)
        g->listaSledecih[i] = NULL;
}

int dodajCvor(Graf *g, const char *ozn) {
    int idx = g->brojCvorova++;
    strncpy(g->oznaka[idx], ozn, 3);
    g->oznaka[idx][3] = '\0';
    return idx;
}

int pronadjiCvor(Graf *g, const char *ozn) {
    for (int i = 0; i < g->brojCvorova; i++)
        if (strcmp(g->oznaka[i], ozn) == 0)
            return i;
    return -1;
}

static void dodajUsmerenGranu(Graf *g, int izvor, int dest, int t) {
    CvorListe *cvor = (CvorListe *)malloc(sizeof(CvorListe));
    if (cvor == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    cvor->dest      = dest;
    cvor->tezina    = t;
    cvor->sledeci   = g->listaSledecih[izvor];
    g->listaSledecih[izvor] = cvor;
}

void dodajGranu(Graf *g, const char *oznIzvor, const char *oznDest, int t) {
    int iz = pronadjiCvor(g, oznIzvor);
    int do_ = pronadjiCvor(g, oznDest);
    if (iz == -1 || do_ == -1) {
        fprintf(stderr, "Greska: cvor nije pronadjen!\n");
        return;
    }
    dodajUsmerenGranu(g, iz, do_, t);
    dodajUsmerenGranu(g, do_, iz, t);
}

void oslobodiGraf(Graf *g) {
    for (int i = 0; i < g->brojCvorova; i++) {
        CvorListe *tek = g->listaSledecih[i];
        while (tek) {
            CvorListe *tmp = tek->sledeci;
            free(tek);
            tek = tmp;
        }
        g->listaSledecih[i] = NULL;
    }
}

void inicijalizujGomilu(MinGomila *h) {
    h->velicina = 0;
    for (int i = 0; i < MAX_CVOROVA; i++)
        h->pozicija[i] = -1;
}

static void zameni(MinGomila *h, int i, int j) {
    h->pozicija[h->podaci[i].cvor] = j;
    h->pozicija[h->podaci[j].cvor] = i;
    CvorGomile tmp = h->podaci[i];
    h->podaci[i]   = h->podaci[j];
    h->podaci[j]   = tmp;
}

static void popraviGore(MinGomila *h, int i) {
    while (i > 0) {
        int roditelj = (i - 1) / 2;
        if (h->podaci[roditelj].kljuc > h->podaci[i].kljuc) {
            zameni(h, roditelj, i);
            i = roditelj;
        } else {
            break;
        }
    }
}

static void popraviDole(MinGomila *h, int i) {
    int najmanji = i;
    int levo     = 2 * i + 1;
    int desno    = 2 * i + 2;

    if (levo  < h->velicina && h->podaci[levo].kljuc  < h->podaci[najmanji].kljuc)
        najmanji = levo;
    if (desno < h->velicina && h->podaci[desno].kljuc < h->podaci[najmanji].kljuc)
        najmanji = desno;

    if (najmanji != i) {
        zameni(h, i, najmanji);
        popraviDole(h, najmanji);
    }
}

void umetniUGomilu(MinGomila *h, int cvor, int kljuc) {
    int i = h->velicina++;
    h->podaci[i].cvor  = cvor;
    h->podaci[i].kljuc = kljuc;
    h->pozicija[cvor]  = i;
    popraviGore(h, i);
}

CvorGomile izvuciMinimum(MinGomila *h) {
    CvorGomile minCvor = h->podaci[0];

    h->pozicija[minCvor.cvor] = -1;

    h->velicina--;
    if (h->velicina > 0) {
        h->podaci[0]                    = h->podaci[h->velicina];
        h->pozicija[h->podaci[0].cvor]  = 0;
        popraviDole(h, 0);
    }

    return minCvor;
}

void smanjiKljuc(MinGomila *h, int cvor, int noviKljuc) {
    int i = h->pozicija[cvor];
    h->podaci[i].kljuc = noviKljuc;
    popraviGore(h, i);
}

int sadrzuGomila(MinGomila *h, int cvor) {
    return h->pozicija[cvor] != -1;
}

void primMST(Graf *g, int koren, int roditelj[], int kljuc[]) {
    int n = g->brojCvorova;
    MinGomila gomila;
    for (int u = 0; u < n; u++) {
        kljuc[u]    = BESKONACNO;
        roditelj[u] = -1;
    }

    kljuc[koren] = 0;

    inicijalizujGomilu(&gomila);
    for (int u = 0; u < n; u++)
        umetniUGomilu(&gomila, u, kljuc[u]);

    while (gomila.velicina > 0) {
        CvorGomile minElem = izvuciMinimum(&gomila);
        int u = minElem.cvor;

        for (CvorListe *sused = g->listaSledecih[u]; sused != NULL; sused = sused->sledeci) {
            int v = sused->dest;
            int t = sused->tezina;

            if (sadrzuGomila(&gomila, v) && t < kljuc[v]) {
                roditelj[v] = u;
                kljuc[v]    = t;
                smanjiKljuc(&gomila, v, t);
            }
        }
    }
}

void ispisiMST(Graf *g, int roditelj[], int kljuc[], int koren) {
    printf("\nGrane minimalnog razapinjuceg stabla (MST):\n");

    int ukupnaTezina = 0;

    for (int v = 0; v < g->brojCvorova; v++) {
        if (v == koren)
            continue;

        int u = roditelj[v];
        printf("%s -|%d|-> %s\n", g->oznaka[u], kljuc[v], g->oznaka[v]);
        ukupnaTezina += kljuc[v];
    }

    printf("Ukupna tezina MST-a: %d\n", ukupnaTezina);
}

int main(void) {

    Graf g;
    inicijalizujGraf(&g);

    dodajCvor(&g, "C");
    dodajCvor(&g, "B");
    dodajCvor(&g, "F");
    dodajCvor(&g, "D");
    dodajCvor(&g, "E");
    dodajCvor(&g, "H");
    dodajCvor(&g, "G");

    dodajGranu(&g, "C", "B",  2);
    dodajGranu(&g, "C", "F",  3);
    dodajGranu(&g, "C", "D",  1);
    dodajGranu(&g, "B", "F",  1);
    dodajGranu(&g, "B", "E",  4);
    dodajGranu(&g, "F", "D", -1);
    dodajGranu(&g, "F", "H",  3);
    dodajGranu(&g, "E", "H",  3);
    dodajGranu(&g, "D", "G",  4);
    dodajGranu(&g, "G", "H",  1);

    int koren = pronadjiCvor(&g, "C");

    int roditelj[MAX_CVOROVA];
    int kljuc[MAX_CVOROVA];

    printf("Polazni cvor: %s\n", g.oznaka[koren]);

    primMST(&g, koren, roditelj, kljuc);

    ispisiMST(&g, roditelj, kljuc, koren);

    oslobodiGraf(&g);

    return 0;
}
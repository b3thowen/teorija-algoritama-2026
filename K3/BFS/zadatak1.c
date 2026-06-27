#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CVOROVA 12

#define BELA 0
#define SIVA  1
#define CRNA  2

typedef struct Cvor {
    int boja;
    int distanca;
    int prethodnik;
} Cvor;

typedef struct CvorListe {
    int indeks;
    struct CvorListe *sled;
} CvorListe;

typedef struct {
    int n;
    CvorListe *adj[MAX_CVOROVA];
    Cvor v[MAX_CVOROVA];
} Graf;

typedef struct CvorReda {
    int indeks;
    struct CvorReda *sled;
} CvorReda;

CvorReda *enqueue(CvorReda *rep, int indeks) {
    CvorReda *novi = (CvorReda *)malloc(sizeof(CvorReda));
    if (novi == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    novi->indeks = indeks;
    novi->sled = NULL;
    if (rep != NULL) {
        rep->sled = novi;
    }
    return novi;
}

int dequeue(CvorReda **glava, CvorReda **rep) {
    CvorReda *tmp = *glava;
    int indeks = tmp->indeks;
    *glava = tmp->sled;
    if (*glava == NULL) {
        *rep = NULL;
    }
    free(tmp);
    return indeks;
}

void initGraf(Graf *g, int n) {
    int i;
    g->n = n;
    for (i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
}

void dodajGranu(Graf *g, int src, int dest, char oznake[]) {
    CvorListe *novi = (CvorListe *)malloc(sizeof(CvorListe));
    if (novi == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    novi->indeks = dest;
    novi->sled = NULL;

    if (g->adj[src] == NULL || oznake[dest] < oznake[g->adj[src]->indeks]) {
        novi->sled = g->adj[src];
        g->adj[src] = novi;
    } else {
        CvorListe *tmp = g->adj[src];
        while (tmp->sled != NULL && oznake[tmp->sled->indeks] < oznake[dest]) {
            tmp = tmp->sled;
        }
        novi->sled = tmp->sled;
        tmp->sled = novi;
    }
}

int indeksOznake(char oznaka, char oznake[]) {
    int i;
    for (i = 0; i < MAX_CVOROVA; i++) {
        if (oznake[i] == oznaka) {
            return i;
        }
    }
    return -1;
}

void bfs(Graf *g, int s) {
    int i, u, v;
    CvorReda *glava = NULL;
    CvorReda *rep = NULL;
    CvorListe *sused;

    for (i = 0; i < g->n; i++) {
        g->v[i].boja = BELA;
        g->v[i].distanca = -1;
        g->v[i].prethodnik = -1;
    }

    g->v[s].boja = SIVA;
    g->v[s].distanca = 0;
    g->v[s].prethodnik = -1;

    rep = enqueue(rep, s);
    if (glava == NULL) {
        glava = rep;
    }

    while (glava != NULL) {
        u = dequeue(&glava, &rep);

        sused = g->adj[u];
        while (sused != NULL) {
            v = sused->indeks;

            if (g->v[v].boja == BELA) {
                g->v[v].boja = SIVA;
                g->v[v].distanca = g->v[u].distanca + 1;
                g->v[v].prethodnik = u;

                CvorReda *noviRed = enqueue(rep, v);
                if (glava == NULL) {
                    glava = noviRed;
                }
                rep = noviRed;
            }

            sused = sused->sled;
        }

        g->v[u].boja = CRNA;
    }
}

void ispisRezultata(Graf *g, char oznake[]) {
    int i;
    printf("BFS obilazak od cvora 'S':\n");
    for (i = 0; i < g->n; i++) {
        if (g->v[i].distanca == -1) {
            printf("%c - d: INF p: NIL\n", oznake[i]);
        } else if (g->v[i].prethodnik == -1) {
            printf("%c - d: %d p: NIL\n", oznake[i], g->v[i].distanca);
        } else {
            printf("%c - d: %d p: %c\n",
                   oznake[i],
                   g->v[i].distanca,
                   oznake[g->v[i].prethodnik]);
        }
    }
}

int main()
{
    char oznake[MAX_CVOROVA] = {'S', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K'};

    Graf g;
    int src;

    initGraf(&g, MAX_CVOROVA);

    dodajGranu(&g, indeksOznake('S', oznake), indeksOznake('A', oznake), oznake);
    dodajGranu(&g, indeksOznake('S', oznake), indeksOznake('B', oznake), oznake);

    dodajGranu(&g, indeksOznake('A', oznake), indeksOznake('C', oznake), oznake);

    dodajGranu(&g, indeksOznake('C', oznake), indeksOznake('A', oznake), oznake);
    dodajGranu(&g, indeksOznake('C', oznake), indeksOznake('S', oznake), oznake);
    dodajGranu(&g, indeksOznake('C', oznake), indeksOznake('D', oznake), oznake);
    dodajGranu(&g, indeksOznake('C', oznake), indeksOznake('E', oznake), oznake);

    dodajGranu(&g, indeksOznake('E', oznake), indeksOznake('F', oznake), oznake);

    dodajGranu(&g, indeksOznake('F', oznake), indeksOznake('D', oznake), oznake);
    dodajGranu(&g, indeksOznake('F', oznake), indeksOznake('H', oznake), oznake);

    dodajGranu(&g, indeksOznake('G', oznake), indeksOznake('D', oznake), oznake);

    dodajGranu(&g, indeksOznake('H', oznake), indeksOznake('E', oznake), oznake);

    dodajGranu(&g, indeksOznake('K', oznake), indeksOznake('I', oznake), oznake);
    dodajGranu(&g, indeksOznake('K', oznake), indeksOznake('J', oznake), oznake);

    src = indeksOznake('S', oznake);

    bfs(&g, src);

    ispisRezultata(&g, oznake);

    for (int i = 0; i < g.n; i++) {
        CvorListe *tek = g.adj[i];
        while (tek != NULL) {
            CvorListe *tmp = tek->sled;
            free(tek);
            tek = tmp;
        }
    }

    return 0;
}
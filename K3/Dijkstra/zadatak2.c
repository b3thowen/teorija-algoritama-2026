#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define BROJ_CVOROVA 10

typedef struct CvorListeSusedstva
{
    int dest;
    int tezina;
    struct CvorListeSusedstva *sled;
} CvorListeSusedstva;

typedef struct ListaSusedstva
{
    struct CvorListeSusedstva *glava;
} ListaSusedstva;

typedef struct Graf
{
    int V;
    struct ListaSusedstva *niz;
} Graf;

typedef struct CvorHeapa
{
    int v;
    int dist;
} CvorHeapa;

typedef struct MinHeap
{
    int velicina;
    int kapacitet;
    int *poz;
    struct CvorHeapa **niz;
} MinHeap;

CvorListeSusedstva *noviCvorListeSusedstva(int dest, int tezina)
{
    CvorListeSusedstva *noviCvor = (CvorListeSusedstva *)malloc(sizeof(CvorListeSusedstva));
    if (noviCvor == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    noviCvor->dest   = dest;
    noviCvor->tezina = tezina;
    noviCvor->sled   = NULL;
    return noviCvor;
}

Graf *kreirajGraf(int V)
{
    Graf *graf = (Graf *)malloc(sizeof(Graf));
    if (graf == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    graf->V   = V;
    graf->niz = (ListaSusedstva *)malloc(V * sizeof(ListaSusedstva));
    if (graf->niz == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }

    int i;
    for (i = 0; i < V; ++i)
        graf->niz[i].glava = NULL;

    return graf;
}

void dodajGranu(Graf *graf, int src, int dest, int tezina)
{
    CvorListeSusedstva *noviCvor = noviCvorListeSusedstva(dest, tezina);
    noviCvor->sled = graf->niz[src].glava;
    graf->niz[src].glava = noviCvor;

    noviCvor = noviCvorListeSusedstva(src, tezina);
    noviCvor->sled = graf->niz[dest].glava;
    graf->niz[dest].glava = noviCvor;
}

void oslobodiGraf(Graf *graf)
{
    for (int i = 0; i < graf->V; i++)
    {
        CvorListeSusedstva *tren = graf->niz[i].glava;
        while (tren)
        {
            CvorListeSusedstva *tmp = tren;
            tren = tren->sled;
            free(tmp);
        }
    }
    free(graf->niz);
    free(graf);
}

CvorHeapa *noviCvorHeapa(int v, int dist)
{
    CvorHeapa *cvor = (CvorHeapa *)malloc(sizeof(CvorHeapa));
    if (cvor == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    cvor->v    = v;
    cvor->dist = dist;
    return cvor;
}

MinHeap *kreirajMinHeap(int kapacitet)
{
    MinHeap *minHeap   = (MinHeap *)malloc(sizeof(MinHeap));
    if (minHeap == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    minHeap->poz       = (int *)malloc(kapacitet * sizeof(int));
    if (minHeap->poz == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    minHeap->velicina  = 0;
    minHeap->kapacitet = kapacitet;
    minHeap->niz       = (CvorHeapa **)malloc(kapacitet * sizeof(CvorHeapa *));
    if (minHeap->niz == NULL) { fprintf(stderr, "malloc greska\n"); exit(1); }
    return minHeap;
}

void zameniCvoroveHeapa(MinHeap *minHeap, int i, int j)
{
    CvorHeapa *tmp   = minHeap->niz[i];
    minHeap->niz[i]  = minHeap->niz[j];
    minHeap->niz[j]  = tmp;

    minHeap->poz[minHeap->niz[i]->v] = i;
    minHeap->poz[minHeap->niz[j]->v] = j;
}

void heapify(MinHeap *minHeap, int idx)
{
    int najmanji = idx;
    int levo     = 2 * idx + 1;
    int desno    = 2 * idx + 2;

    if (levo < minHeap->velicina &&
        minHeap->niz[levo]->dist < minHeap->niz[najmanji]->dist)
        najmanji = levo;

    if (desno < minHeap->velicina &&
        minHeap->niz[desno]->dist < minHeap->niz[najmanji]->dist)
        najmanji = desno;

    if (najmanji != idx)
    {
        zameniCvoroveHeapa(minHeap, najmanji, idx);
        heapify(minHeap, najmanji);
    }
}

int prazan(MinHeap *minHeap)
{
    return minHeap->velicina == 0;
}

CvorHeapa *izvuciMin(MinHeap *minHeap)
{
    if (prazan(minHeap))
        return NULL;

    CvorHeapa *koren = minHeap->niz[0];

    CvorHeapa *poslednji   = minHeap->niz[minHeap->velicina - 1];
    minHeap->niz[0]        = poslednji;

    minHeap->poz[koren->v]     = minHeap->velicina - 1;
    minHeap->poz[poslednji->v] = 0;

    --minHeap->velicina;
    heapify(minHeap, 0);

    return koren;
}

void smanjiKljuc(MinHeap *minHeap, int v, int dist)
{
    int i = minHeap->poz[v];
    minHeap->niz[i]->dist = dist;

    while (i > 0 &&
           minHeap->niz[i]->dist < minHeap->niz[(i - 1) / 2]->dist)
    {
        zameniCvoroveHeapa(minHeap, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

int uHeapu(MinHeap *minHeap, int v)
{
    return minHeap->poz[v] < minHeap->velicina;
}

void stampajPutanju(int roditelj[], int v, const char *oznakeCvorova[])
{
    if (roditelj[v] == -1)
    {
        printf("%s", oznakeCvorova[v]);
        return;
    }
    stampajPutanju(roditelj, roditelj[v], oznakeCvorova);
    printf(" -> %s", oznakeCvorova[v]);
}

void stampajResenje(int dist[], int roditelj[], int V, int src, const char *oznakeCvorova[])
{
    printf("\nNajkrace putanje od cvora %s:\n", oznakeCvorova[src]);

    for (int i = 0; i < V; ++i)
    {
        if (i == src)
            continue;

        if (dist[i] == INT_MAX)
        {
            printf("%s: nedostupan\n", oznakeCvorova[i]);
        }
        else
        {
            stampajPutanju(roditelj, i, oznakeCvorova);
            printf(", cost: %d\n", dist[i]);
        }
    }
}

void dijkstra(Graf *graf, int src, const char *oznakeCvorova[])
{
    int V = graf->V;

    int dist[V];
    int roditelj[V];

    MinHeap *minHeap = kreirajMinHeap(V);

    for (int v = 0; v < V; ++v)
    {
        dist[v]         = INT_MAX;
        roditelj[v]     = -1;
        minHeap->niz[v] = noviCvorHeapa(v, dist[v]);
        minHeap->poz[v] = v;
    }

    dist[src] = 0;
    smanjiKljuc(minHeap, src, dist[src]);

    minHeap->velicina = V;

    while (!prazan(minHeap))
    {
        CvorHeapa *minCvor = izvuciMin(minHeap);
        int u = minCvor->v;
        free(minCvor);

        CvorListeSusedstva *susedCvor = graf->niz[u].glava;
        while (susedCvor != NULL)
        {
            int v      = susedCvor->dest;
            int tezina = susedCvor->tezina;

            if (uHeapu(minHeap, v) &&
                dist[u] != INT_MAX &&
                dist[u] + tezina < dist[v])
            {
                dist[v]     = dist[u] + tezina;
                roditelj[v] = u;
                smanjiKljuc(minHeap, v, dist[v]);
            }

            susedCvor = susedCvor->sled;
        }
    }

    stampajResenje(dist, roditelj, V, src, oznakeCvorova);

    free(minHeap->poz);
    free(minHeap->niz);
    free(minHeap);
}

int main()
{
    const char *oznakeCvorova[] = {"S", "A", "B", "C", "D", "E", "F", "G", "H", "I"};

    int V = BROJ_CVOROVA;
    Graf *graf = kreirajGraf(V);

    dodajGranu(graf, 0, 2, 5);
    dodajGranu(graf, 0, 1, 1);
    dodajGranu(graf, 2, 3, 1);
    dodajGranu(graf, 2, 5, 6);
    dodajGranu(graf, 3, 4, 4);
    dodajGranu(graf, 3, 7, 2);
    dodajGranu(graf, 3, 5, 3);
    dodajGranu(graf, 1, 4, 2);
    dodajGranu(graf, 5, 8, 2);
    dodajGranu(graf, 6, 8, 4);
    dodajGranu(graf, 6, 9, 1);
    dodajGranu(graf, 7, 9, 5);
    dodajGranu(graf, 8, 9, 1);

    dijkstra(graf, 0, oznakeCvorova);

    oslobodiGraf(graf);

    return 0;
}
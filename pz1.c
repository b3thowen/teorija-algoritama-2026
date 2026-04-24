#include <string.h>
#include "utils.h"

#define TEST_SORT_EL 100
#define TEST_SORT_ITERS 20
#define TEST_TIME_MIN_EL 10
#define TEST_TIME_MAX_EL 100000
#define TEST_TIME_ITERS 10

void selectionSort(int *arr, int n);
void mergeSort(int *arr, int n);
void hybridSort(int *arr, int n);

int main()
{
    srand(time(0));

    void (*all_sorts[])(int *, int) = {selectionSort, mergeSort, hybridSort};
    const char *names[] = {"Selection Sort", "Merge Sort", "Hybrid Sort"};

    const char *valid_names[3];

    int num_sorts = sizeof(all_sorts) / sizeof(all_sorts[0]);

    void (*valid_sorts[3])(int *, int);
    int valid_count = 0;

    for (int i = 0; i < num_sorts; i++)
    {
        printf("Testing %s:\n", names[i]);
        int res = test_sort(all_sorts[i], TEST_SORT_EL, TEST_SORT_ITERS);

        if (res == 1)
        {
            fprintf(stderr, "Fatal error: Memory allocation failed during %s.\n", names[i]);
            return 1;
        }
        else if (res == 0)
        {
            valid_sorts[valid_count] = all_sorts[i];
            valid_names[valid_count++] = names[i];
        }
        else if (res == 2)
        {
            printf("%s failed the test and will be excluded from further testing.\n", names[i]);
        }
    }

    if (valid_count > 0)
    {
        printf("\nRunning multiple tests on working algorithms (%d total):\n", valid_count);
        test_multiple_sorts(valid_sorts, valid_names, valid_count, TEST_TIME_MIN_EL, TEST_TIME_MAX_EL, TEST_TIME_ITERS);
    }
    else
    {
        printf("\nNo algorithms passed the initial tests. Skipping multiple sort testing.\n");
    }

    return 0;
}

void selectionSort(int *niz, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int min_idx = i;
        for (int j = i; j < n; j++)
        {
            if (niz[j] < niz[min_idx])
            {
                min_idx = j;
            }
        }
        int temp = niz[i];
        niz[i] = niz[min_idx];
        niz[min_idx] = temp;
    }
}

void merge(int *niz, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    int i, j, k;

    for (i = 0; i < n1; i++)
        L[i] = niz[l + i];
    for (j = 0; j < n2; j++)
        R[j] = niz[m + 1 + j];

    i = 0; j = 0; k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            niz[k++] = L[i++];
        else
            niz[k++] = R[j++];
    }

    while (i < n1)
        niz[k++] = L[i++];

    while (j < n2)
        niz[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSortRecursive(int *niz, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortRecursive(niz, l, m);
        mergeSortRecursive(niz, m + 1, r);
        merge(niz, l, m, r);
    }
}

void mergeSort(int *niz, int n) {
    if (n <= 1) return;
    mergeSortRecursive(niz, 0, n - 1);
}

void hybridSortRecursive(int* niz, int l, int r) {
    if ((r - l + 1) <= 15) {
        selectionSort(niz + l, r - l + 1);
    } else {
        if (l < r) {
            int m = l + (r - l) / 2;
            hybridSortRecursive(niz, l, m);
            hybridSortRecursive(niz, m + 1, r);
            merge(niz, l, m, r);
        }
    }
}

void hybridSort(int* niz, int n) {
    hybridSortRecursive(niz, 0, n - 1);
}

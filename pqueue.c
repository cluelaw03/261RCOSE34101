#include "pqueue.h"

static void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }

void pq_init(PQueue *pq, pq_cmp_fn cmp) {
    pq->size = 0;
    pq->cmp  = cmp;
}

void pq_push(PQueue *pq, int idx) {
    int i = pq->size++;
    pq->heap[i] = idx;
    /* 위로 올리기(up-heap) */
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (pq->cmp(pq->heap[i], pq->heap[parent]) < 0) {
            swap(&pq->heap[i], &pq->heap[parent]);
            i = parent;
        } else break;
    }
}

int pq_pop(PQueue *pq) {
    if (pq->size == 0) return -1;
    int top = pq->heap[0];
    pq->heap[0] = pq->heap[--pq->size];
    /* 아래로 내리기(down-heap) */
    int i = 0;
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, best = i;
        if (l < pq->size && pq->cmp(pq->heap[l], pq->heap[best]) < 0) best = l;
        if (r < pq->size && pq->cmp(pq->heap[r], pq->heap[best]) < 0) best = r;
        if (best == i) break;
        swap(&pq->heap[i], &pq->heap[best]);
        i = best;
    }
    return top;
}

int pq_peek(PQueue *pq) { return pq->size ? pq->heap[0] : -1; }
int pq_empty(PQueue *pq) { return pq->size == 0; }

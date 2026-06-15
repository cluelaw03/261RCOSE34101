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

void q_init(Queue *q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}
 
void q_push(Queue *q, int idx) {
    if (q->size >= cfg.Max_Process) return;   /* 가득 차면 무시(안전장치) */
    q->items[q->tail] = idx;
    q->tail = (q->tail + 1) % cfg.Max_Process;
    q->size++;
}
 
int q_pop(Queue *q) {
    if (q->size == 0) return -1;          /* 비었으면 -1 */
    int front = q->items[q->head];
    q->head = (q->head + 1) % cfg.Max_Process;
    q->size--;
    return front;
}
 
int q_peek(Queue *q) { return q->size ? q->items[q->head] : -1; }
int q_empty(Queue *q) { return q->size == 0; }

#ifndef PQUEUE_H
#define PQUEUE_H

#include "types.h"

/* 두 프로세스 인덱스 a, b 를 비교.
 * 반환값 < 0 이면 a 가 b 보다 먼저 나와야 함(우선순위 높음). */
typedef int (*pq_cmp_fn)(int a, int b);

typedef struct {
    int heap[MAX_PROCESS];  /* 프로세스 인덱스를 저장하는 힙 */
    int       size;
    pq_cmp_fn cmp;
} PQueue;

void pq_init(PQueue *pq, pq_cmp_fn cmp);
void pq_push(PQueue *pq, int idx);
int  pq_pop(PQueue *pq);          /* 최우선 인덱스 반환, 비었으면 -1 */
int  pq_peek(PQueue *pq);   /* 꺼내지 않고 확인, 비었으면 -1 */
int  pq_empty(PQueue *pq);

typedef struct {
    int items[MAX_PROCESS];   /* 프로세스 인덱스를 저장하는 원형 버퍼 */
    int head;                 /* 다음에 꺼낼(front) 위치 */
    int tail;                 /* 다음에 넣을(back) 위치 */
    int size;                 /* 현재 들어있는 원소 수 */
} Queue;

void q_init(Queue *q);
void q_push(Queue *q, int idx);   /* 뒤(back)에 삽입 */
int  q_pop(Queue *q);             /* 앞(front)에서 꺼냄, 비었으면 -1 */
int  q_peek(Queue *q);            /* 꺼내지 않고 앞(front) 확인, 비었으면 -1 */
int  q_empty(Queue *q);


#endif /* PQUEUE_H */

#ifndef UTILS_H
#define UTILS_H

static inline int imax(int a, int b){ return a > b ? a : b; }

int rand_range(int lo, int hi);
void reset_test(void);

int ready_empty(void);

/* 큐 보조 함수 */
void enqueue_ready(int idx);
int  dequeue_ready(void);       /* 우선순위 큐에서 최우선 프로세스 꺼냄, 비었으면 -1 */
void enqueue_wait(int idx);
int dequeue_wait(void);       /* Waiting Queue 에서 가장 오래된 프로세스 꺼냄, 비었으면 -1 */

#endif /* UTILS_H */

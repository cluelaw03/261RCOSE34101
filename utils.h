#ifndef UTILS_H
#define UTILS_H

int rand_range(int lo, int hi);
void reset_test(void);

/* 큐 보조 함수 */
void enqueue_ready(int idx);
int  dequeue_ready(void);       /* 우선순위 큐에서 최우선 프로세스 꺼냄, 비었으면 -1 */
void enqueue_wait(int idx);
void remove_wait_at(int pos);

#endif /* UTILS_H */

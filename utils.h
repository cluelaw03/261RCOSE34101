#ifndef UTILS_H
#define UTILS_H

int rand_range(int lo, int hi);
void reset_test(void);

/* 큐 보조 함수 */
void enqueue_ready(int idx);
void remove_ready_at(int pos);
void enqueue_wait(int idx);
void remove_wait_at(int pos);

#endif /* UTILS_H */
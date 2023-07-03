#ifndef _SEM_H
#define _SEM_H

#include <linux/sched.h>

#define QUE_LEN 16
#define SEM_FAILED  (void*) 0

struct semaphore_queue {
	int front;
	int rear;
	struct task_struct *wait_tasks[QUE_LEN];
};
typedef struct semaphore_queue sem_queue;

struct semaphore_t {
    int value;
    int occupied;
    char name[16];
    struct semaphore_queue wait_queue;
};
typedef struct semaphore_t sem_t;

#endif
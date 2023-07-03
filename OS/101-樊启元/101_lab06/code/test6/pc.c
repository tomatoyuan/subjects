#define __LIBRARY__
#include <unistd.h>
#include <linux/sem.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t *, sem);
_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, const char *, name);

#define NUMBER 300 /* 打出数字总数 */
#define CUSTOMERS 5 /* 消费者进程数 */
#define BUFSIZE 10 /* 缓冲区大小 */

sem_t *empty, *full, *mutex;
int fd; /* 文件描述符 */

int main()
{
    int i, j, k;
    int  data;
    pid_t p;
    int buf_out = 0; /* 从缓冲区读取上次读取后记录的位置 */
    int buf_in = 0; /* 写入缓冲区的位置 */

    /* 打开信号量 */
    if ( (mutex = sem_open("carmutex", 1)) == SEM_FAILED ) {
        perror("sem_open(mutex) error!\n");
        return -1;
    }
    if ( (empty = sem_open("carempty", BUFSIZE)) == SEM_FAILED ) {
        perror("sem_open(empty) error!\n");
        return -1;
    }
    if ( (full = sem_open("carfull", 0)) == SEM_FAILED ) {
        perror("sem_open(full) error!\n");
        return -1; 
    }

    fd = open("buffer.dat", O_CREAT | O_RDWR | O_TRUNC, 0666);
    /* 将读取指针移动到缓冲区的大小 10*4 个字节之后 */
    lseek(fd, BUFSIZE * sizeof(int), SEEK_SET);
    /* 写入 buf_out 记录下次读取的起始位置 */
    write(fd, (char *)&buf_out, sizeof(int));

    /* 生产者进程 */
    if ( (p = fork()) == 0 ) {
        /* 只有一个生产者，需要在 BUF 的空间中循环生成 NUMBER 个 */
        for (i = 0; i < NUMBER; i++) {
            sem_wait(empty);
            sem_wait(mutex);
            /* 写入一个字符 */
            lseek(fd, buf_in * sizeof(int), SEEK_SET);
            write(fd, (char *)&i, sizeof(int));
            /* 生产资源 */
            printf("pid=%d(producer):\t %d\n", getpid(), i);
            fflush(stdout); 
            buf_in = (buf_in + 1) % BUFSIZE;

            sem_post(mutex);
            sem_post(full);
        }
        return 0;
    } else if (p < 0) {
        perror("Fail to fork!\n");
        return -1;
    }
    
    /* 消费者进程 */
    for (j = 0; j < CUSTOMERS; j++) {
        if ( (p = fork()) == 0 ) {
            int avg_num = NUMBER / CUSTOMERS;
            for (k = 0; k < avg_num; k++) {
                sem_wait(full);
                sem_wait(mutex);
                /* 去 BUF 后的第一个位置，获得读取位置 */
                lseek(fd, BUFSIZE * sizeof(int), SEEK_SET);
                read(fd, (char *)&buf_out, sizeof(int));

                /* 读取数据 */
                lseek(fd, buf_out * sizeof(int), SEEK_SET);
                read(fd, (char *)&data, sizeof(int));

                /* 更新并写入读取位置 */
                buf_out = (buf_out + 1) % BUFSIZE;
                lseek(fd, BUFSIZE * sizeof(int), SEEK_SET);
                write(fd, (char *)&buf_out, sizeof(int));

                /* 消费资源 */
                printf("pid=%d(customer):\t %d\n", getpid(), data);
                fflush(stdout); /* 将输出强制写到终端，保持各个进程数据同步 */

                sem_post(mutex);
                sem_post(empty);
            }
            return 0;
        } else if (p < 0) {
            perror("Fail to fork!\n");
            return -1;
        }
    }

    wait(NULL);
    /* 释放信号量 */
    sem_unlink("carfull");
    sem_unlink("carempty");
    sem_unlink("carmutex");
    /* 释放资源 */
    close(fd);

    return 0;
}
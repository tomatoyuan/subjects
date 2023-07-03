#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>

#define HZ 100

void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char *argv[])
{
    /* 有一个父进程，创建 10 个子进程 */
    pid_t pid;
    int i = 0;
    for (i = 0; i < 10; i++) {
        pid = fork();
        if (pid == 0) {
            /* 子进程不用创建进程，直接执行后续指令即可 */ 
            break;
        }
        /* 其余情况继续创建进程直到创建出 10 个子进程为止 */
    }

    if (pid > 0) {
        /* 父进程执行回收子进程的操作 */
        int ret = 0;
        while (1) {
            printf("parent_pid = %d\n", getpid());
            
            ret = wait(NULL);
            /* 没有可回收的子进程，即所有子进程全被回收了，则退出 */
            if (ret == -1) break;

            printf("child process die, pid = %d\n", ret);
        }

    } else if (pid == 0) {
        /* 子进程执行任务 */
        pid_t child_pid = getpid();
        printf("child_pid = %d\n", child_pid);
        cpuio_bound(10, child_pid % 10, 10 - (child_pid % 10));
    }

    return 0;
}

void cpuio_bound(int last, int cpu_time, int io_time)
{
    struct tms start_time, current_time;
    clock_t utime, stime;
    int sleep_time;

    while (last > 0) {

        /* CPU time */
        times(&start_time);
        do {
            times(&current_time);
            utime = current_time.tms_utime - start_time.tms_utime;
            stime = current_time.tms_stime - start_time.tms_stime;
        } while ( (utime + stime) / HZ < cpu_time );
        last -= cpu_time;

        if (last < 0) break;

        /* IO time */
        sleep_time = 0;
        while (sleep_time < io_time) {
            sleep(1);
            sleep_time++;
        }
        last -= sleep_time;
    }
}


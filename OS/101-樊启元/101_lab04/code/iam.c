# define __LIBRARY__
# include <unistd.h>
# include <stdio.h>
# include <errno.h>

_syscall1(int, iam, const char*, name);

int main(int argc, char** argv){
    int wlen = 0;
    if (argc < 1 ){
        printf("not enougth arguments!\n");
    }
    wlen = iam(argv[1]);
    return wlen;
}

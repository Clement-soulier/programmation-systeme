//don't work
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define path "/tmp/pipe"
int pipe_opened;

void sigint_handler(int sig){
    if(close(pipe_opened) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }
    if(unlink(path) == -1){
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    exit(0);
}

int main(void){
    int pipefd[2];
    if(mkfifo(path, 0666) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pipe_opened = open(path, O_RDONLY | O_NONBLOCK);
    if(pipe_opened == -1){
        if(errno != EEXIST){
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    while(1){
        sleep(1);
    }
}

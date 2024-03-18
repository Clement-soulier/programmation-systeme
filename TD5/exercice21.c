#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    int *pipefd = malloc(2 * sizeof(int));
    pid_t pid_ls, pid_sed;

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_ls = fork();
    if(pid_ls == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(pid_ls == 0){
        if(dup2(pipefd[1], STDOUT_FILENO) == -1){
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        if(close(pipefd[0]) == -1){
            perror("close");
            exit(EXIT_FAILURE);
        }
        if(execlp("ls", "ls", "-l", NULL) == -1){
            perror("execlp" "ls");
            exit(EXIT_FAILURE);
        }
        exit(0);
    }

    pid_sed = fork();
    if(pid_sed == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(pid_sed == 0){
        if(dup2(pipefd[0], STDIN_FILENO) == -1){
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        if(close(pipefd[1]) == -1){
            perror("close");
            exit(EXIT_FAILURE);
        }
        if(execlp("sed", "sed", "s/\\.c$/.COUCOU/", NULL) == -1){
            perror("execlp" "sed");
            exit(EXIT_FAILURE);
        }
        exit(0);
    }

    if(close(pipefd[0]) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }

    if(close(pipefd[1]) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }

    waitpid(pid_ls, NULL, 0);
    waitpid(pid_sed, NULL, 0);

    free(pipefd);

    return 0;
}

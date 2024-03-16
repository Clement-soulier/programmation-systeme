#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

extern int errno;


void comportement1(){
    while(1){
        sleep(1);
    }
}

void handler_comportement2(int sig){
    return;
}

void comportement2(){
    struct sigaction act;
    act.sa_handler= handler_comportement2;
    sigaction(SIGUSR1, &act, NULL);
    while(1){
        sleep(1);
    }
}

void handler_comportement3(int sig){
    exit(0);
}

void comportement3(){
    struct sigaction act;
    act.sa_handler= handler_comportement3;
    sigaction(SIGUSR1, &act, NULL);
    while(1){
        sleep(1);
    }
}

void handler_comportement4(int sig){
    exit(1);
}

void comportement4(){
    struct sigaction act;
    act.sa_handler= handler_comportement4;
    sigaction(SIGUSR1, &act, NULL);
    while(1){
        sleep(1);
    }
}


int main(void){
    int *proc_tab = malloc(10 * sizeof(pid_t));
    pid_t pid;
    int status;
    int behaviour_id;
    for(int i = 0; i < 10; i++){
        errno = 0;
        pid = fork();
        proc_tab[i] = pid;
        if(pid == 1){
            fprintf(stderr, "Erreur lors de la création d'un processus enfant: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            pid = getpid();
            srand(pid);
            behaviour_id = rand() % 4;
            printf("Le processus %i a le comportement %i\n", pid, behaviour_id+1);
            switch (behaviour_id) {
                case 0:
                    comportement1();
                    break;
                case 1:
                    comportement2();
                    break;
                case 2:
                    comportement3();
                    break;
                case 3:
                    comportement4();
                    break;
                default:
                    break;
            }
        }
    }
    sleep(1);
    for(int i = 0; i < 10; i++){
        kill(proc_tab[i], SIGUSR1);
        sleep(1);
        pid = waitpid(proc_tab[i], &status, WNOHANG);
        if (WIFSIGNALED(status)){
            printf("Le processus %i avait le comportement 1\n", proc_tab[i]);
        } else if(pid == 0){
            printf("Le processus %i avait le comportement 2\n", proc_tab[i]);
        }
        else if(WEXITSTATUS(status) == 1){
            printf("Le processus %i avait le comportement 4\n", proc_tab[i]);
        }
        else if(WEXITSTATUS(status) == 0){
            printf("Le processus %i avait le comportement 3\n", proc_tab[i]);
        }
    }
    free(proc_tab);
}

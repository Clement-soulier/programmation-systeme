#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int nombre_signal = 0;

void increment_sig_count(int sig){
    nombre_signal++;
}

int main(void){
    int *tab = malloc(10 * sizeof(int));
    int *check = malloc(10 * sizeof(int));
    int *is_finished = malloc(10 * sizeof(int));
    pid_t pid;
    int status;
    for(int i = 0; i < 10; i++){
        check[i] = 1;
        is_finished[i] = 0;
        errno = 0;
        tab[i] = fork();
        if(tab[i] == -1){
            fprintf(stderr, "erreur lors de la création du processus %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if(tab[i] == 0){
            struct sigaction act;
            act.sa_handler = increment_sig_count;
            sigaction(SIGTERM, &act, NULL);
            pid = getpid();
            srand(pid);
            int num = 1 + rand() % 10;
            printf("PID: %i, num: %i\n", pid, num);
            while(1){
                if(nombre_signal == num){
                    exit(0);
                }
            }
        }
    }
    sleep(2);
    int compteur = 0;
    for(int j = 0; j < 10; j++){
        for(int i = 0; i < 10; i++){
            if(!is_finished[i]){
                errno = 0;
                pid = waitpid(tab[i], &status, WNOHANG);
                if(errno){
                    fprintf(stderr, "Erreur lors de l'attente du processus %i: %s\n", tab[i], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                if(!pid){
                    errno = 0;
                    kill(tab[i], SIGTERM);
                    if(errno){
                        fprintf(stderr, "Erreur lors de l'envoie d'un signal au processus %i: %s", tab[i], strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        compteur++;
        usleep(500);
        for(int i = 0; i < 10; i++){
            pid = waitpid(tab[i], &status, WNOHANG);
            if(pid && check[i]){
                check[i] = 0;
                is_finished[i] = 1;
                printf("le processus %i avait le numéro %i\n", tab[i], compteur);
            }
        }
    }
}

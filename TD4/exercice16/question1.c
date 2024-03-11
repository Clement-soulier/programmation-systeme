#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

extern int errno;

void affiche_tab(int *tab, size_t size){
    printf("[");
    for(unsigned int i = 0; i < size; i++){
        printf("%i", tab[i]);
        if(i != (size - 1)){
            printf(",");
        }
    }
    printf("]\n");
}

int main(void){
    int tab[10] = {9,3,8,7,1,4,2,5,6,10};
    affiche_tab(tab, 10);
    pid_t pid;
    int status;
    for(int i = 0; i < 10; i++){
        errno = 0;
        pid = fork();
        if(pid == -1){
            fprintf(stderr, "%s", strerror(errno));
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            sleep(tab[i]);
            exit(tab[i]);
        }
    }
    for(int i = 0; i < 10; i++){
        wait(&status);
        tab[i] = WEXITSTATUS(status);
    }
    affiche_tab(tab, 10);
    exit(0);
}

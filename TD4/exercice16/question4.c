#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

extern int errno;

size_t name_buffer_size = 255;

int min(int *tab, size_t size){
    int min = tab[0];
    for(unsigned int i = 1; i < size; i++){
        if(min > tab[i]){
            min = tab[i];
        }
    }
    return min;
}

char *filename(pid_t PID){
    char *name = malloc(name_buffer_size * sizeof(char));
    strcpy(name, "");
    strcat(name, "sleepsort");
    strcat(name, ".");
    char *buffer = malloc(6 * sizeof(char));
    sprintf(buffer, "%i", PID);
    strcat(name, buffer);
    free(buffer);
    return name;
}

void enregistrer(pid_t PID, int entier){
    char *name = filename(PID);
    int file = open(name, O_WRONLY | O_CREAT, S_IRWXU);
    if(errno){
        fprintf(stderr, "erreur lors de l'ouverture pour l'enregistrement de %s: %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    write(file, &entier, sizeof(int));
    if(errno){
        fprintf(stderr, "erreur lors de l'écriture pour l'enregistrement de %s: %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        fprintf(stderr, "erreur lors de la fermeture pour l'enregistrement %s: %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    free(name);
}

int lecture(pid_t PID){
    errno = 0;
    char *name = filename(PID);
    int file = open(name, O_RDONLY);
    if(errno){
        fprintf(stderr, "erreur lors de l'ouverture pour la lecture %s: %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    int reponse;
    errno = 0;
    read(file, &reponse, sizeof(int));
    if(errno){
        fprintf(stderr, "erreur lors de la lecture de %s: %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        fprintf(stderr, "erreur lors de la fermeture de %s %s\n", name,strerror(errno));
        exit(EXIT_FAILURE);
    }
    free(name);
    return reponse;
}

void delete(pid_t PID){
    errno = 0;
    char *name = filename(PID);
    unlink(name);
    if(errno){
        fprintf(stderr, "erreur lors de la supression de %s:  %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
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
    int tab[10] = {-16,12,-82,73,200,0,2,5,6,10};
    affiche_tab(tab, 10);
    pid_t pid;
    int status, offset;
    int tab_min = min(tab, 10);
    if(tab_min < 0){
        offset = -tab_min;
    } else{
        offset = 0;
    }
    for(int i = 0; i < 10; i++){
        errno = 0;
        pid = fork();
        if(pid == -1){
            fprintf(stderr, "Erreur lors de la création d'un processu enfant: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            enregistrer(getpid(), tab[i]);
            usleep(((tab[i] + offset) * 700) + 1000);
            exit(0);
        }
    }
    for(int i = 0; i < 10; i++){
        errno = 0;
        pid = wait(&status);
        if(errno){  
            fprintf(stderr, "Erreur lors de l'attente d'un processus enfant: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(WIFEXITED(status)){
            tab[i] = lecture(pid);
            delete(pid);
        }
    }
    affiche_tab(tab, 10);
    exit(0);
}

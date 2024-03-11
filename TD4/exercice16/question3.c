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
}

int lecture(pid_t PID){
    errno = 0;
    int file = open(filename(PID), O_RDONLY);
    if(errno){
        fprintf(stderr, "erreur lors de l'ouverture pour la lecture %s: %s\n", filename(PID), strerror(errno));
        exit(EXIT_FAILURE);
    }
    int reponse;
    errno = 0;
    read(file, &reponse, sizeof(int));
    if(errno){
        fprintf(stderr, "erreur lors de la lecture %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        fprintf(stderr, "erreur lors de la fermeture après la lecture %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return reponse;
}

void delete(pid_t PID){
    errno = 0;
    unlink(filename(PID));
    if(errno){
        fprintf(stderr, "erreur lors de la supression du fichier %s\n", strerror(errno));
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
    int tab[10] = {192,390,82,73,119,430,2,5,6,10};
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
            enregistrer(getpid(), tab[i]);
            sleep(tab[i]);
            exit(0);
        }
    }
    for(int i = 0; i < 10; i++){
        pid = wait(&status);
        if(WIFEXITED(status)){
            tab[i] = lecture(pid);
            delete(pid);
        }
    }
    affiche_tab(tab, 10);
    exit(0);
}

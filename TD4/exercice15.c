#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

//ldd -lcrypto
#include <openssl/md5.h>

//={0} => allthearrayis resettozero(onlyworksforzero!)
char hash[1+ 2*MD5_DIGEST_LENGTH]={0};

char *md5hash(char *str){
    unsigned char md5[MD5_DIGEST_LENGTH]={0};
    MD5(str, strlen(str), md5);
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(hash+ 2*i, "%02x", md5[i]);
    }
    return hash;
}

extern int errno;
size_t name_buffer_size = 255;

char *filename(pid_t PID){
    char *name = malloc(name_buffer_size * sizeof(char));
    strcpy(name, "");
    strcat(name, "found");
    strcat(name, ".");
    char *buffer = malloc(6 * sizeof(char));
    sprintf(buffer, "%i", PID);
    strcat(name, buffer);
    free(buffer);
    return name;
}

void enregistrer(pid_t PID, int entier){
    char *name = filename(PID);
    int file = open(name, O_WRONLY | O_CREAT, S_IRWXO);
    if(errno){
        perror("open");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    write(file, &entier, sizeof(int));
    if(errno){
        perror("write");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        perror("open");
        exit(EXIT_FAILURE);
    }
}

int lecture(pid_t PID){
    errno = 0;
    char *name = filename(PID);
    int file = open(name, O_RDONLY);
    if(errno){
        perror("open");
        exit(EXIT_FAILURE);
    }
    int reponse;
    errno = 0;
    read(file, &reponse, sizeof(int));
    if(errno){
        perror("read");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        perror("close");
        exit(EXIT_FAILURE);
    }
    free(name);
    return reponse;
}

void delete(pid_t PID){
    char *name = filename(PID);
    errno = 0;
    unlink(name);
    if(errno){
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    free(name);
}

int zeros(char *s, int n){
    int nombre_caractere = strlen(s);
    if(nombre_caractere < n){
        return 0;
    }
    int compteur = 0;
    for(int i = 0; i < nombre_caractere; i++){
        if(s[i] == 0){
            compteur++;
        } else {
            break;
        }
    }
    if(compteur == n){
        return 1;
    } else {
        return 0;
    }
}

void bruteforce(int first, long unsigned int step, long unsigned int zero){
    int number = first;
    char *text_int = malloc(6 * sizeof(char));
    while(1){
        sprintf(text_int, "%i", number);
        char *text_hashed = md5hash(text_int);
        if(zeros(text_hashed, zero)){
            pid_t PID = getpid();
            enregistrer(PID, number);
            exit(0);
        }
        number += step;
    }
}

int main(void){
    pid_t pid;
    pid_t pids[10];
    int status;
    for(int i = 0; i < 10; i++){
        errno = 0;
        pid = fork();
        pids[i] = pid;
        if(pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            bruteforce(i, 10, 6);
        }
    }
    while(1){
        for(int i = 0; i < 10; i++){
            errno = 0;
            pid = waitpid(pids[i], &status, WNOHANG);
            if(errno){
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if(WIFEXITED(status)){
                printf("%i\n", lecture(pid));
                delete(pid);
                exit(0);

            }
        }
    }
}

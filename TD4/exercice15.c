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

void enregistrer(pid_t PID, long unsigned int entier){
    char *name = filename(PID);
    int file = open(name, O_WRONLY);
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    write(file, &entier, sizeof(long unsigned int));
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

long unsigned int lecture(pid_t PID){
    errno = 0;
    int file = open(filename(PID), O_RDONLY);
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    long unsigned int reponse;
    errno = 0;
    read(file, &reponse, sizeof(long unsigned int));
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    close(file);
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return reponse;
}

void delete(pid_t PID){
    errno = 0;
    unlink(filename(PID));
    if(errno){
        fprintf(stderr, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int zeros(char *s, long unsigned int n){
    long unsigned int nombre_caractere = strlen(s);
    if(nombre_caractere < n){
        return 0;
    }
    long unsigned int compteur = 0;
    for(long unsigned int i = 0; i < nombre_caractere; i++){
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

void bruteforce(long unsigned int first, long unsigned int step, long unsigned int zero){
    long unsigned int number = first;
    while(1){
        char *text_int = malloc(6 * sizeof(char));
        sprintf(text_int, "%lu", number);
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
    int status;
    for(long unsigned int i = 0; i < 10; i++){
        errno = 0;
        pid = fork();
        if(pid == -1){
            fprintf(stderr, "%s", strerror(errno));
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            bruteforce(i, 10, 6);
        }
    }
    while(1){
        pid = waitpid(0, &status, WNOHANG);
        if(status > 0){
            if(WIFEXITED(status)){
                printf("%lu\n", lecture(pid));
                delete(pid);
                exit(0);

            }
        }
    }
}

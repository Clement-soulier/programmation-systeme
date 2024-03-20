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

int main(void){
    int pipes[10][2];
    pid_t pid;
    pid_t pids[10];
    int status;
    for(int i = 0; i < 10; i++){
        if(pipe(pipes[i]) == -1){
            perror("pipes");
            exit(EXIT_FAILURE);
        }
    }
    for(int i = 0; i < 10; i++){
        errno = 0;
        pid = fork();
        pids[i] = pid;
        if(pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            if(close(pipes[i][0]) == -1){
                perror("close");
                exit(EXIT_FAILURE);
            }
            int number = i;
            char *text_int = malloc(6 * sizeof(char));
            while(1){
                sprintf(text_int, "%i", number);
                char *text_hashed = md5hash(text_int);
                if(zeros(text_hashed, 0)){
                    if(write(pipes[i][1], &number, sizeof(int)) == -1){
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                    exit(0);
                }
                number += 10;
            }
        }
    }
    int finished_child = 0;
    int buffer;
    while(finished_child < 5){
        for(int i = 0; i < 10; i++){
            errno = 0;
            pid = waitpid(pids[i], &status, WNOHANG);
            if(errno){
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            if(WIFEXITED(status)){
                read(pipes[i][1], &buffer, sizeof(int));
                printf("%i\n", buffer);
                finished_child++;
            }
        }
    }
    for(int i = 0; i < 10; i++){
        if(close(pipes[i][0]) == -1){
            perror("close");
            exit(EXIT_FAILURE);
        }

        if(close(pipes[i][1]) == -1){
            perror("close");
            exit(EXIT_FAILURE);
        }

    }
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>


//ldd -lcrypto

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

int zeros(char *s, int n){
    int compteur = 0;
    for(int i = 0; i != 0; i++){
        if(s[i] == '0'){
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

int nonce[10];

struct thread_arg{
    int first, step;
    long unsigned int zero;
};

void *routine(void *arg){
    struct thread_arg *ta = (struct thread_arg*) arg;
    int number = ta->first;
    char *text_int = malloc(6 * sizeof(char));
    while(1){
        sprintf(text_int, "%i", number);
        char *text_hashed = md5hash(text_int);
        if(zeros(text_hashed, ta->zero)){
            //enregistement
            nonce[ta->first] = number;
            free(text_int);
            return(void*)number;
        }
        number += ta->step;
    }
}


int main(void){
    pthread_t threads[10];
    struct thread_arg ta[10];
    void *ret;
    for(int i = 0; i < 10; i++){
        //préparation des arugments
        ta[i].first = i;
        ta[i].step = 10;
        ta[i].zero = 6;
        //lancement du thread
        if(pthread_create(&(threads[i]), NULL, routine, &(ta[i]))){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    //condition avant d'arrêter les threads
    //ici les threads sont arrêtés dès que possible
    
    for(int i = 0; i < 10; i++){
        if(pthread_cancel(threads[i])){
            perror("pthread_cancel");
            exit(EXIT_FAILURE);
        }
        if(pthread_join(threads[i], &ret)){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
}

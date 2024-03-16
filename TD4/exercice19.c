#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

extern int errno;
int sig_int_count = 0;
int is_stop = 1;

void sig_alarm_handler(int sig){
    is_stop = 1;
    sig_int_count = 0;
    printf("Returning to work...\n");
}

void sig_int_handler(int sig){
    is_stop = 0;
    sig_int_count++;
    alarm(2);
    if(sig_int_count < 2){
        printf("\nCtrl+C in less than two seconds to quit\n");
    }
}

int main(void){
    struct sigaction act;
    act.sa_handler = sig_int_handler;
    errno = 0;
    sigaction(SIGINT, &act, NULL);
    if(errno){
        fprintf(stderr, "Erreur lors de l'installation du traitant de signal: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    act.sa_handler = sig_alarm_handler;
    errno = 0;
    sigaction(SIGALRM, &act, NULL);
    if(errno){
        fprintf(stderr, "Erreur lors de l'installation du traitant de signal: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    while(sig_int_count < 2){
        if(is_stop){
            printf("working...\n");
        }
        sleep(1);
    }
    printf("\nGoodbye\n");
    exit(0);
}

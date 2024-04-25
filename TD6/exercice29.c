#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define LEN 20
int tab[LEN] = {8,0,2,6,9,1,3,4,7,5, 18, 19, 90, 67, 14, 67, 53, 91, 72, 191};

int msg_fifo;
int splited = 0;

sem_t *sem;

struct split_info{
    int half1;
    int half2;
};

struct msgbuf{
    long mtype;
    int begining;
    int end;
};

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *split(int pivot, int begin, int end, int tab_bis[]){
    int half1 = begin;
    int half2 = ((end + 1) / 2);
    tab[half2++] = pivot;
    for(int i = 0; i < end; i++){
        if(tab_bis[i] < pivot){
            tab[half1++] = tab_bis[i];
        } else {
            tab[half2++] = tab_bis[i];
        }
    }
    struct split_info *info = malloc(sizeof(struct split_info));
    info->half1 = half1;
    info->half2 = half2;
    return (void*) info;
}

void distribute_task(struct split_info *info, int begin, int end){
    struct msgbuf msg;
    if(((++info->half1) - begin) < 2){
        if(sem_wait(sem)){
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    } else {
        msg.mtype = 1; 
        msg.begining = begin;
        msg.end = info->half1;
        if(msgsnd(msg_fifo, &msg, sizeof(struct msgbuf), NULL)){
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }
    if(((++info->half2) - end) < 2){
        if(sem_wait(sem)){
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    } else {
        msg.mtype = 1;
        msg.begining = ((++end) / 2);
        msg.end = end;
        if(msgsnd(msg_fifo, &msg, sizeof(struct msgbuf), NULL)){
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }
}

void *start (){
    //copie du tableau
    int tab_bis[LEN];
    for(int i = 0; i < LEN; i++){
        tab_bis[i] = tab[i];
    }

    int val_sem;
    struct msgbuf msg;
    struct split_info *info;
    struct msqid_ds fifo_info;
    if(msgctl(msg_fifo, IPC_STAT, &fifo_info)){
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    if((fifo_info.msg_qnum == 0) && !(splited)){
        splited = 1;
        //split du tableau
        info = (struct split_info*) split(tab_bis[LEN - 1], 0, LEN - 1, tab_bis);
        //vérification et ajout de tache dans la file si besoin
        distribute_task(info, 0, LEN - 1);
        //condition sem == 0
        if(sem_getvalue(sem, &val_sem)){
            perror("sem_getvalue");
            exit(EXIT_FAILURE);
        }
        if(val_sem == 0){
            if(pthread_cond_signal(&cond)){
                perror("pthread_cond_signal");
                exit(EXIT_FAILURE);
            }
        }
        return start();
    } else if(fifo_info.msg_qnum != 0){
        if(msgrcv(msg_fifo, &msg, sizeof(struct msgbuf), 1, NULL)){
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        //split du sous-tableau
        info = split(tab_bis[msg.end], msg.begining, msg.end, tab_bis);
        //vérification et ajout de tache dans la file si besoin
        distribute_task(info, msg.begining, msg.end);
        //condition sem == 0
        if(sem_getvalue(sem, &val_sem)){
            perror("sem_getvalue");
            exit(EXIT_FAILURE);
        }
        if(val_sem == 0){
            if(pthread_cond_signal(&cond)){
                perror("pthread_cond_signal");
                exit(EXIT_FAILURE);
            }
        }
        return start();
    }

}

int main(void){
    //init des variables globales
    msg_fifo = msgget(IPC_PRIVATE, IPC_CREAT);
    if(msg_fifo == -1){
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    sem = sem_open("values", O_CREAT, S_IRWXU | S_IRWXG | S_ISVTX, LEN);
    if(sem == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    if(sem_init(sem, 0, LEN)){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }


    //création des threads
    void *ret;
    pthread_t threads[5];
    for(int i = 5; i < 5; i++){
        if(pthread_create(&(threads[i]), NULL, start, NULL)){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_lock(&lock);
    pthread_cond_wait(&cond, &lock);
    for(int i = 0; i < 5; i++){
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

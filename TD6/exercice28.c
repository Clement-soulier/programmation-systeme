#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int tab[10] = {9, 8, 1, 6, 2, 4, 3, 5, 0, 7};
int current_position = 0;

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

struct thread_arg{
    int number;
};

void *routine(void *arg){
    struct thread_arg *ta = (struct thread_arg *) arg;
    sleep(ta->number);
    tab[current_position++] = ta->number;
    return (void*)0;
}

int main(void){
    affiche_tab(tab, 10);
    pthread_t threads[10];
    struct thread_arg ta[10];
    void *ret;
    for(int i = 0; i < 10; i++){
        ta[i].number = tab[i];
        if(pthread_create(&(threads[i]), NULL, routine, &(ta[i]))){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    sleep(10);

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

    affiche_tab(tab, 10);
}

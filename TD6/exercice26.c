#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int is_prime(int nombre){
    for(int i = 2; i < nombre; i++){
        if(nombre % i == 0){
            return 0;
        }
    }
    return 1;
}

int main(void){
    sem_t memory_key;
    if(sem_init(&memory_key, 1, 1) == -1){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    //vérifier l'init
    int *array = malloc(101 * sizeof(char));
    if(array == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 101; i++){
        array[i] = 0;
    }

    //création processus enfant
    pid_t pid;
    for(int i = 0; i < 10; i++){
        pid = fork();
        if(pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            //execution enfant
            int nombre, is_present, current_value;
            for(int j = 0; j < 10; j++){
                is_present = 0;
                nombre = 2 + rand() % 100;
                //condition pour savoir si il est premier
                if(is_prime(nombre)){
                    //confition pour savoir si déjà dans le tableau
                    sem_wait(&memory_key);
                    for(int z = 1; z < 101; z++){
                        //récupération valeur indice z
                        if(array[z] == nombre){
                            is_present = 1;
                        }
                        if(array[z] == 0 && !is_present){
                            array[z] = nombre;
                            //incrémentation compteur
                            array[0]++;
                            break;
                        }
                    }
                    sem_post(&memory_key);
                }
            }
            //tri du tableau par ordre décroissant
            sem_wait(&memory_key);
            int l,k,c;
            for(int l = 1; l < 100; l++){
                for(int k = l+1; k < 101; k++){
                    if (array[l] < array[k]) {
                        c = array[k];
                        array[l] = array[k];
                        array[k] = c;
                    } 
                }
            }
            sem_post(&memory_key);
        }
    }
    
    int status;
    for(int i = 0; i < 10; i++){
        if(wait(&status) == -1){
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    int is_ordered = 1;
    int previous = array[1];
    printf("[");
    printf("%i", array[1]);
    for(int i = 2; i < 101; i++){
        printf("%i", array[i]);
        if(i < 100){
            printf(", ");
        }
        if(previous >= array[i]){
            is_ordered = 0;
        }
        previous = array[i];
    }
    printf("]\n");

    if(is_ordered){
        printf("Le tableau est trié\n");
    } else {
        printf("Le tableau n'est pas trié\n");
    }

    if(sem_destroy(&memory_key) == -1){
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }
    free(array);
}

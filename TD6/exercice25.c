#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

union semun{
    int val;
    struct semi_ds *buf;
    int *array;
    struct seminfo *__buf;
};

int is_prime(int nombre){
    for(int i = 2; i < nombre; i++){
        if(nombre % i == 0){
            return 0;
        }
    }
    return 1;
}

int main(void){
    union semun arg;
    int memory_key = semget(IPC_PRIVATE, 101, IPC_CREAT | 0666);
    if(memory_key == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    void memory[101];
    //vérifier l'init
    arg.array = malloc(101 * sizeof(int));
    if(arg.array == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < 101; i++){
        arg.array[i] = 0;
    }
    if(semctl(memory_key, 0, SETALL, arg) == -1){
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    free(arg.array);

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
                    for(int z = 1; z < 101; z++){
                        //récupération valeur indice z
                        if((current_value = semctl(memory_key, z, GETVAL)) == -1){
                            perror("semctl");
                            exit(EXIT_FAILURE);
                        }
                        if(current_value == nombre){
                            is_present = 1;
                        }
                        if(current_value == 0 && !is_present){
                            if(semctl(memory_key, z, SETVAL, nombre) == -1){
                                perror("semctl");
                                exit(EXIT_FAILURE);
                            }
                            //incrémentation compteur
                            if((current_value = semctl(memory_key, 0, GETVAL)) == -1){
                                perror("semctl");
                                exit(EXIT_FAILURE);
                            }
                            if(semctl(memory_key, 0, SETVAL, current_value + 1) == -1){
                                perror("semctl");
                                exit(EXIT_FAILURE);
                            }
                            break;
                        }
                    }
                }
            }
            //tri du tableau par ordre décroissant
            int l,k, value1, value2;
            for(int l = 1; l < 100; l++){
                for(int k = l+1; k < 101; k++){
                    if((value1 = semctl(memory_key, l, GETVAL)) == -1){
                        perror("semctl");
                        exit(EXIT_FAILURE);
                    }
                    if((value2 = semctl(memory_key, k, GETVAL)) == -1){
                        perror("semctl");
                        exit(EXIT_FAILURE);
                    }
                    if (value1 < value2) {
                        if(semctl(memory_key, l, SETVAL, value2) == -1){
                            perror("semctl");
                            exit(EXIT_FAILURE);
                        }
                        if(semctl(memory_key, k, SETVAL, value1) == -1){
                            perror("semctl");
                            exit(EXIT_FAILURE);
                        }
                    } 
                }
            }
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
    int current_value;
    int previous = semctl(memory_key, 1, GETVAL);
    if(previous == -1){
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    printf("[");
    printf("%i", memory[1]);
    for(int i = 2; i < 101; i++){
        if((current_value = semctl(current_value, i, GETVAL)) == -1){
            perror("semctl");
            exit(EXIT_FAILURE);
        }
        printf("%i", current_value);
        if(i < 100){
            printf(", ");
        }
        if(previous >= current_value){
            is_ordered = 0;
        }
        previous = current_value;
    }
    printf("]\n");

    if(is_ordered){
        printf("Le tableau est trié\n");
    } else {
        printf("Le tableau n'est pas trié\n");
    }

    if(semctl(memory_key, IPC_RMID, NULL) == -1){
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

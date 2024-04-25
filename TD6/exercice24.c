#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
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
    int memory_key = shmget(IPC_PRIVATE, 101 * sizeof(int), IPC_CREAT);
    if(memory_key == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    void memory[101];
    //pas sur des flags
    if(shmat(memory_key, memory, 0666)[0] == -1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    //vérifier l'init
    for(int i = 0; i < 101; i++){
        memory[i] = 0;
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
            int nombre;
            if(shmat(memory_key, memory, 0666)[0] == -1){
                perror("shmat");
                exit(EXIT_FAILURE);
            }
            int is_present;
            for(int j = 0; j < 10; j++){
                is_present = 0;
                nombre = 2 + rand() % 100;
                //condition pour savoir si il est premier
                if(is_prime(nombre)){
                    //confition pour savoir si déjà dans le tableau
                    for(int z = 1; z < 101; z++){
                        if(memory[z] == nombre){
                            is_present = 1;
                        }
                        if(memory[z] == 0 && !is_present){
                            memory[z] = nombre;
                            memory[0]++;
                            break;
                        }
                    }
                }
            }
            //tri du tableau par ordre décroissant
            int l,k,c;
            for(int l = 1; l < 100; l++){
                for(int k = l+1; k < 101; k++){
                    if (memory[l] < memory[k]) {
                        c = memory[k];
                        memory[l] = memory[k];
                        memory[k] = c;
                    } 
                }
            }
            if(shmdt(memory) == -1){
                perror("shmdt");
                exit(EXIT_FAILURE);
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
    int previous = memory[1];
    printf("[");
    printf("%i", memory[1]);
    for(int i = 2; i < 101; i++){
        printf("%i", memory[i]);
        if(i < 100){
            printf(", ");
        }
        if(previous >= memory[i]){
            is_ordered = 0;
        }
        previous = memory[i];
    }
    printf("]\n");

    if(is_ordered){
        printf("Le tableau est trié\n");
    } else {
        printf("Le tableau n'est pas trié\n");
    }

    if(shmdt(memory) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    if(shmctl(memory_key, IPC_RMID, NULL) == -1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}

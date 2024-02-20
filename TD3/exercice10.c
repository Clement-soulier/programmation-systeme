#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int errno;

void set_env(char *nom, char *valeur){
    errno = 0;
    setenv(nom, valeur, 1);
    if(errno){
        fprintf(stderr, "Impossible d'écrire la variable %s: %s", nom, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void delete_env(char *nom){
    errno = 0;
    unsetenv(nom);
    if(errno){
        fprintf(stderr, "Impossible de supprimer la variable %s: %s", nom, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

char *get_name(char *string){
  size_t size = 0;
  while(string[size] != '='){
    size++;
  }
  printf("size: %li\n", size);
  char *name = malloc((size + 1) * sizeof(char));
  strncpy(name, string, size + 1);
  name[size] = '\0';
  return name;
}

int main(int argc, char *argv[]){
    int id = 1;
    while(id < argc){
        char *arg = argv[id];
        char *is_to_set = index(arg, 61);
        if(strcmp(is_to_set, "=")){
            char *name = get_name(arg);
            char *value = 1 + is_to_set;
            set_env(name, value);
            free(name);
            free(value);
            free(is_to_set);
            free(arg);
        }
        else if(){
            char *name = get_name(arg);
            delete_env(name);

        }
        id++;
    }
}

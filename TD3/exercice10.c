#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t cmd_list_lenght = 128;
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
    char *name = malloc((size + 1) * sizeof(char));
    if(!name){
        exit(EXIT_FAILURE);
    }
    strncpy(name, string, size + 1);
    name[size] = '\0';
    return name;
}

void add_cmd(char* cmd_list, char *arg){
  int list_lenght = (int) strlen(cmd_list) + 1;
  int arg_lenght = (int) strlen(arg) + 1;
  if((list_lenght - arg_lenght) >= 0){
    strcat(cmd_list, arg);
  }
  else{
    cmd_list_lenght += 60;
    cmd_list = realloc(cmd_list, cmd_list_lenght * sizeof(char));
    if(!cmd_list){
      exit(EXIT_FAILURE);
    }
    strcat(cmd_list, arg);
  }
}

int main(int argc, char* argv[]){
    int id = 1;
    int cmd_mode = 1;
    char *cmd = malloc(cmd_list_lenght * sizeof(char));
    if(!cmd){
        exit(EXIT_FAILURE);
    }
    strcpy(cmd, "");
    while(id < argc){
        char *arg = argv[id];
        if(cmd_mode == 1){
            char *is_to_set = index(arg, 61);
            if(is_to_set){
                if(*(1 + is_to_set) == '\0'){
                char *name = get_name(arg);
                delete_env(name);
                free(name);
                }
                else {
                    char *name = get_name(arg);
                    char *value = 1 + is_to_set;
                    set_env(name, value);
                    free(name);
                }
            }
            else {
                cmd_mode = 0;
            }
            free(is_to_set);
        }
        else {
            add_cmd(cmd, arg);
        }
        id++;
    }
    if(cmd[0] != '\0'){
        return system(cmd);
    }
    return EXIT_SUCCESS;
}

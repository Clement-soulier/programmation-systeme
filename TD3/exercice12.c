#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern int errno;

typedef struct{
    char **files;
    int len;
}dir_t;

int is_dir(char *path){
    errno = 0;
    struct stat *directory = malloc(1 * sizeof(struct stat));
    if(directory == NULL){
        printf("Impossible d'allouer la mémoire pour stocker les métatdonnées de %s", path);
        exit(EXIT_FAILURE);
    }
    stat(path, directory);
    if(errno == ENOENT || errno == ENOTDIR){
        free(directory);
        return 0;
    }
    else if(errno){
        fprintf(stderr, "Erreur lors de l'accès au métadonnées de %s: %s",path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(directory->st_mode & S_IFDIR){
        free(directory);
        return 1;
    }
    free(directory);
    return 0;
}

dir_t ls(char *path){
    errno = 0;
    DIR *wd = opendir(path);
    if(wd == NULL){
        fprintf(stderr, "Impossible d'ouvrir le répertoire %s: %s", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    dir_t dir = {.files = malloc(sizeof(char*[100])), .len = 0};
    if(dir.files == NULL){
        printf("Impossible d'allouer la mémoire pour dir_t pour le chemin %s", path);
        exit(EXIT_FAILURE);
    }
    struct dirent *file;
    do{
        errno = 0;
        file = readdir(wd);
        if(file == NULL){
            break;
        }
        if(errno){
            fprintf(stderr, "Impossible de lire l'intérieur du currant working directory: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(!strcmp(file->d_name, ".") && !strcmp(file->d_name, "..")){
            dir.files[dir.len++] = file->d_name;
        }
    }while(file != NULL);
    closedir(wd);
    return dir;
}


void print_name(char *name, char *prefix){
    char *output = malloc(255 * sizeof(char));
    strcat(output, prefix);
    strcat(output, name);
    printf("%s\n", output);
    free(output);
}

void tree(char *path, char *prefix){
    dir_t files = ls(path);
    for(int i = 0; i < files.len; i++){
        char *file_name = malloc(255 * sizeof(char));
        strcpy(file_name, "");
        strcat(file_name, path);
        strcat(file_name, "/");
        strcat(file_name, files.files[i]);
        
        //code pour écrire le bon prefix
        if(is_dir(file_name)){
            print_name(files.files[i], prefix);
            tree(file_name, prefix);
        }
        else {
            print_name(files.files[i], prefix);
        }
        free(file_name);
    }
}

int main(int argc, char *argv[]){
    size_t cwd_buffer_size = 128;
    errno = 0;
    char *cwd_buffer = malloc(cwd_buffer_size * sizeof(char));
    if(cwd_buffer == NULL){
        printf("Impossible d'allouer la mémoire du cwd_buffer\n");
        exit(EXIT_FAILURE);
    }
    if(getcwd(cwd_buffer, cwd_buffer_size) == NULL){
        fprintf(stderr, "Impossible d'accéder au current working directory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    tree(cwd_buffer, "");
}

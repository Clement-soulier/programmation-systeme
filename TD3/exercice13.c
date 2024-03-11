#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

extern int errno;

int is_dir(char *path){
    errno = 0;
    struct stat *directory = malloc(1 * sizeof(struct stat));
    if(directory == NULL){
        printf("Impossible d'allouer la mémoire pour stocker les métatdonnées de %s\n", path);
        exit(EXIT_FAILURE);
    }
    stat(path, directory);
    if(errno == ENOENT || errno == ENOTDIR){
        free(directory);
        return 0;
    }
    else if(errno){
        fprintf(stderr, "Erreur lors de l'accès au métadonnées de %s: %s\n",path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(directory->st_mode & S_IFDIR){
        free(directory);
        return 1;
    }
    free(directory);
    return 0;
}

void cp_mode(char *src, char *tgt){
    struct stat *meta = malloc(sizeof(struct stat));
    if(meta == NULL){
        printf("Impossible d'allouer la mémoire pour les métadonnées de %s\n", src);
        exit(EXIT_FAILURE);
    }
    errno = 0;
    stat(src, meta);
    if(errno){
        fprintf(stderr, "Impossible de lire les métadonnées de %s: %s\n", src, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    chmod(tgt, meta->st_mode);
    if(errno){
        fprintf(stderr, "Impossible de changer les permissions de %s: %s\n", tgt, strerror(errno));
        exit(EXIT_FAILURE);
    }
    free(meta);
}

void cp_file(char *src, char *tgt){
    errno = 0;
    int opened_src = open(src, O_RDONLY);
    if(errno){
        fprintf(stderr, "Erreur lors de l'ouverture de %s: %s\n", src, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    int opened_tgt = open(tgt, O_WRONLY | O_CREAT);
    if(errno){
        fprintf(stderr, "Erreur lors de l'ouverture de %s: %s\n", tgt, strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t count = 1000;
    size_t to_write;
    void *buffer = malloc(count * sizeof(char));
    if(buffer == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer\n");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    do{
        to_write = read(opened_src, buffer, count);
        if(errno){
            fprintf(stderr, "Erreur lors de la lecture de %s: %s\n", src, strerror(errno));
            exit(EXIT_FAILURE);
        }
        write(opened_tgt, buffer, to_write);
        if(errno){
            fprintf(stderr, "Erreur lors de l'écriture de %s: %s\n", tgt, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }while(to_write != 0);
    errno = 0;
    if(close(opened_src)){
        fprintf(stderr, "Erreur lors de la fermeture de %s: %s", src, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    if(close(opened_tgt)){
        fprintf(stderr, "Erreur lors de la fermeture de %s: %s", tgt, strerror(errno));
        exit(EXIT_FAILURE);
    }
    free(buffer);
    cp_mode(src, tgt);
}

void cp_dir(char *src, char *tgt){
    errno = 0;
    DIR *opened_tgt = opendir(tgt);
    if(errno != ENOENT){
        if(errno){
            fprintf(stderr, "Erreur lors de l'ouverture de %s: %s\n", tgt, strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            printf("Le dossier %s existe déja, impossible de l'écraser\n", tgt);
            exit(EXIT_FAILURE);
        }
    } else {
        char *current_src_path = malloc(255 * sizeof(char));
        if(current_src_path == NULL){
            printf("Impossible d'allouer la mémoire pour current_src_path\n");
            exit(EXIT_FAILURE);
        }
        char *current_tgt_path = malloc(255 * sizeof(char));
        if(current_tgt_path == NULL){
            printf("Impossible d'allouer la mémoire pour current_tgt_path\n");
            exit(EXIT_FAILURE);
        }
        errno = 0;
        DIR *opened_src = opendir(src);
        if(errno){
            fprintf(stderr, "Erreur lors de l'ouverture de %s: %s\n", src, strerror(errno));
            exit(EXIT_FAILURE);
        }
        struct stat *directory = malloc(sizeof(struct stat));
        if(directory == NULL){
            printf("Impossible d'allouer la mémoire pour les métadonnées de %s", src);
            exit(EXIT_FAILURE);
        }
        errno = 0;
        stat(src, directory);
        if(errno){
            fprintf(stderr, "Impossible de lire les métadonnées de %s: %s\n", src, strerror(errno));
            exit(EXIT_FAILURE);
        }
        errno = 0;
        mkdir(tgt, directory->st_mode);
        if(errno){
            fprintf(stderr, "Impossible de créer le fichier %s: %s", tgt, strerror(errno));
            exit(EXIT_FAILURE);
        }
        struct dirent *file;
        do{
            errno = 0;
            file = readdir(opened_src);
            if(file == NULL){
                break;
            }
            if(errno){
                fprintf(stderr, "Impossible de lire l'intérieur de %s: %s\n", src, strerror(errno));
                exit(EXIT_FAILURE);
            }
            strcpy(current_src_path, "");
            strcat(current_src_path, src);
            strcat(current_src_path, "/");
            strcat(current_src_path, file->d_name);

            strcpy(current_tgt_path, "");
            strcat(current_tgt_path, tgt);
            strcat(current_tgt_path, "/");
            strcat(current_tgt_path, file->d_name);
            if(file->d_type & DT_DIR ){
                if(strcmp(file->d_name, ".") && strcmp(file->d_name, "..")){
                    cp_dir(current_src_path, current_tgt_path);
                }
            } else {
                if(strcmp(file->d_name, ".") && strcmp(file->d_name, "..")){
                    cp_file(current_src_path, current_tgt_path);    
                }
            }
        }while(file != NULL);
        free(current_src_path);
        free(current_tgt_path);
        closedir(opened_src);
    }
    closedir(opened_tgt);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Erreur: Pas assez d'arguments\n");
        exit(EXIT_FAILURE);
    }
    if(argc > 3){
        printf("Erreur: Trop d'arguments\n");
        exit(EXIT_FAILURE);
    }
    if(is_dir(argv[1])){
        cp_dir(argv[1], argv[2]);
    } else {
        cp_file(argv[1], argv[2]);
    }
}

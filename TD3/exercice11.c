#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

extern int errno;
size_t cwd_buffer_size = 128;


void contruct_right_display(char *output, struct stat *file){
    //first letter
    if(file->st_mode & S_IFREG){
        strcat(output, "-");
    } else if(file->st_mode & S_IFDIR){
        strcat(output, "d");
    } else if(file->st_mode & S_IFLNK){
        strcat(output, "l");
    } else if(file->st_mode & S_IFSOCK){
        strcat(output, "s");
    } else if(file->st_mode & S_IFIFO){
        strcat(output, "p");
    } else if(file->st_mode & S_IFBLK){
        strcat(output, "b");
    } else if(file->st_mode & S_IFCHR){
        strcat(output, "c");
    }
    //user permissions
    if(file->st_mode & S_IRUSR){
        strcat(output, "r");
    } else {
        strcat(output, "-");
    }
    if(file->st_mode & S_IWUSR){
        strcat(output, "w");
    } else {
        strcat(output, "-");
    }
    if(file->st_mode & S_IXUSR){
        strcat(output, "x");
    } else if(file->st_mode & S_ISUID){
        strcat(output, "s");
    } else {
        strcat(output, "-");
    }
    //group permissions
    if(file->st_mode & S_IRGRP){
        strcat(output, "r");
    } else {
        strcat(output, "-");
    }
    if(file->st_mode & S_IWGRP){
        strcat(output, "w");
    } else {
        strcat(output, "-");
    }
    if(file->st_mode & S_IXGRP){
        strcat(output, "x");
    } else if(file->st_mode & S_ISGID){
        strcat(output, "s");
    } else {
        strcat(output, "-");
    }
    //others permissions
    if(file->st_mode & S_IROTH){
        strcat(output, "r");
    } else {
        strcat(output, "-");
    }
    if(file->st_mode & S_IWOTH){
        strcat(output, "w");
    } else {
        strcat(output, "-");
    }
    if(file->st_mode & S_IXOTH){
        strcat(output, "x");
    } else if(file->st_mode & S_ISVTX){
        strcat(output, "t");
    } else {
        strcat(output, "-");
    }
}

void display_info(char *parent_path, struct dirent *file){
    size_t file_path_size = 128;
    char *file_path = malloc((cwd_buffer_size + file_path_size) * sizeof(char));
    if(file_path == NULL){
        printf("Impossible d'allouer la mémoire pour le chemin du fichier: %s\n", file->d_name);
        printf("qui se trouve dans le répertoire: %s\n", parent_path);
        exit(EXIT_FAILURE);
    }
    strcpy(file_path, "");    
    strcat(file_path, parent_path);
    strcat(file_path, "/");
    strcat(file_path, file->d_name);
    
    errno = 0;
    struct stat *file_meta = malloc(1 * sizeof(struct stat));
    if(file_meta == NULL){
        printf("Impossible d'allouer la mémoire pour les métadonnées du fichier %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    if(stat(file_path, file_meta)){
        fprintf(stderr, "Impossible de récupérer les métadonnées du fichier %s: %i\n", file_path, errno);
        exit(EXIT_FAILURE);
    }
    char *meta_out = malloc(255 * sizeof(char));
    if(meta_out == NULL){
        printf("Impossible d'allouer la mémoire pour afficher les métadonnées du fichier %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    strcpy(meta_out, "");
    contruct_right_display(meta_out, file_meta);
    strcat(meta_out, " ");
    char *string_buffer = malloc(16 * sizeof(char));
    if(string_buffer == NULL){
        printf("Impossible d'allouer la mémoire pour liens symbolique du fichier %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    sprintf(string_buffer, "%lu", file_meta->st_nlink); 
    strcat(meta_out, string_buffer);

    strcat(meta_out, " ");
    
    sprintf(string_buffer, "%u", file_meta->st_uid);
    strcat(meta_out, string_buffer);

    strcat(meta_out, " ");

    sprintf(string_buffer, "%u", file_meta->st_gid);
    strcat(meta_out, string_buffer);

    strcat(meta_out, " ");

    sprintf(string_buffer, "%li", file_meta->st_size);
    strcat(meta_out, string_buffer);


    strcat(meta_out, " ");
    //condiiton -> comportement lien symbolique
    strcat(meta_out, file->d_name);
    printf("%s\n", meta_out);


    free(file_path);
    free(file_meta);
    free(meta_out);
    free(string_buffer);
}

int main(void){
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
    errno = 0;
    DIR *cwd = opendir(cwd_buffer);
    if(cwd == NULL){
        fprintf(stderr, "Impossible d'ouvrir le current working directory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct dirent *file;
    do{
        errno = 0;
        file = readdir(cwd);
        if(file == NULL){
            break;
        }
        if(errno){
            fprintf(stderr, "Impossible de lire l'intérieur du current working directory: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        display_info(cwd_buffer, file);
    }while(file != NULL);
    free(cwd_buffer);
    closedir(cwd);
    exit(EXIT_SUCCESS);
}

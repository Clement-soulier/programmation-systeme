#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define CHUNKCOUNT 1024
#define CHUNKSIZE CHUNKCOUNT*sizeof(int)


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

void make_data(char *path, unsigned int size){
    errno = 0;
    int opened_src = open("/dev/urandom", O_RDONLY);
    if(errno){
        fprintf(stderr, "Erreur lors de l'ouverture de /dev/urandom: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    int opened_tgt = open(path, O_WRONLY | O_CREAT);
    if(errno){
        fprintf(stderr, "Erreur lors de l'ouverture de %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t to_write;
    int *buffer = malloc(size * sizeof(int));
    if(buffer == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer\n");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    to_write = read(opened_src, buffer, size);
    if(errno){
        fprintf(stderr, "Erreur lors de la lecture de /dev/urandom: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(opened_tgt, buffer, to_write);
    if(errno){
        fprintf(stderr, "Erreur lors de l'écriture de %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    if(close(opened_src)){
        fprintf(stderr, "Erreur lors de la fermeture de /dev/urandom: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    if(close(opened_tgt)){
        fprintf(stderr, "Erreur lors de la fermeture de %s: %s", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    cp_mode("/dev/urandom", path);
    free(buffer);
}

int is_sorted(char *path){
    errno = 0;
    int file = open(path, O_RDONLY);
    if(errno){
        fprintf(stderr, "Erreur lors de l'ouverture de %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t readed;
    int *number1 = malloc(sizeof(int));
    if(number1 == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer\n");
        exit(EXIT_FAILURE);
    }
    int *number2 = malloc(sizeof(int));
    if(number2 == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer\n");
        exit(EXIT_FAILURE);
    }
    readed =  read(file, number1, 1);
    while(readed != 0){
        readed = read(file, number2, 1);
        if(number1[0] > number2[0]){
            close(file);
            free(number1);
            free(number2);
            return 0;

        }
        number1[0] = number2[0];
    }
    close(file);
    free(number1);
    free(number2);
    return 1;
}

char *chunk_name(char *base, unsigned int count, char *prev){
    if(prev != NULL){
        strcpy(prev, "");
        strcat(prev, base);
        strcat(prev, ".");
       	char *padding = malloc(6 * sizeof(char));
        sprintf(padding, "%06d", (int) count);
        strcat(prev, padding);
        free(padding);
        return prev;
    } else {
        char *name = malloc(255 * sizeof(char));
        strcpy(name, "");
        strcat(name, base);
        strcat(name, ".");
        char *padding = malloc(6 * sizeof(char));
        sprintf(padding, "%06u", count);
        strcat(name, padding);
        free(padding);
        return name;
    }
}

int find_min(int *tab, int len){
    int min = tab[0];
    int index = 0;
    for(int i = 0; i < len; i++){
        if(tab[i] < min){
            min = tab[i];
            index = i;
        }
    }
    return index;
}

void delete_index_from_tab(int *tab, int len, int index){// fuite de mémoire ???
    int *new_tab = malloc((len - 1) * sizeof(int));
    if(new_tab == NULL){
        printf("Impossible d'allouer la mémoire pour trié le tableau");
        exit(EXIT_FAILURE);
    }
    int offset = 0;
    for(int i = 0; i < (len - 1); i++){
        if(i != index){
            new_tab[i - offset] = tab[i];
        } else {
            offset = 1;
        }
    }
    tab = new_tab;
}


//ne fonctionne pas
void sort(int *tab){ // fuite de mémoire ???
    int *new_tab = malloc(CHUNKSIZE);
    if(new_tab == NULL){
        printf("Impossible d'allouer la mémoire pour trié le tableau");
        exit(EXIT_FAILURE);
    }
    int len = CHUNKCOUNT;
    int min;
    for(int i = 0; i < CHUNKCOUNT; i++){
        min = find_min(tab, len);
        new_tab[i] = tab[min];
        delete_index_from_tab(tab, len, min);
        len--;
    }
    tab = new_tab;
}

int split(char *path){
    int morceaux = 0;
    errno = 0;
    int src = open(path, O_RDONLY);
    if(errno){
        fprintf(stderr, "Impossible d'ouvrir le fichier %s: %s", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    int *buffer = malloc(CHUNKSIZE);
    if(buffer == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer");
        exit(EXIT_FAILURE);
    }
    char *prev = NULL;
    int tgt;
    int to_write;
    while(1){
        errno = 0;
        to_write = read(src, buffer, CHUNKCOUNT);
        if(errno){
            fprintf(stderr, "Impossible de lire le fichier %s: %s", path, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(to_write == 0){
            break;
        }
        sort(buffer);
        char *name = chunk_name(path, morceaux, prev);
        errno = 0;
        tgt = open(name, O_CREAT | O_WRONLY);
        if(errno){
            fprintf(stderr, "Impossible d'ouvrir le fichier %s: %s", name, strerror(errno));
            exit(EXIT_FAILURE);
        }
        errno = 0;
        write(tgt, buffer, to_write);
        if(errno){
            fprintf(stderr, "Impossible de lire le fichier %s: %s", path, strerror(errno));
            exit(EXIT_FAILURE);
        }
        errno = 0;
        close(tgt);
        if(errno){
            fprintf(stderr, "Impossible de lire le fichier %s: %s", path, strerror(errno));
            exit(EXIT_FAILURE);
        }
        morceaux++;
    }
    return morceaux;
}

void fuse(char *src1, char *src2, char *tgt){   
    errno = 0;
    int opened_src1 = open(src1, O_RDONLY);
    if(errno){
        fprintf(stderr, "Impossible d'ouvrir le fichier %s: %s", src1, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    int opened_src2 = open(src1, O_RDONLY);
    if(errno){
        fprintf(stderr, "Impossible d'ouvrir le fichier %s: %s", src2, strerror(errno));
        exit(EXIT_FAILURE);
    }
    errno = 0;
    int opened_tgt = open(tgt, O_WRONLY);
    if(errno){
        fprintf(stderr, "Impossible d'ouvrir le fichier %s: %s", tgt, strerror(errno));
        exit(EXIT_FAILURE);
    }
    int *buffer1 = malloc(sizeof(int));
    if(buffer1 == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer1");
        exit(EXIT_FAILURE);
    }
    int *buffer2 = malloc(sizeof(int));
    if(buffer2 == NULL){
        printf("Impossible d'allouer la mémoire pour le buffer2");
        exit(EXIT_FAILURE);
    }
    int to_write1, to_write2;
    while(1){
        errno = 0;
        to_write1 = read(opened_src1, buffer1, 1);
        if(errno){
            fprintf(stderr, "Impossible de lire le fichier %s: %s", src1, strerror(errno));
            exit(EXIT_FAILURE);
        }
        errno = 0;
        to_write2 = read(opened_src2, buffer2, 1);
        if(errno){
            fprintf(stderr, "Impossible de lire le fichier %s: %s", src2, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(to_write1 == 0 && to_write2 == 0){
            break;
        }
        if(buffer1[0] > buffer2[0]){
            errno = 0;
            write(opened_tgt, buffer2, to_write2);
            if(errno){
                fprintf(stderr, "Impossible d'écrire sur le fichier %s: %s", tgt, strerror(errno));
                exit(EXIT_FAILURE);
            }
            if(lseek(opened_src1, -1, SEEK_CUR) == -1){
                fprintf(stderr, "Erreur lors du déplacement de curseur dans le fichier %s: %s", src1, strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else {
            write(opened_tgt, buffer1, to_write1);
            if(errno){
                fprintf(stderr, "Impossible d'écrire sur le fichier %s: %s", tgt, strerror(errno));
                exit(EXIT_FAILURE);
            }
            if(lseek(opened_src2, -1, SEEK_CUR) == -1){
                fprintf(stderr, "Erreur lors du déplcement de curseur dans le fichier %s: %s", src2, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    close(opened_src1);
    close(opened_src2);
    close(opened_tgt);
}

char *truncate_filename(char *path){
    char *new_path = malloc(255 * sizeof(char));
    strcpy(new_path, path);
    int len = strlen(new_path);
    for (int i = len - 1; i >= 0; i--) {
        if (new_path[i] == '/') {
            new_path[i + 1] = '\0';
        }
    }
    return new_path;
}

void sort_file(char *path){
    char *name = malloc(255 * sizeof(char));
    char *prev = NULL;
    char *current_file;
    if(name == NULL){
        printf("Impossible d'allouer la mémoire pour le nom");
        exit(EXIT_FAILURE);
    }
    strcpy(name, "");
    strcat(name, path);
    strcat(name, ".");
    strcat(name,"sorted");
    char *path_without_filename = truncate_filename(path);
    if(is_sorted(path)){
        cp_file(path, name);
    } else{
        int morceau = split(path);
        for(int i = 0; i < morceau; i++){
            current_file = chunk_name(path, i, prev);
            char *current_file_path = malloc(255 * sizeof(char));
            strcpy(current_file_path, "");
            strcat(current_file_path, path_without_filename);
            strcat(current_file_path, current_file);
            fuse(current_file, name, name);//non testé
            unlink(current_file_path);
        }
    }
}

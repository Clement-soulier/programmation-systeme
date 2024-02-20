#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

unsigned int stack_size = 128;
extern int errno;

typedef struct{
    int *content;
    int current_position;
}stack;


void affiche_pile(stack *s){
    printf("***********************************\n");
    printf("Current position: %i\n", s->current_position);
    for(int i = s->current_position - 1; i > -1; i--){
        printf("%i\n", s->content[i]);
    }
    printf("***********************************\n");
}


stack *create_stack(){
    stack *s_pointeur = malloc(1 * sizeof(stack));
    if(s_pointeur){
        stack s = {
            .content = malloc(stack_size * sizeof(int)),
            .current_position = 0
        };
        if(s.content){
            *s_pointeur = s;
            return s_pointeur;
        }
        printf("Erreur: Mémoire pile non alloué");
        exit(-1);
    }
    printf("Erreur: Mémoire non alloué");
    exit(-1);
}

void free_stack(stack *s){
    if(s){
        if(s->content){
            free(s->content);
        }
        free(s);
    }
}

void make_bigger_content(stack *s){
    errno = 0;
    stack_size += 30;
    int *tmp = realloc(s->content, stack_size * sizeof(int));
    if(!tmp){
        errno = 254;
        fprintf(stderr, "Mémoire inssufisante pour la pile: %s\n", strerror(errno));
        exit(-1);
    }
}

void put(stack *s, int number){
    if(s->current_position < 128){
        s->content[s->current_position++] = number;
    } else {
        make_bigger_content(s);
        s->content[s->current_position++] = number;
    }
}

int pop(stack *s){
    errno = 0;
    if(s->current_position - 1 > -1){
        return s->content[--s->current_position];
    }
    errno = 253;
    fprintf(stderr, "Il n'y a pas assez d'agument pour assurer le calcul: %s\n", strerror(errno));
    exit(-1);
}

void add(stack *s){
    int b = pop(s);
    int a = pop(s);
    put(s, a + b);
}

void soustract(stack *s){
    int b = pop(s);
    printf("b = %i\n", b);
    int a = pop(s);
    printf("a = %i\n", a);
    int result = a - b;
    put(s, result);
}

void multiply(stack *s){
    int b = pop(s);
    int a = pop(s);
    put(s, a * b);
}

void divide(stack *s){
    errno = 0;
    int b = pop(s);
    if(b == 0){
        errno = 252;
        fprintf(stderr, "Impossible de diviser par 0: %s\n", strerror(errno));
        exit(-1);
    }
    int a = pop(s);
    put(s, a / b);
}

void execute_operation(stack *s, char *sign){
    errno = 0;
    if(!strcmp(sign, "+")){
        add(s);
    } else
    if(!strcmp(sign, "-")){
        printf("succès\n");
        soustract(s);
    } else
    if(!strcmp(sign, "*")){
        multiply(s);
    } else
    if(!strcmp(sign, "/")){
        divide(s);
    } else {
        errno = 251;
        fprintf(stderr, "L'entier %s est invalide: %s\n", sign, strerror(errno));
        exit(-1);
    }
}


void read_command_line(int argc, char *argv[], int base, stack *s){
    int index = 1;
    char *end;
    char *entry;
    int number;
    while(index < argc){
        end = NULL;
        entry = argv[index];
        number = (int) strtol(entry, &end, base);
        if(strcmp(end, entry)){
            put(s, number);
        } else {
            execute_operation(s, entry);
        }
        index++;
    }
}

int verify_base(int base){
    if(base >= 2 && base <= 16){
        return 1;
    }
    return 0;
}

int get_base(){
    errno = 0;
    char *base_from_env = getenv("BASE");
    if(base_from_env){
      int base = (int) strtol(base_from_env, NULL, 10);
      if(verify_base(base)){
       return base;
      }
    }
    errno = 250;
    fprintf(stderr, "La base n'est pas comprise entre 2 et 16: %s\n", strerror(errno));
    exit(-1);
}

int verify_stack(stack *s){
    if(s->current_position == 1){
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]){
    int base = get_base();
    stack *s = create_stack();
    read_command_line(argc, argv, base, s);
    errno = 0;
    if(verify_stack(s)){
        errno = 255;
        printf("Aucun argument donné ou bien\n");
        fprintf(stderr, "Il reste des valeurs sur la pile, les opérateurs sont mal placés: %s\n", strerror(errno));
        exit(-1);
    }
    printf("%i\n", pop(s));
    free_stack(s);
    exit(EXIT_SUCCESS);
}

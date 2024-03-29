//not tested
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

char commands[90] = "Avalaible command:\nsnd: send a message\nrcv: receive a message\nexit: exit the program\n";
#define path "/tmp/pipe"
#define MSG_SIZE 256

void free_stdin(){
    int c = 0;
    while(c != '\n' && c!= EOF){
        c = getchar();
    }
}

void send_message(int to_write){
    char text[MSG_SIZE + 1];
    char msg[MSG_SIZE + 2];
    printf("Please write the message you want to send:\n");
    if(fgets(text, MSG_SIZE + 1, stdin) == NULL){
        free_stdin();
        printf("An error occured, please retry\n");
        return;
    } else if(fgetc(stdin) != EOF){ //don't work
        free_stdin();
        printf("Your message should be less then 256 characters !\nPlease retry\n");
        return;
    } else {
        int text_len = (int) strlen(text);
        sprintf(msg, "%i%s", text_len, text);
        int writed = write(to_write, msg, MSG_SIZE + 2);
        if(writed == -1){
            perror("write");
            return;
        } else if(writed < text_len + 2){
            printf("An error occured during the message writing\n");
            return;
        }
    }
    printf("Message sent sucessfully !\n\n");
}

void exit_prog(int read, int write){
    if(close(read) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }
    if(close(write) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }
    printf("Goodbye !\n");
    exit(0);
}

//don't work
void receive_message(int to_read){
    char buf[MSG_SIZE + 1];
    int readed = read(to_read, buf, MSG_SIZE + 1);
    if(readed == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }
    int to_display = (int) strtol(buf, NULL, 10);
    if(read(to_read, buf, to_display) == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buf);
}

int main(void){
    system("./server");
    char cmd[6];
    int pipe_read = open(path, O_RDONLY);
    if(pipe_read == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    int pipe_write = open(path, O_WRONLY);
    if(pipe_write == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("Welcome to message.c !\n");
    while(1){
        printf("%s\n", commands);
        printf("Please enter a command:");
        if(fgets(cmd, 6, stdin) == NULL){
            free_stdin();
            printf("Sorry an error occured, please retry\n");
            continue;
        } else if(fgetc(stdin) != EOF) {
            printf("%i\n", fgetc(stdin));
            free_stdin();
            printf("The command is too long, please retry\n\n");
            continue;

        }
        if(!strcmp(cmd, "snd\n")){
            send_message(pipe_write);
            continue;
        } else if(!strcmp(cmd, "rcv\n")){
            receive_message(pipe_read);
            continue;
        } else if(!strcmp(cmd, "exit\n")){
                exit_prog(pipe_read, pipe_write);
        } else {
            printf("%s\n", cmd);
            printf("This command is invalid\n\n");
            continue;
        }
    }
}

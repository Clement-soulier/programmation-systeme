#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

char commands[148] = "Avalaible command:\nsnd: send a message\nrcv: receive a message\nstat: see the number of message\nrm: delete the message FIFO and ends the program\n";
#define MSG_SIZE 256

struct msgbuf{
    long msg_type;
    char msg_text[MSG_SIZE];
};

void free_stdin(){
    int c = 0;
    while(c != '\n' && c!= EOF){
        c = getchar();
    }
}

void send_message(int msg_queue){
    char text[MSG_SIZE + 1];
    printf("Please write the message you want to send:\n");
    if(fgets(text, MSG_SIZE + 1, stdin) == NULL){
        free_stdin();
        printf("An error occured, please retry\n");
        return;
    } else if(fgetc(stdin) != 10){ //don't work
        free_stdin();
        printf("Your message should be less then 256 characters !\nPlease retry\n");
        return;
    } else {
        struct msgbuf message;
        message.msg_type = 1;
        strcpy(message.msg_text, text);
        if(msgsnd(msg_queue, &message, strlen(text), IPC_NOWAIT) == -1){
            free_stdin();
            perror("msgsnd");
            return;
        }
    }
    printf("Message sent sucessfully !\n\n");
}


//don't work
void receive_message(int msg_queue){
    char text[MSG_SIZE];
    ssize_t readed = msgrcv(msg_queue, text, MSG_SIZE, 0, IPC_NOWAIT);
    printf("%c", text[0]);
    if(readed == -1){
        perror("msgrcv");
        return;
    }
    printf("%s\n\n", text);
}


void stat_message(int msg_queue){
    struct msqid_ds stat;
    if(msgctl(msg_queue, IPC_STAT, &stat) == -1){
        perror("msgctl");
        return;
    }
    printf("There are %lu messages\n", stat.msg_qnum);
}

void rm_message(int msg_queue){
    if(msgctl(msg_queue, IPC_RMID, NULL) == -1){
        perror("msgctl");
        return;
    }
    printf("message queue removed sucessfully !\n");
}

int main(void){
    char cmd[6];
    int msg_queue = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if(msg_queue == -1){
        perror("msgget");
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
        } else if(fgetc(stdin) != 10) {
            printf("%i\n", fgetc(stdin));
            free_stdin();
            printf("The command is too long, please retry\n\n");
            continue;

        }
        printf("\n");
        if(!strcmp(cmd, "snd\n")){
            send_message(msg_queue);
            continue;
        } else if(!strcmp(cmd, "rcv\n")){
            receive_message(msg_queue);
            continue;
        } else if(!strcmp(cmd, "stat\n")){
            stat_message(msg_queue);
            continue;
        } else if(!strcmp(cmd, "rm\n")){
            rm_message(msg_queue);
            break;
        } else {
            printf("%s\n", cmd);
            printf("This command is invalid\n\n");
            continue;
        }
    }
    printf("Goodbye !\n");
}

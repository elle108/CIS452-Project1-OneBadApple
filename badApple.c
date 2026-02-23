#include <stdio.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/stat.h> 
#include <sys/ipc.h>  
#include <sys/shm.h> 
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>

// Message size
#define SIZE 256

// Struct for message
typedef struct {
    int destination;
    char content[SIZE];
    int isEmpty;
} Message;

// Global Variables
int numNodes;
int shutdown = 0;

// Signal handler
void sigHandler (int sigNum) {
    if(sigNum == SIGINT){
        printf ("Exit Gracefully\n");
        exit(0);
    }
}

// Child process function
void childNodeProcess(int nodeId, int readPipe, int writePipe) {
    Message message;
    printf("inside child process node: %d\n", nodeId);

    read(readPipe, &message, sizeof(Message));
        // Wait to receive the apple
        // if (read(readPipe, &message, sizeof(Message)) <= 0) {
        //     return;
        // }
        
        // Check if this is a shutdown signal
        // if (message.destination == -1) {
        //     printf("Node %d has received shutdown signal.\n", nodeId);
        //     // Pass shutdown signal to next node
        //     write(writePipe, &message, sizeof(Message));
        //     return;
        // }
        
        // If apple received, check if message is for node
        if (!message.isEmpty) {
            if (message.destination == nodeId) {
                printf("Node %d has received message: %s\n", nodeId, message.content);

                // Message is now empty
                message.isEmpty = 1;
                strcpy(message.content, "");
            } else {
                printf("Node %d is passing message to destination: Node %d\n", 
                       nodeId, message.destination);
            }
        } else {
            printf("Node %d is passing empty apple to next node\n", nodeId);
        }
        
        // Pass the apple to next node
        write(writePipe, &message, sizeof(Message));
        
        // recusively fork child processes until numNodes reached
        // childProcess 0 -> childProcess 1 -> childProcess 2 -> exit -> ParentProcess
        int status;
        pid_t pid = fork();
        if(pid < 0){
            perror("Fork Failed");
            exit(1);
        }
        if(pid == 0){
            //child process call ChildNodeProcess
            printf("next child process node id = %d\n", nodeId + 1);
            if(nodeId == numNodes){
                printf("Failed to find destination.\n");
                printf("[Node %d] Process terminated\n", nodeId);
                return;
            }
            childNodeProcess(nodeId + 1, readPipe, writePipe);
        }
        else{
            close(readPipe);
            close(writePipe);
            printf("Waiting for child node %d \n", nodeId);
            waitpid(pid, &status, 0);
            printf("Done with node: %d\n", nodeId);      
            return;      
        }

    // printf("[Node %d] Process terminated\n", nodeId);
    // close(readPipe);
    // close(writePipe);
}

// Function for parent process
void parentNodeProcess(int readPipe, int writePipe) { 
    // Implement
  //  while(true){
    int status;
    int nodeId = 0;
    pid_t pid = fork();
    if(pid < 0){
        perror("Fork Failed");
        exit(1);
    }
    if(pid == 0){
        childNodeProcess(nodeId, readPipe, writePipe);
    }
    if(pid > 0){
        waitpid(pid, &status, 0);
        Message message;

        // ask user if they want to send message
        char input;
        printf("Would you like to send a message? y/n ");
        scanf("%c", &input);
        // if user wants to send message
        if(input == 'y'){
            // prompt user for destination node
            int dest;
            printf("Message Destination: ");
            
            while (getchar() != '\n');
            scanf("%d", &dest);

            // prompt user for message
            char getContent[SIZE];
            printf("Input Message: ");
            
            while (getchar() != '\n');
            fgets(getContent, SIZE, stdin);
            while (getchar() != '\n');
            // add to writePipe
            memcpy(message.content, getContent, sizeof(getContent));
            message.destination = dest;
            message.isEmpty = 0;

            write(writePipe, &message, sizeof(Message));

            close(writePipe);
            close(readPipe);
            // initialize node to 0 and call Child process
            parentNodeProcess(readPipe, writePipe);
        }
        // exit gracefully if user doesn't want to send message
        else{
            printf("No more messages.\n");
            raise(SIGINT);
        }
    }// pid > 0

//} //while true
}

int main() {
    // Get number of nodes from user
    printf("Enter number of nodes: ");
    if (scanf("%d", &numNodes) != 1 || numNodes < 2) {
        printf("Invalid number of nodes. Must be at least 2.\n");
        return 1;
    }
    
    // Clear input buffer
    while (getchar() != '\n');
    
    // Setup signal handler
    signal(SIGINT, sigHandler);
    
    // Create pipes
    int pipes[numNodes][2];
    int readPipe = *pipes[0];
    int writePipe = *pipes[1];
    
    // // Finish main function  
   
    // call parent process
    parentNodeProcess(readPipe, writePipe);

}


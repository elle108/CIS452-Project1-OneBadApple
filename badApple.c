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
    
        // Wait to receive the apple
        // if (read(readPipe, &message, sizeof(Message)) <= 0) {
        //     return;
        // }
        
        // Check if this is a shutdown signal
        if (message.destination == -1) {
            printf("Node %d has received shutdown signal.\n", nodeId);
            // Pass shutdown signal to next node
            write(writePipe, &message, sizeof(Message));
            return;
        }
        
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
        
        int status;
        pid_t pid = fork();
        if(pid < 0){
            perror("Fork Failed");
            exit(1);
        }
        if(pid == 0){
            //child process call ChildNodeProcess
            printf("next child process node id = %d\n", nodeId + 1);
            if(nodeId + 1 == numNodes){
                printf("Failed to find destination.\n");
                printf("[Node %d] Process terminated\n", nodeId);
                close(readPipe);
                close(writePipe);
                return;
            }
            childNodeProcess(nodeId + 1, readPipe, writePipe);
        }
        else{
            close(readPipe);
            close(writePipe);
            printf("[Node %d] Process terminated\n", nodeId);
            printf("Waiting for child node %d \n", nodeId);
            waitpid(pid, &status, 0);
            printf("Done with node: %d\n", nodeId);            
        }

    // printf("[Node %d] Process terminated\n", nodeId);
    // close(readPipe);
    // close(writePipe);
}

// Function for parent process
void parentNodeProcess(int nodeId, int readPipe, int writePipe) {
    childNodeProcess(nodeId, readPipe, writePipe);
    // // Implement
    // int status;
    // // create fork n nodes = n child processes 
    // pid_t pid = fork();
    // if(pid < 0){
    //     perror("Fork Failed");
    //     exit(1);
    // }
    // if(pid == 0){
    //     //child process call ChildNodeProcess
    //     printf("next child process node id = %d\n", nodeId + 1);
    //     if(nodeId < numNodes - 1){
    //         parentNodeProcess(nodeId + 1, readPipe, writePipe);
    //     }
    // }
    // else{
        //parent process
        // Parent process prompt for input
        // if(nodeId == numNodes){
        //open(writePipe);
        Message message;

        char input;
        printf("Would you like to send a message? y/n ");
        scanf("%c", &input);
        if(input == 'y'){
            // prompt user for destination node
            int dest;
            printf("Message Destination: ");
            // while(scanf("%d", &dest) > numNodes-1 || scanf("%d", &dest) < 0){
            //     printf("Invalid Destination.\n");
            //     printf("New Message Destination: ");
                scanf("%d", &dest);
            // }

            // prompt user for message
            printf("Input Message: ");
            char getContent[SIZE];
            //int check;
            //while(check = getchar() != '\n'){
            fgets(getContent, SIZE, stdin);
            //}
            memcpy(message.content, getContent, sizeof(getContent));
            message.destination = dest;
            message.isEmpty = 0;

            write(writePipe, &message, sizeof(Message));
            write(1, &message, sizeof(Message));
            close(writePipe);
        }
        else if(input == 'n'){
            printf("No more messages.\n");
            raise(SIGINT);
            return;
        }
        return;
    // } // if nodeId == numNodes

    //     // wait for child process to finish
    //     printf("Waiting for child node %d \n", nodeId);
    //     waitpid(pid, &status, 0);
    //     printf("Done with node: %d\n", nodeId);
        
    // } // if pid > 0
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
    int nodeId = 0;
    // close(readPipe);
    // close(writePipe);
    parentNodeProcess(nodeId, readPipe, writePipe);
    

}

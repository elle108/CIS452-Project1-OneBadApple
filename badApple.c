#include <stdio.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/stat.h> 
#include <sys/ipc.h>  
#include <sys/shm.h> 
#include <unistd.h>
#include <string.h>
#include <signal.h>

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
    
    while (1) {
        // Wait to receive the apple
        if (read(readPipe, &message, sizeof(Message)) <= 0) {
            break;
        }
        
        // Check if this is a shutdown signal
        if (message.destination == -1) {
            printf("Node %d has received shutdown signal.\n", nodeId);
            // Pass shutdown signal to next node
            write(writePipe, &message, sizeof(Message));
            break;
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
    }

    printf("[Node %d] Process terminated\n", nodeId);
    close(readPipe);
    close(writePipe);
}

// Function for parent process
void parentNodeProcess(int readPipe, int writePipe) {
    // Implement
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

    // Finish main function
}
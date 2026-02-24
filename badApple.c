#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>

// Message size
#define SIZE 256

// Struct for message
struct Message {
    // Node message is for
    int destination;
    // Message content
    char content[SIZE];
    // Flag to see if message is empty (1 if empty, 0 if not)
    int isEmpty;
};

// Global Variable for Number of Nodes
int numNodes;

// Signal handler
// Signal handler
void sigHandler (int sigNum) {
    if(sigNum == SIGINT){
        printf ("Exit Gracefully\n");
        exit(0);
    }
}

// Child process function for Nodes 1 to numNodes
void childNodeProcess(int nodeId, int readPipe, int writePipe) {
    struct Message message;
    printf("Created Process for Node %d\n", nodeId);

    while(1){
        // Wait to have the apple 
        if (read(readPipe, &message, sizeof(struct Message)) <= 0) {
            break;
        }

        printf("Node %d has received the apple\n", nodeId);

        // If apple is received, see if there is a message for this Node
        if (message.isEmpty == 0){
            if (message.destination == nodeId) {
                printf("Destination reached! ");
                printf("Node %d has received message: %s\n", nodeId, message.content);
                
                // Change message to empty now
                message.isEmpty = 1;
                strcpy(message.content, "");

                if (nodeId == numNodes - 1){
                    printf("Node %d is passing the empty message to next Node 0\n", nodeId);
                }
                else {
                    printf("Node %d is passing the empty message to next Node %d\n", nodeId, nodeId + 1);
                }
            }
            // If message is not for this Node then pass it along 
            else {
                if (nodeId == numNodes - 1){
                    printf("Node %d is passing the message to next Node 0\n", nodeId);
                }
                else {
                    printf("Node %d is passing the message to next Node %d\n", nodeId, nodeId + 1);
                }
            }
        }
        // If the message is empty, pass it along
        else {
            if (nodeId == numNodes - 1){
                printf("Node %d is passing the empty message to the next Node 0\n", nodeId);
            }
            else{
                printf("Node %d is passing the empty message to the next Node %d\n", nodeId, nodeId+1);
            }
        }

        // Pass the apple to the next node
        write(writePipe, &message, sizeof(struct Message));
    }

    close(readPipe);
    close(writePipe);
}

// Function for parent process
void parentNodeProcess(int readPipe, int writePipe) { 
    struct Message message;
    char input[SIZE];
    int destination;

    printf("Created process for Node 0 (parent node)\n");
    printf("Node 0 will now send the initial empty apple\n");

    // Send the empty apple to start off the process
    message.destination = 0;
    message.isEmpty = 1;
    strcpy(message.content, "");
    write(writePipe, &message, sizeof(struct Message));

    while(1) {

        // Wait to have apple
        read(readPipe, &message, sizeof(struct Message));

        printf("Node 0 has received the apple\n");

        // Check if there is a message for Node 0
        if (message.isEmpty == 0 && message.destination == 0) {
            printf("Destination reached! Node 0 has received message: %s\n", message.content);
            // Change message to empty now
            message.isEmpty = 1;
            strcpy(message.content, "");
        }

        // check if node is not reached for destination nodes out of bounds
        if (message.isEmpty == 0 && message.destination != 0) {
            printf("Failed to to find Node: %d\n", message.destination);
        }

        // Ask user for destination
        printf("Enter destination: ");
        scanf("%d", &destination);
        getchar();
        
        // Ask user for message
        printf("Enter message: ");
        if (fgets(input, SIZE, stdin) == NULL) input[0] = '\0';
        input[strcspn(input, "\n")] = '\0';

        // Send the message
        message.destination = destination;
        strcpy(message.content, input);
        message.isEmpty = 0;

        printf("Node 0 is passing the message to next Node 1\n");
        write(writePipe, &message, sizeof(struct Message));

    }
    close(readPipe);
    close(writePipe);
}

int main() {
    printf("One Bad Apple Project\n");

    // Get number of nodes from user
    printf("Enter number of nodes: ");
    scanf("%d", &numNodes);

    // Clear enter key from input
    while(getchar() != '\n');

    // Setup signal handler
    signal(SIGINT, sigHandler);
    
    // Create pipes
    int pipes[numNodes][2];
    for (int i = 0; i < numNodes; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    // For pipe setup, we created pipes[0] to pipes[numNodes - 1]
    // Pipe [i][1] is write end and Pipe [i][0] is read end
    // For each child, it reads from the previous node so pipe[i-1][0] and writes to pipe[i][1]

    // Create child processes for nodes 1 to numNodes - 1
    for(int i = 1; i < numNodes; i++){
        pid_t pid = fork();
        if(pid < 0){
            perror("Fork Failed");
            exit(1);
        }
        if(pid == 0){
            // Read end of previous node
            int readPipe = pipes[i-1][0];
            // Write end of next node
            int writePipe = pipes[i][1];

            // Close unused pipes
            for (int j = 0; j < numNodes; j++) {
                if (j == i - 1) {
                    // This pipe is for input so close write end
                    close(pipes[j][1]);
                } else if (j == i) {
                    // This pipe is for output so close read end
                    close(pipes[j][0]);
                } else {
                    if (j != i - 1 && j != i) {
                        // Close both ends of unused pipes
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                }
            }
            childNodeProcess(i, readPipe, writePipe);
            exit(0);
        }
    }

    int parentReadPipe = pipes[numNodes - 1][0];
    int parentWritePipe = pipes[0][1];

    // Close unused pipes
    for (int j = 0; j < numNodes; j++) {
        if (j == numNodes - 1) {
            // Parent needs read end of last pipe so close write end
            close(pipes[j][1]);
        } else if (j == 0) {
            // Parent needs the write end so close read end
            close(pipes[j][0]);
        } else {
            // Close both ends of unused pipes
            close(pipes[j][0]);
            close(pipes[j][1]);     
        }
    }

    parentNodeProcess(parentReadPipe, parentWritePipe);

    return 0;
}


# One Bad Apple 
(CIS490-01 Project 1) \
By: Elle and Delaney 
## How to Use
Note: After all prompts for user input type in your answer and hit enter. 
### Step 1
Compile badApple.c with gcc. 
### Step 2 
Run your executable. \
You will be prompted to input a number of nodes. \
\
A statement will print for each process created. \
The processes should be labeled from 0 to the number of nodes minus one. \
On the first run the nodes will pass around an empty message. 
### Step 3
You will be prompted to enter a destination node for your message. \
Then you will be prompted to enter your message. The size limit is 256 characters. \
\
Starting with node 0 the message will be passed to the next node in numerical order. \
When the destination node is reahced it will print the message. \
Afterwards the destination node will pass an empty message to the following node. \
The empty message will be passed around until we reach node 0 again. \
\
Then you will be reprompted to send another message.

### Step 4
To exit the system enter Ctrl+C. \
A shutdown statement will print for every node. 

## Design
### main
This function creates n number of processes for n nodes the user inputs. \
The parent process is node 0 and the child proceses are nodes 1 though n - 1.\
Each of these processes communicates via pipes with each process reading from the previous process and writing to the next. \
All pipes that are currently not reading or writing will be closed in both parent and child processes.  \
The child processes call childNodeProcess and pass the node number, the read pipe from the previous node, and the current node's writepipe as arguments to it. \
The parent process calls parentNodeProcess and passes the previous node's read pipe and write pipe for Node 0 as arguments to it. \

### parentNodeProcess
This function creates an empty message and sends it around to all the nodes in order to spawn all the processes. \
Then in a loop this function will prompt the user for a destination node and message. \
If the destination node is 0, then the message will be passed to all the nodes before it gets return to the parent process, node 0, and printed. \
If the destination node does not exist a statement indicating so prints to the terminal. \
If a Ctrl+C signal is raised all the processes will exit gracefully.

### childNodeProcess
This function reads the message and destination input from the previous node. \
If the current node is the destination then the function prints the message. \
After the destination is reached and empty messages is passed around. \
If the current node is not the destination then it writes to its write pipe the destination and message for the next node to read. \
A statement prints for every node, which tells the user which node currently has read and write permissions (the "apple"). \
A statement also prints telling the user which node is gets the "apple" next.




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




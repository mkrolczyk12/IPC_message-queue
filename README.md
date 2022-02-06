
## Table of contents

- [Description](#description)
- [Status](#status)
- [Contact](#contact)


## Description

### Inter-process communication (IPC) with use of linux message queue and threads

The project consists of two programs that run independently: the server and the client. <br />

The client process sends a string to the server process. The server receives this string and processes it
in it changing all letters to uppercase, and then outputs the string z
back to the client. The client receives the processed string and prints it to the screen. By using
the mechanism of the message queue, the above task should be implemented with the ability to support multiple clients
at the same time. Use one queue in the solution messages. Use appropriate message labeling to distinguish between the queue
data for the server and data for individual clients. <br />

The server creates a message queue and waits for messages from clients. The server removes the queue after
receiving a user-defined signal (eg SIGINT). <br />

The client is a multi-threaded application. Thread 1 is responsible for sending messages and thread 2 for receiving messages from server.
Threads work asynchronously, for example: thread 1 can send several messages before thread 2 receives any feedback from the server. <br />

There is also a need to handle a queue overflow and a message overflow error. <br />

For system functions - implement error handling based on the perror() function and the errno variable. 

## Status

_completed_

## Contact

Created by @mkrolczyk12 - feel free to contact me!

- E-mail: m.krolczyk66@gmail.com

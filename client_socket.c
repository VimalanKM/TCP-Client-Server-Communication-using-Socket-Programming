/*
A C program that creates, initializes, and connects a client socket to a server socket. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h> - Unavailable on Windows - Used in Linux
#include <winsock2.h>
#include <ws2tcpip.h>


#define BUFFER_SIZE 1024

//Getting server information through command-line arguments
int main(int argc, char *argv[]){

    //Checking for required command-line arguments
    if(argc != 3){
        fprintf(stderr, "Usage %s <Server Address> <Port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Initializing Winsock - Windows method
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        fprintf(stderr, "Winsock initialization failed\n");
        return EXIT_FAILURE;
    }

    const char* server_address = argv[1];
    int port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0){
        perror("Socket Creation Failed");
        WSACleanup();
        return EXIT_FAILURE;
    }

    //Defining server address structure
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    //Converting server address to binary format
    if (inet_pton(AF_INET, server_address, &server.sin_addr) <= 0){
        perror("Invalid server address");
        close(client_socket);
        return EXIT_FAILURE;
    }

    //Connecting to the server
    if(connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Connection Failed");
        close(client_socket);
        return EXIT_FAILURE;
    }

    //Connection successful
    printf("Connected to server %s on port %d\n", server_address, port);

    //Messages can be sent or received from the server
    char buffer[BUFFER_SIZE];
    while(1){
        printf("Type your message to be sent to the server (type exit to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; //Removing newline character

        if(strcmp(buffer, "exit") == 0){
            //End of messages
            break;
        }

        send(client_socket, buffer, strlen(buffer), 0);

        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE-1, 0);

        if(bytes_received > 0){
            buffer[bytes_received] = '\0';
            printf("Server: %s\n", buffer);
        }
        else{
            printf("Server Disconnected \n");
            break;
        }
    }

    //Closing the socket
    close(client_socket);
    printf("Connection closed\n");
    
    // Cleanup Winsock
    WSACleanup();  

    return EXIT_SUCCESS;
}
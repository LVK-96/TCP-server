#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

int connection_handler(void *socket_desc) {
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[2000];
     
    //Receive a message from client
    int fail_counter = 0;
    while(1) {
        read_size = recv(sock , client_message , 2000 , 0);
        if (read_size < 0) {
            fail_counter++;
            if (fail_counter > 14) {break;}
        }     
        else {
            fail_counter = 0;
            //Print recieved data 
            printf("%s\n", client_message);
            fflush(stdout);
        } 
    }
    if(read_size == -1) {
        perror("recv failed");
    }
    //Free the socket pointer
    free(socket_desc);
    puts("socket free");
    return 0;
}

int main()
{
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    
    //set socket options
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int reuse_opt = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse_opt, sizeof(reuse_opt)); //reuse enabled
    setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)); //set timeout

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while(1) {
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (new_socket<0) {
            continue;
        }
        puts("Connection accepted");
	    new_sock = malloc(1);
        *new_sock = new_socket;
        connection_handler(new_sock);
        close(new_socket);
        exit(0);        
    }
    return 0;
}
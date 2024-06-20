#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
    // Disable output buffering to ensure that all log messages are immediately printed to standard output and error.
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // Print an initial log message for debugging purposes.
    printf("Logs from your program will appear here!\n");

    int server_fd, client_addr_len;
    struct sockaddr_in client_addr;
    
    // Create a new socket of type TCP (SOCK_STREAM) within the Internet Protocol (IPv4, AF_INET).
    // The third parameter '0' automatically chooses the appropriate protocol (TCP for SOCK_STREAM).
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return 1;
    }
    
    // Configure the socket to allow reuse of local addresses with SO_REUSEADDR.
    // This is useful in development to avoid "Address already in use" errors after restarts.
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        return 1;
    }
    
    // Initialize the server address structure with zero and set its parameters.
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,          // Internet address family.
        .sin_port = htons(4221),        // Local port to listen on, converted to network byte order.
        .sin_addr = { htonl(INADDR_ANY) } // Listen on all local interfaces.
    };
    
    // Bind the socket to the specified IP address and port.
    if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("Bind failed: %s \n", strerror(errno));
        return 1;
    }
    
    // Listen on the socket for incoming connections, with a maximum backlog of 5 pending connections.
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        printf("Listen failed: %s \n", strerror(errno));
        return 1;
    }
    
    // Notify that the server is now waiting for a client to connect.
    printf("Waiting for a client to connect...\n");
    client_addr_len = sizeof(client_addr);
        
    // Accept a connection from a client. This call blocks until a client connects.
    // The accept function returns a new socket file descriptor for the accepted connection.
    int fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (fd < 0) {
        printf("Accept failed: %s \n", strerror(errno));
        return 1;
    }
    printf("Client connected\n");

    // Prepare an HTTP response message to send to the client.
    char *reply = "HTTP/1.1 200 OK\r\n\r\n";
    
    // Send the HTTP response to the client over the accepted socket.
    int bytes_sent = send(fd, reply, strlen(reply), 0);
    if (bytes_sent < 0){

    	printf("Send failed\n");
    	return 1;
    }

   

    // Close the server socket and terminate the program.
    close(server_fd);

    return 0;
}

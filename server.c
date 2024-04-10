#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_unix_socket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[256];

    // Create a socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(server_fd, 5);
    printf("Server listening... pid=%d\n", getpid());

    // Accept a connection
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    struct ucred cred;
    uint len = sizeof(cred);
    int ret = getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &cred, &len);
    if (ret < 0) {
        perror("Error getsockopt SO_PEERCRED");
        exit(EXIT_FAILURE);
    }
    printf("client cred: pid=%d, euid=%d, egid=%d\n", cred.pid, cred.uid, cred.gid);

    // Receive data from the client
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("Received message from client: %s\n", buffer);

    // Clean up
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    exit(EXIT_SUCCESS);
}

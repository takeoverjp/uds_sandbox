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
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[256];

    printf("Client connecting... pid=%d\n", getpid());

    // Create a socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting");
        exit(EXIT_FAILURE);
    }

    struct ucred cred;
    uint len = sizeof(cred);
    int ret = getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &cred, &len);
    if (ret < 0) {
        perror("Error getsockopt SO_PEERCRED");
        exit(EXIT_FAILURE);
    }
    printf("server cred: pid=%d, euid=%d, egid=%d\n", cred.pid, cred.uid, cred.gid);

    // Send a message to the server
    strcpy(buffer, "Hello from client!");
    send(client_fd, buffer, strlen(buffer), 0);

    // Clean up
    close(client_fd);

    return 0;
}

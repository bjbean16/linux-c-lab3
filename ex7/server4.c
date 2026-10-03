#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS];
int client_count = 0;

void broadcast(int sender, const char *msg) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i] != sender) {
            write(clients[i], msg, strlen(msg));
        }
    }
}

void handle_client(int client_sock) {
    char buffer[1024];
    int n;
    while ((n = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("Client %d: %s\n", client_sock, buffer);
        broadcast(client_sock, buffer);
    }
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client_sock) {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
    close(client_sock);
    exit(0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    printf("Broadcast server listening on port %d\n", PORT);
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = client_sock;
        }
        if (fork() == 0) {
            close(server_sock);
            handle_client(client_sock);
        }
    }
    return 0;
}

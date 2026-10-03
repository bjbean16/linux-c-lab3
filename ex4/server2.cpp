#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/shm.h>
#include <cstdlib>

#define PORT 8080
#define SHM_KEY 1234

void handle_client(int client_sock, int *counter) {
    char buffer[1024];
    read(client_sock, buffer, sizeof(buffer));
    std::cout << "Received: " << buffer << std::endl;
    write(client_sock, "Hello from C++ server", 22);
    close(client_sock);
    __sync_fetch_and_sub(counter, 1);
    exit(0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    int shmid = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    int *counter = (int*)shmat(shmid, NULL, 0);
    *counter = 0;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    std::cout << "Server listening on port " << PORT << std::endl;
    while (true) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        __sync_fetch_and_add(counter, 1);
        std::cout << "Active clients: " << *counter << std::endl;
        if (fork() == 0) {
            close(server_sock);
            handle_client(client_sock, counter);
        }
    }
    return 0;
}

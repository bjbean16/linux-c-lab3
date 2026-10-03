#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#define PORT 8080

void log_error(const char *msg) {
    FILE *log = fopen("server_errors.log", "a");
    time_t now = time(NULL);
    char *ts = ctime(&now);
    ts[strcspn(ts, "\n")] = '\0';
    fprintf(log, "[%s] %s\n", ts, msg);
    fclose(log);
}

void handle_client(int client_sock) {
    char buffer[1024];
    if (read(client_sock, buffer, sizeof(buffer)) < 0) {
        log_error("read() failed");
    } else {
        std::cout << "Received: " << buffer << std::endl;
        if (write(client_sock, "Hello from C++ server", 22) < 0) {
            log_error("write() failed");
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
    if (server_sock < 0) { log_error("socket() failed"); return 1; }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("bind() failed");
        return 1;
    }
    if (listen(server_sock, 5) < 0) { log_error("listen() failed"); return 1; }

    std::cout << "Server listening on port " << PORT << std::endl;
    while (true) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) { log_error("accept() failed"); continue; }
        pid_t pid = fork();
        if (pid < 0) {
            log_error("fork() failed");
            close(client_sock);
            continue;
        }
        if (pid == 0) {
            close(server_sock);
            handle_client(client_sock);
        }
    }
    return 0;
}

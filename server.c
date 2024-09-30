//Implemented by Devon Villalona
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Function to process the math expression
double process_math_expression(char* expression) {
    double x, y;
    char op;
    
    // Parse the math expression "x op y="
    sscanf(expression, "%lf %c %lf", &x, &op, &y);

    // Perform the appropriate operation
    switch(op) {
        case '+': return x + y;
        case '-': return x - y;
        case '*': return x * y;
        case '/': 
            if (y == 0) {
                printf("Division by zero error.\n");
                return 0;
            }
            return x / y;
        default: return 0;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int port = 5555;  // Ensure this matches the client port

    // Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR to avoid "Address already in use" error
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an IP address and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Socket bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) != 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    // Accept a connection from a client
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Server accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected by client on %s\n", inet_ntoa(client_addr.sin_addr));

    // Communication loop
    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Receive the data from the client
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        printf("Received question: %s", buffer);

        // Check if the client sent "0/0=" to terminate
        if (strncmp(buffer, "0/0=", 4) == 0) {
            printf("Received question '0/0='; end the server program.\n");
            break;
        }

        // Process the math expression and calculate the result
        double result = process_math_expression(buffer);
        
        // Limit the result to 2 decimal places
        sprintf(buffer, "%.2lf\n", result);

        // Send the result back to the client
        send(client_fd, buffer, strlen(buffer), 0);
        printf("Sent back answer: %s", buffer);
    }

    // Close the sockets
    close(client_fd);
    close(server_fd);
    printf("Server closed.\n");

    return 0;
}

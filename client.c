//Implemented by Devon Villalona
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int port = 5555;  // Ensure this matches the server port

    // Create a TCP socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Specify the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  // Set the port (5555, ensure it matches the server)

    // Use the IP address obtained from ping (e.g., 10.173.204.158)
    if (inet_pton(AF_INET, "10.173.204.158", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Connection to the server failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected with server on %s\n", inet_ntoa(server_addr.sin_addr));

    // Communication loop
    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Get math expression from the user
        printf("Enter math question (or '0/0=' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send the expression to the server
        send(client_fd, buffer, strlen(buffer), 0);

        // Check if the input is "0/0=" to terminate
        if (strncmp(buffer, "0/0=", 4) == 0) {
            printf("User input ends; end the client program.\n");
            break;
        }

        // Receive the result from the server
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        printf("Answer from server: %s", buffer);
    }

    // Close the socket
    close(client_fd);
    printf("Client closed.\n");

    return 0;
}

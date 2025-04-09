/*
 * File: client.cpp
 * Author: Johnny CW
 * Date: April 8, 2025
 * Connects to a server, sends messages, and receives encrypted responses
 */
#define USE_IPV6 true

#if defined _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#define WSVERS MAKEWORD(2, 2)
WSADATA wsadata;
#else
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#endif

#include "../common/caesar_cipher.h"

#define DEFAULT_PORT "1234"
#define BUFFER_SIZE 1024
#define SEGMENT_SIZE 70

using namespace std;

void printBuffer(const char *header, const char *buffer) {
    cout << "------" << header << "------" << endl;
    for (size_t i = 0; i < strlen(buffer); i++) {
        if(buffer[i] == '\r'){
            cout << "buffer[" << i << "]=\\r" << endl;
        }else if(buffer[i] == '\n'){
            cout << "buffer[" << i << "]=\\n" << endl;
        }else{
            cout << "buffer[" << i << "]=" << buffer[i] << endl;
        }
    }
    cout << "---" << endl;
}

int main(int argc, char *argv[]){
    char portNum[12];
#if defined _WIN32
    SOCKET s = INVALID_SOCKET;
#else
    int s = -1;
#endif
    char send_buffer[BUFFER_SIZE], recv_buffer[BUFFER_SIZE];
    int bytes;
    char serverHost[NI_MAXHOST], serverService[NI_MAXSERV];

    // Winsock Initialization (Windows only)
#if defined _WIN32
    int err = WSAStartup(WSVERS, &wsadata);
    if(err != 0){
        printf("WSAStartup failed with error: %d\n", err);
        WSACleanup();
        return 1;
    }
    if(LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2){
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return 1;
    }
    cout << "Winsock 2.2 initialized.\n";
#endif

    cout << "\n<<<Caesar Cipher TCP Client>>>\n";
    if(USE_IPV6){
        cout << "IPv6 mode enabled.\n";
    }else{
        cout << "IPv4 mode enabled.\n";
    }

    // Set up server address
    struct addrinfo hints, *result = nullptr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = USE_IPV6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Handle command-line arguments
    if(argc == 3){
        strncpy(portNum, argv[2], sizeof(portNum)-1);
        portNum[sizeof(portNum)-1] = '\0';
        cout << "Connecting to " << argv[1] << " on port " << portNum << endl;
        if (getaddrinfo(argv[1], portNum, &hints, &result) != 0) {
            printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
#if defined _WIN32
            WSACleanup();
#endif
            return 1;
        }
    }else{
        cout << "Using default: 127.0.0.1:" << DEFAULT_PORT << endl;
        strncpy(portNum, DEFAULT_PORT, sizeof(portNum)-1);
        portNum[sizeof(portNum)-1] = '\0';
        if (getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result) != 0) {
            printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
#if defined _WIN32
            WSACleanup();
#endif
            return 1;
        }
    }

    // Create socket
    s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(s == INVALID_SOCKET || s < 0){
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
#if defined _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Connect to the server
    if(connect(s, result->ai_addr, result->ai_addrlen) != 0){
        printf("connect failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
#if defined _WIN32
        closesocket(s);
        WSACleanup();
#endif
        return 1;
    }

    // Display server info
    getnameinfo(result->ai_addr, result->ai_addrlen, serverHost, sizeof(serverHost),
                serverService, sizeof(serverService), NI_NUMERICHOST);
    cout << "Connected to server at " << serverHost << ":" << portNum << endl;
    freeaddrinfo(result);

    // Interaction loop
    while (true) {
        memset(send_buffer, 0, sizeof(send_buffer));
        cout << "Type here: ";
        if(!fgets(send_buffer, SEGMENT_SIZE, stdin)){
            printf("Input Error\n");
            break;
        }

        if (strncmp(send_buffer, ".", 1) == 0) {
            break;
        }

        send_buffer[strcspn(send_buffer, "\n")] = 0;
        strcat(send_buffer, "\r\n");

        //debug
        //printBuffer("Sending", send_buffer);

        bytes = send(s, send_buffer, strlen(send_buffer), 0);
        if(bytes <= 0){
            printf("send failed with error: %d\n", WSAGetLastError());
            break;
        }
        cout << "Message Sent --->>>: " << send_buffer << endl;

        memset(recv_buffer, 0, BUFFER_SIZE);
        bytes = recv(s, recv_buffer, BUFFER_SIZE - 1, 0);
        if(bytes <= 0){
            printf("Receive failed or connection closed: %d\n", WSAGetLastError());
            break;
        }
        // Strip \r\n and null terminate
        for(int i = 0; i < bytes; i++){
            if(recv_buffer[i] == '\n'){
                recv_buffer[i] = '\0';
                if (i > 0 && recv_buffer[i - 1] == '\r'){
                    recv_buffer[i - 1] = '\0';
                }
                break;
            }
        }

        if(bytes <= 0){
            break;
        }
        cout << "Message Received --->>>: " << recv_buffer << "\n\n";
    }

    // Cleanup
    cout << "\nShutting down ...\n";
#if defined _WIN32
    closesocket(s);
    WSACleanup();
#else
    close(s);
#endif
    return 0;
}
/*
* File: server.cpp
 * Author: Johnny CW
 * Date: April 8, 2025
 * Description: TCP Server for the Caesar Cipher Encryption Service
 * Listens for client connections, encrypts messages, and sends back ciphertext
 */
#define USE_IPV6 true // Toggle between IPv6 (true) and IPv4 (false)

#if defined _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#define WSVERS MAKEWORD(2, 2) // Winsock version 2.2
WSADATA wsadata; // Winsock data structure
#else // Unix-like systems
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

using namespace std;

void printBuffer(const char *header, const char *buffer) {
    cout << "------" << header << "------" << endl;
    for (size_t i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\r') cout << "buffer[" << i << "]=\\r" << endl;
        else if (buffer[i] == '\n') cout << "buffer[" << i << "]=\\n" << endl;
        else cout << "buffer[" << i << "]=" << buffer[i] << endl;
    }
    cout << "---" << endl;
}


int main(int argc, char *argv[]) {
    /*Step 1 : Initialize Variables*/
    struct sockaddr_storage clientAddress;
    char clientHost[NI_MAXHOST], clientService[NI_MAXSERV];
#if defined _WIN32
    SOCKET s = INVALID_SOCKET, ns = INVALID_SOCKET; //Listening and client sockets
#else
    int s = -1, ns = -1;
#endif
    char send_buffer[BUFFER_SIZE], recv_buffer[BUFFER_SIZE];
    int bytes, addrlen;
    char portNum[12];

    /*Step 2: Winsock Initialization (Only for Windows)*/
#if defined _WIN32
    int err = WSAStartup(WSVERS, &wsadata);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        WSACleanup();
        return 1;
    }
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return 1;
    }
    cout << "Winsock 2.2 initialized.\n";
#endif

    cout << "\n<<<Caesar Cipher TCP Server>>>\n";
    if (USE_IPV6) {
        cout << "IPv6 mode enabled.\n";
    }else {
        cout << "IPv4 mode enabled.\n";
    }

    /*Step 3: Set Up Server Address*/
    struct addrinfo hints, *result = nullptr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = USE_IPV6 ? AF_INET6 : AF_INET; //Set IPv6 or IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP Stream Socket
    hints.ai_protocol = IPPROTO_TCP; // The TCP Protocol
    hints.ai_flags = AI_PASSIVE;

    if (argc==2) {
        strncpy(portNum, argv[1], sizeof(portNum)-1);
        portNum[sizeof(portNum)-1]='\0';
    }else {
        strncpy(portNum, DEFAULT_PORT, sizeof(portNum)-1);
        portNum[sizeof(portNum)-1]='\0';
        cout << "Using default port number: " << DEFAULT_PORT << endl;
    }

    if (getaddrinfo(nullptr, portNum, &hints, &result) != 0) {
        printf("getaddrinfo failed with error: %d\n", WSAGetLastError());
#if defined _WIN32
        WSACleanup();
#endif
        return 1;
    }

    /*Step 4: Create the Listening Socket*/
    s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (s == INVALID_SOCKET || s < 0) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
#if defined _WIN32
        WSACleanup();
#endif
        return 1;
    }

    /*Step 5: Bind Socket*/
    if (bind(s, result->ai_addr, result->ai_addrlen) != 0) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
#if defined _WIN32
        WSACleanup();
#endif
        return 1;
    }

    /*Step 6: Listen for connections*/
    if (listen(s, SOMAXCONN) != 0) {
        printf("listen failed with error: %d\n", WSAGetLastError());
#if defined _WIN32
        closesocket(s);
        WSACleanup();
#endif
        return 1;
    }
    cout << "Server listening on port " << portNum << "...\n";

    /*Step 7: Main Loop to accept clients*/
    while (true) {
        addrlen = sizeof(clientAddress);
        ns = accept(s, (struct sockaddr *)&clientAddress, &addrlen);
        if (ns == INVALID_SOCKET || ns < 0) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            continue;
        }

        //Client info
        getnameinfo((struct sockaddr *)&clientAddress, addrlen, clientHost, sizeof(clientHost),
                    clientService, sizeof(clientService), NI_NUMERICHOST);
        cout << "\nClient Connected: " << clientHost << ":" << clientService << "\n";

        /* Step 8: Client interaction loop */
        while (true) {
            memset(recv_buffer, 0, BUFFER_SIZE);
            bytes = recv(ns, recv_buffer, BUFFER_SIZE - 1, 0);
            if(bytes <= 0){ cout << "Client disconnected.\n"; break;}
            for (int i = 0; i < bytes; i++) {
                if(recv_buffer[i] == '\n') {
                    recv_buffer[i] = '\0';
                    if(i){
                        recv_buffer[i-1] = '\0';
                    }
                    break;
                }
            }
            // Debug
            //printBuffer("RECEIVE_BUFFER", recv_buffer);
            cout << "Message received <<<---: " << recv_buffer << "\n";

            /* Step 9: Process Caesar Cipher */
            int shift = 3;           // Default shift
            string text = recv_buffer;
            string result;           // Will hold the encrypted or decrypted text
            bool is_decrypt = false;

            // Parse the input
            string command;
            size_t first_space = text.find(' ');
            if (first_space != string::npos) {
                command = text.substr(0, first_space);
                text = text.substr(first_space + 1);

                // Check if command is encrypt or decrypt
                if (command == "encrypt") {
                    // Do nothing
                } else if (command == "decrypt") {
                    is_decrypt = true;
                } else {
                    text = recv_buffer; // Reset text to full input
                    command.clear();
                }
            } else {
                command.clear(); //treat as default
            }

            // Extract shift if provided
            if(!command.empty() && text.find(' ') != string::npos) {
                size_t shift_end = text.find(' ');
                if(shift_end != string::npos && isdigit(text[0])) {
                    shift = stoi(text.substr(0, shift_end));
                    text = text.substr(shift_end + 1);
                }
            }else if(command.empty() && isdigit(text[0])){
                size_t shift_end = text.find(' ');
                if(shift_end != string::npos){
                    shift = stoi(text.substr(0, shift_end));
                    text = text.substr(shift_end + 1);
                }
            }

            // Apply encryption or decryption
            if (is_decrypt){
                result = caesar_decrypt(text, shift);
                snprintf(send_buffer, BUFFER_SIZE, "Decrypted with shift %d: %s\r\n", shift, result.c_str());
            }else{
                result = caesar_encrypt(text, shift);
                snprintf(send_buffer, BUFFER_SIZE, "Encrypted with shift %d: %s\r\n", shift, result.c_str());
            }

            // Send response
            bytes = send(ns, send_buffer, strlen(send_buffer), 0);
            if(bytes <= 0){
                printf("send failed with error: %d\n", WSAGetLastError());
                break;
            }
            cout << "Message sent: " << send_buffer << "\n";
        }
}

        /* Step 10: Close Client Socket */
        cout << "Disconnected from " << clientHost << ":" << clientService << "\n";
#if defined _WIN32
        closesocket(ns);
#else
        close(ns);
#endif
        return 0;
    }


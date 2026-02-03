#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <random>
#include <windows.h>
#include "transfer_data.hpp"

#define FROM 1
#define TO 10
#define MAX_TRIES 3

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 10);

    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        printf("WSAStartup failed: %d\n", wsaResult);
        return 1;
    }

    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket created successfully.\n");

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(8080);

    int iResult = connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        printf("Connect failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
    printf("Connected to server successfully.\n");

    transfer_data_client tdc = {false, FROM, TO, MAX_TRIES};
    transfer_data_server tds;
    char sendBuffer[200];
    char recvBuffer[200];
    int tries = 0;
    int correct_number = dist(gen);
    std::cout << "Number to guess: " << correct_number << std::endl;
    while(true){
        memcpy(sendBuffer, &tdc, sizeof(tdc));
        Sleep(1000);
        iResult = send(connectSocket, sendBuffer, 200, 0);
        
        if((tries == MAX_TRIES) || tdc.guessed){
             std::cout <<"Press enter to exit\n";
             std::cin.get();
             break;
        }

        if (iResult == SOCKET_ERROR) {
            printf("Send failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }

        iResult = recv(connectSocket, recvBuffer, 200, 0);
        if(iResult > 0){
            memcpy(&tds, recvBuffer, sizeof(tds));
            tries++;
            tdc.tries = MAX_TRIES - tries;
            
            if(tds.guess == correct_number){
                std::cout << "Correct!\n";
                tdc.guessed = true;
                tdc.from = tries;
                tdc.to = correct_number;
            }else if(tds.guess > correct_number){
                std::cout << "Lower\n";
                tdc.to = tds.guess - 1;
            }else if(tds.guess < correct_number){
                std::cout << "Higher\n";
                tdc.from = tds.guess + 1;
            }
            
            if((tries == MAX_TRIES) && !tdc.guessed){
                std::cout << "No more tries left\n";
                tdc.guessed = false;
                tdc.from = tries;
                tdc.to = correct_number;
            }
        }
    }

    closesocket(connectSocket);
    WSACleanup();
    return 0;
}
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <random>
#include <windows.h>
#include "transfer_data.hpp"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    

    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        printf("WSAStartup failed: %d\n", wsaResult);
        return 1;
    }
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(8080);

    if (listenSocket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created successfully.\n");
    int iResult = bind(listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }
    printf("Bind successful.\n");
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }
    printf("Listening on socket.\n");

    SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);
    if (acceptedSocket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }
    printf("Client connected successfully.\n");

    char recvBuffer[200];
    char sendBuffer[200];
    transfer_data_client tdc;
    transfer_data_server tds;
    while(true){

        iResult = recv(acceptedSocket, recvBuffer, 200, 0);
        if (iResult > 0) {
            memcpy(&tdc, recvBuffer, sizeof(tdc));
            
            if(tdc.guessed){
                std::cout << "Upsesno pogodjen broj " << tdc.to << " u " << tdc.from << " pokusaja!\n";
                std::cout <<"Press enter to exit\n";
                std::cin.get();
                break;
            }else if(!tdc.tries){
                std::cout << "Neuspesno pogodjen broj " << tdc.to << " u " << tdc.from << " pokusaja\n";
                std::cout <<"Press enter to exit\n";
                std::cin.get();
                break;
            }
            std::cout << "Broj je izmedju " << tdc.from << " i " << tdc.to << "\nJos " << tdc.tries << " pokusaja\n";

            std::uniform_int_distribution<> dist(tdc.from, tdc.to);
            tds.guess = dist(gen);

            std::cout << "Pogadjam: " << tds.guess << std::endl; 
            
            memcpy(sendBuffer, &tds, sizeof(tds));
            
            Sleep(1000);
            iResult = send(acceptedSocket, sendBuffer, 200, 0);

            if (iResult == SOCKET_ERROR) {
            printf("Send failed with error: %d\n", WSAGetLastError());
            closesocket(acceptedSocket);
            WSACleanup();
            return 1;
        }
        }
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
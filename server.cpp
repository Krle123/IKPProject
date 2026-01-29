#include <stdio.h>
#include <winsock2.h>

int main() {

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

    char recvBuffer[512];
    iResult = recv(acceptedSocket, recvBuffer, sizeof(recvBuffer), 0);
    if (iResult > 0) {
        printf("Received %d bytes: %.*s\n", iResult, iResult, recvBuffer);
    } else if (iResult == 0) {
        printf("Connection closing...\n");
    } else {
        printf("Recv failed with error: %d\n", WSAGetLastError());
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
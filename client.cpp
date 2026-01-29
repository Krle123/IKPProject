#include <stdio.h>
#include <winsock2.h>

int main() {

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

    iResult = send(connectSocket, "Hello, Server!", 14, 0);
    if (iResult == SOCKET_ERROR) {
        printf("Send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
    printf("Sent %d bytes to server.\n", iResult);
    closesocket(connectSocket);
    WSACleanup();
    return 0;
}
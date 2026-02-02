#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <fstream>
#include <conio.h>
#include "transfer_data.hpp"
#include "lista/mlist.hpp"

#define MAX_CLIENTS 8

List<SOCKET> clients;
CRITICAL_SECTION clients_lock;
CRITICAL_SECTION stats_lock;

volatile bool server_running = true;

int total_wins = 0;
int total_losses = 0;

void log_result(bool win) {
    EnterCriticalSection(&stats_lock);

    if (win)
        total_wins++;
    else
        total_losses++;

    std::ofstream file("results.txt", std::ios::app);
    file << (win ? "WIN\n" : "LOSS\n");
    file.close();

    std::cout << "Wins: " << total_wins
              << " | Losses: " << total_losses << std::endl;

    LeaveCriticalSection(&stats_lock);
}

DWORD WINAPI client_thread(LPVOID param) {
    SOCKET client = (SOCKET)param;

    u_long mode = 1;
    ioctlsocket(client, FIONBIO, &mode);

    fd_set readfds;
    timeval tv;

    char recvBuffer[200];
    char sendBuffer[200];

    transfer_data_client tdc;
    transfer_data_server tds;

    std::random_device rd;
    std::mt19937 gen(rd());

    bool game_finished = false;
    bool win = false;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(client, &readfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int res = select(0, &readfds, NULL, NULL, &tv);
        if (res <= 0) continue;

        if (FD_ISSET(client, &readfds)) {
            int bytes = recv(client, recvBuffer, 200, 0);
            if (bytes <= 0) break;

            memcpy(&tdc, recvBuffer, sizeof(tdc));

            if (tdc.guessed) {
                game_finished = true;
                win = true;
                break;
            }

            if (!tdc.tries) {
                game_finished = true;
                win = false;
                break;
            }

            std::uniform_int_distribution<> dist(tdc.from, tdc.to);
            tds.guess = dist(gen);

            memcpy(sendBuffer, &tds, sizeof(tds));
            send(client, sendBuffer, 200, 0);
        }
    }

    if (game_finished)
        log_result(win);

    EnterCriticalSection(&clients_lock);
    if (clients.len() > 0)
        clients.pop_front();
    LeaveCriticalSection(&clients_lock);

    closesocket(client);
    return 0;
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    InitializeCriticalSection(&clients_lock);
    InitializeCriticalSection(&stats_lock);

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    bind(listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
    listen(listenSocket, SOMAXCONN);

    u_long mode = 1;
    ioctlsocket(listenSocket, FIONBIO, &mode);

    fd_set readfds;
    timeval tv;

    std::ofstream file("results.txt", std::ios::app);
    file << "\nServer started\n";
    file.close();

    while (server_running) {
        if (_kbhit()) {
            char c = _getch();
            if (c == '1') {
                server_running = false;
                break;
            }
        }


        FD_ZERO(&readfds);
        FD_SET(listenSocket, &readfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int res = select(0, &readfds, NULL, NULL, &tv);
        if (res <= 0) continue;

        if (FD_ISSET(listenSocket, &readfds)) {
            SOCKET client = accept(listenSocket, NULL, NULL);
            if (client == INVALID_SOCKET) continue;

            EnterCriticalSection(&clients_lock);
            if (clients.len() < MAX_CLIENTS) {
                clients.push_back(client);
                CreateThread(NULL, 0, client_thread, (LPVOID)client, 0, NULL);
            } else {
                closesocket(client);
            }
            LeaveCriticalSection(&clients_lock);
        }
    }

    DeleteCriticalSection(&stats_lock);
    DeleteCriticalSection(&clients_lock);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
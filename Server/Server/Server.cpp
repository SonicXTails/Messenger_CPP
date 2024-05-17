#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult = nullptr;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    char recvBuffer[512];

    const char* sendBuffer = "Hello from server";

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Настройка структуры addrinfo
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE; // Используется для серверного сокета

    // Получение адресной информации для прослушивания на порту 666
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создание слушающего сокета
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязка сокета к адресу и порту
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Прослушивание входящих соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Listen failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принятие входящего соединения
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Accept failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Обработка сообщений клиента
    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ClientSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes: " << recvBuffer << endl;

            // Отправка ответа клиенту
            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Send failed with error: " << WSAGetLastError() << endl;
                closesocket(ClientSocket);
                closesocket(ListenSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        } else if (result == 0) {
            cout << "Connection closing" << endl;
        } else {
            cout << "Receive failed with error: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            closesocket(ListenSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Закрытие соединения
    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed with error: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Освобождение ресурсов
    closesocket(ClientSocket);
    closesocket(ListenSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}

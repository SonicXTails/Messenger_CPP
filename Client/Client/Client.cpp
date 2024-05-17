#define WIN32_LEAN_AND_MEAN

#include <Windows.h> // Основной заголовочный файл для Windows API.
#include <iostream>
#include <WinSock2.h> // Заголовочный файл для использования сокетов в Windows.
#include <WS2tcpip.h> // Заголовочный файл для функций и структур для TCP/IP.

using namespace std;

int main() {
    WSADATA wsaData;
    ADDRINFO hints; 
    ADDRINFO* addrResult = nullptr; 
    SOCKET ConnectSocket = INVALID_SOCKET;
    char recvBuffer[512]; // Буфер для хранения полученных данных.

    const char* sendBuffer1 = "Hello from client, message 1"; // Первое сообщение для отправки.
    const char* sendBuffer2 = "Hello from client, message 2"; // Второе сообщение для отправки.

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализирует библиотеку Winsock.
    if (result != 0) { // Проверка на успешную инициализацию.
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Настройка структуры addrinfo
    ZeroMemory(&hints, sizeof(hints)); // Заполнение структуры нулями.
    hints.ai_family = AF_INET; // Использовать IPv4.
    hints.ai_socktype = SOCK_STREAM; // Установить тип сокета на потоковый (TCP).
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP.

    // Получение адресной информации для подключения к серверу
    result = getaddrinfo("localhost", "666", &hints, &addrResult); // Получает адресную информацию для подключения к "localhost" на порт "666".
    if (result != 0) { // Проверка на успешное выполнение функции.
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup(); // Освобождает ресурсы Winsock.
        return 1; // Выход с ошибкой, если получение информации не удалось.
    }

    // Создание сокета
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создание сокета с полученными параметрами.
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на успешное создание сокета.
        cout << "Socket creation failed with error: " << WSAGetLastError() << endl; // Вывод кода ошибки.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Освобождение ресурсов Winsock.
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Подключение к серверу.
    if (result == SOCKET_ERROR) { // Проверка на успешное подключение.
        cout << "Unable to connect to server, error: " << WSAGetLastError() << endl; // Вывод кода ошибки.
        closesocket(ConnectSocket); // Закрытие сокета.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Освобождение ресурсов Winsock.
        return 1;
    }

    // Отправка первого сообщения
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0); // Отправка первого сообщения.
    if (result == SOCKET_ERROR) { // Проверка на успешную отправку.
        cout << "Send failed, error: " << WSAGetLastError() << endl; // Вывод кода ошибки.
        closesocket(ConnectSocket); // Закрытие сокета.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Освобождение ресурсов Winsock.
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl; // Вывод количества отправленных байт.

    // Отправка второго сообщения
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0); // Отправка второго сообщения.
    if (result == SOCKET_ERROR) { // Проверка на успешную отправку.
        cout << "Send failed, error: " << WSAGetLastError() << endl; // Вывод кода ошибки.
        closesocket(ConnectSocket); // Закрытие сокета.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Освобождение ресурсов Winsock.
        return 1; 

    cout << "Sent: " << result << " bytes" << endl; // Вывод количества отправленных байт.

    // Завершение отправки данных
    result = shutdown(ConnectSocket, SD_SEND); // Завершение отправки данных на сокет.
    if (result == SOCKET_ERROR) { // Проверка на успешное завершение.
        cout << "Shutdown failed, error: " << WSAGetLastError() << endl; // Вывод кода ошибки.
        closesocket(ConnectSocket); // Закрытие сокета.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Освобождение ресурсов Winsock.
        return 1; 
    }

    // Получение данных от сервера
    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера перед получением данных.
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных из сокета.
        if (result > 0) { // Проверка на успешное получение данных.
            cout << "Received " << result << " bytes: " << recvBuffer << endl; // Вывод количества полученных байт и данных.
        }
        else if (result == 0) { // Проверка на закрытие соединения.
            cout << "Connection closed" << endl; // Сообщение о закрытии соединения.
        }
        else { // Проверка на ошибку при получении данных.
            cout << "Receive failed, error: " << WSAGetLastError() << endl; // Вывод кода ошибки.
        }
    } while (result > 0); // Повторять, пока есть данные для получения.

    // Освобождение ресурсов
    freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
    closesocket(ConnectSocket); // Закрытие сокета.
    WSACleanup(); // Освобождение ресурсов Winsock.
    return 0; 
}

#include "ClipBridgeNative.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

namespace
{
    std::string g_lastMessage;
    std::mutex g_messageMutex;

    bool InitializeWinsock()
    {
        WSADATA data{};
        return WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }

    void StoreLastMessage(const char* text, int length)
    {
        std::lock_guard<std::mutex> lock(g_messageMutex);
        g_lastMessage.assign(text, length);
    }
}

extern "C" CLIPBRIDGE_API bool StartServer(int port)
{
    if (port <= 0 || port > 65535 || !InitializeWinsock())
    {
        return false;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(static_cast<u_short>(port));

    bool success = false;

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    if (listen(listenSocket, 1) == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket != INVALID_SOCKET)
    {
        char buffer[4096]{};
        // This tiny demo accepts one TCP connection and reads one text payload.
        int bytesReceived = recv(clientSocket, buffer, static_cast<int>(sizeof(buffer) - 1), 0);
        if (bytesReceived > 0)
        {
            StoreLastMessage(buffer, bytesReceived);
            std::cout << "Received: " << g_lastMessage << std::endl;
            success = true;
        }

        closesocket(clientSocket);
    }

    closesocket(listenSocket);
    WSACleanup();
    return success;
}

extern "C" CLIPBRIDGE_API bool SendText(const char* ipAddress, int port, const char* text)
{
    if (ipAddress == nullptr || text == nullptr || port <= 0 || port > 65535 || !InitializeWinsock())
    {
        return false;
    }

    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(static_cast<u_short>(port));

    if (inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr) != 1)
    {
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }

    bool success = false;
    if (connect(connectSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != SOCKET_ERROR)
    {
        const int length = static_cast<int>(std::strlen(text));
        success = send(connectSocket, text, length, 0) == length;
    }

    closesocket(connectSocket);
    WSACleanup();
    return success;
}

extern "C" CLIPBRIDGE_API void GetLastMessage(char* buffer, int bufferSize)
{
    if (buffer == nullptr || bufferSize <= 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(g_messageMutex);
    const int copyLength = std::min(static_cast<int>(g_lastMessage.size()), bufferSize - 1);
    if (copyLength > 0)
    {
        std::memcpy(buffer, g_lastMessage.data(), copyLength);
    }

    buffer[copyLength] = '\0';
}

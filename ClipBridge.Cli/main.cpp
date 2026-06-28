#include "../ClipBridge.Native/ClipBridgeNative.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
    void PrintUsage()
    {
        std::cout << "Usage:\n"
                  << "  ClipBridge.Cli.exe server <port>\n"
                  << "  ClipBridge.Cli.exe send <ip-address> <port> <text>\n";
    }

    bool TryParsePort(const char* value, int& port)
    {
        if (value == nullptr)
        {
            return false;
        }

        port = std::atoi(value);
        return port > 0 && port <= 65535;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        PrintUsage();
        return 1;
    }

    const std::string command = argv[1];

    if (command == "server")
    {
        if (argc != 3)
        {
            PrintUsage();
            return 1;
        }

        int port = 0;
        if (!TryParsePort(argv[2], port))
        {
            std::cerr << "Invalid port." << std::endl;
            return 1;
        }

        std::cout << "ClipBridge server waiting on port " << port << "..." << std::endl;
        if (!StartServer(port))
        {
            std::cerr << "Server failed." << std::endl;
            return 1;
        }

        char message[4096]{};
        GetLastMessage(message, sizeof(message));
        std::cout << "Last message: " << message << std::endl;
        return 0;
    }

    if (command == "send")
    {
        if (argc != 5)
        {
            PrintUsage();
            return 1;
        }

        int port = 0;
        if (!TryParsePort(argv[3], port))
        {
            std::cerr << "Invalid port." << std::endl;
            return 1;
        }

        if (!SendText(argv[2], port, argv[4]))
        {
            std::cerr << "Send failed." << std::endl;
            return 1;
        }

        std::cout << "Text sent." << std::endl;
        return 0;
    }

    PrintUsage();
    return 1;
}

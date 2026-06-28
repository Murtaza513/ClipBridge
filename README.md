# ClipBridge

ClipBridge is a minimal Windows x64 portfolio project that sends text messages over TCP between two console instances. It demonstrates a native C++ Winsock DLL, a C++ command-line app, and a tiny C# demo that calls the native DLL through P/Invoke.

## Quick Demo for Recruiters

If you only have a few minutes, run the C++ demo first. It shows the core TCP text transfer. Then run the C# demo to verify that managed C# code can call the native C++ DLL through P/Invoke.

ClipBridge is a two-terminal demo:

1. Terminal 1 starts the server.
2. Terminal 2 sends text.
3. Terminal 1 prints each received message.

Build or download the Windows x64 files first. If using a release zip, open PowerShell in the extracted folder. The C++ quick demo expects these files to be in the same folder:

```text
ClipBridge.Cli.exe
ClipBridge.Native.dll
```

Recruiters can get these files in one of two ways:

- Download the GitHub Release zip, extract it, and run the commands from the extracted folder.
- Clone the repository, open `ClipBridge.sln` in Visual Studio, build `Release | x64`, and then use the generated `.\x64\Release\` folder.

Terminal 1:

```powershell
.\ClipBridge.Cli.exe server 5050
```

Terminal 2:

```powershell
.\ClipBridge.Cli.exe send 127.0.0.1 5050 "Hello from ClipBridge"
```

Expected output:

```text
Terminal 2:
Text sent.

Terminal 1:
Received: Hello from ClipBridge
```

Run the Terminal 2 send command again with different text to send more messages. Stop the server with `Ctrl+C`.

### C# Interop Demo

Start the C++ server first:

```powershell
.\ClipBridge.Cli.exe server 5050
```

Then run the C# sender from the repository root:

```powershell
dotnet run --project .\ClipBridge.CSharpDemo\ClipBridge.CSharpDemo.csproj --property:Platform=x64 -- 127.0.0.1 5050 "Hello from C#"
```

Expected output:

```text
Text sent from C#.
```

The server should print:

```text
Received: Hello from C#
```

The C# demo requires the native DLL to be built first because it calls `ClipBridge.Native.dll`.

## What This Demonstrates

`ClipBridge.Native` exports plain C-style functions from a C++ DLL:

```cpp
extern "C" __declspec(dllexport) bool StartServer(int port);
extern "C" __declspec(dllexport) bool SendText(const char* ipAddress, int port, const char* text);
extern "C" __declspec(dllexport) void GetLastMessage(char* buffer, int bufferSize);
```

The C++ CLI calls those functions directly. The C# console app calls `SendText` from the same native DLL using P/Invoke.

## Technologies

- C++ native DLL
- Winsock TCP sockets
- C++ console app
- C# .NET console app
- P/Invoke for C# to native C++ interoperability
- Windows x64

## Project Layout

```text
ClipBridge/
+-- ClipBridge.Native/          C++ DLL
+-- ClipBridge.Cli/             C++ console app
+-- ClipBridge.CSharpDemo/      C# console app using P/Invoke
+-- README.md
+-- ClipBridge.sln
```

## Build from Source

Use this path if you cloned the repository instead of downloading a prebuilt release zip.

1. Open `ClipBridge.sln` in Visual Studio.
2. Select `Release` as the configuration.
3. Select `x64` as the platform.
4. Build the solution.

Recommended output folder, relative to the repository root:

```text
.\x64\Release\
```

During local development, `Debug | x64` is also fine:

```text
.\x64\Debug\
```

All projects should target x64 so the C# process can load the x64 native DLL.

## Run the C++ Demo

Start the server in one terminal:

```powershell
cd .\x64\Release
.\ClipBridge.Cli.exe server 5050
```

Send text from a second terminal:

```powershell
cd .\x64\Release
.\ClipBridge.Cli.exe send 127.0.0.1 5050 "Hello from ClipBridge"
```

The server keeps running, prints each received message, and waits for the next connection. Stop it with `Ctrl+C`.

## Run the C# P/Invoke Demo

Start the C++ server first:

```powershell
cd .\x64\Release
.\ClipBridge.Cli.exe server 5050
```

Then run the C# sender:

```powershell
cd ..\..
dotnet run --project .\ClipBridge.CSharpDemo\ClipBridge.CSharpDemo.csproj --property:Platform=x64 -- 127.0.0.1 5050 "Hello from C#"
```

This confirms that C# can call the native C++ `SendText` function through P/Invoke.

## Scope

ClipBridge has no UI, no database, no web API, no authentication, no encryption, no installer, and no background service. It is a deliberately minimal example of C++, Winsock, TCP, and C# interoperability inspired by remote support workflows.

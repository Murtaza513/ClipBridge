using System.Runtime.InteropServices;

if (args.Length != 3)
{
    Console.WriteLine("Usage:");
    Console.WriteLine("  dotnet run -- 127.0.0.1 5050 \"Hello from C#\"");
    return 1;
}

if (!int.TryParse(args[1], out int port) || port <= 0 || port > 65535)
{
    Console.WriteLine("Invalid port.");
    return 1;
}

bool success = NativeMethods.SendText(args[0], port, args[2]);
Console.WriteLine(success ? "Text sent from C#." : "Send failed.");
return success ? 0 : 1;

internal static class NativeMethods
{
    // P/Invoke imports the exported native C++ function from ClipBridge.Native.dll.
    [DllImport("ClipBridge.Native.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    internal static extern bool SendText(string ipAddress, int port, string text);
}

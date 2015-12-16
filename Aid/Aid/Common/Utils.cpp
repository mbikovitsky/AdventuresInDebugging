#include <boost\lexical_cast.hpp>

#include "Utils.hpp"


namespace Aid { namespace Utils {


void ClearScreen()
{
    auto console = WinApiHelpers::CreateFile(L"CONOUT$",
                                             GENERIC_READ | GENERIC_WRITE,
                                             OPEN_EXISTING,
                                             FILE_SHARE_WRITE);

    // Obtain some info
    CONSOLE_SCREEN_BUFFER_INFOEX info = WinApiHelpers::GetConsoleScreenBufferInfoEx(console);

    // Do the actual clearing
    DWORD cells = info.dwSize.X * info.dwSize.Y;
    COORD origin = { 0, 0 };
    DWORD written = 0;
    if (!FillConsoleOutputCharacterW(console, L' ', cells, origin, &written))
    {
        WinApiHelpers::ThrowLastError("FillConsoleOutputCharacterW");
    }
    if (!FillConsoleOutputAttribute(console, info.wAttributes, cells, origin, &written))
    {
        WinApiHelpers::ThrowLastError("FillConsoleOutputAttribute");
    }

    // Finally, move the cursor the the origin
    if (!SetConsoleCursorPosition(console, origin))
    {
        WinApiHelpers::ThrowLastError("SetConsoleCursorPosition");
    }
}

WinApiHelpers::PlainHandle ForkToAddress(ChildFunction childFunction)
{
    // Calculate the function's RVA
    ptrdiff_t functionRva = PointerToRva(childFunction);

    // Build the parameters for process creation
    auto moduleFileName = WinApiHelpers::GetModuleFileName();
    auto arguments = boost::lexical_cast<std::wstring>(functionRva);

    // Spawn the process
    return WinApiHelpers::CreateProcess(moduleFileName, arguments);
}


ptrdiff_t PointerToRva(const void * pointer)
{
    const char * baseAddress = reinterpret_cast<const char *>(WinApiHelpers::GetModuleHandle());
    const char * pointerCast = reinterpret_cast<const char *>(pointer);

    return pointerCast - baseAddress;
}

const void * RvaToPointer(ptrdiff_t rva)
{
    ptrdiff_t baseAddress = reinterpret_cast<ptrdiff_t>(WinApiHelpers::GetModuleHandle());

    return reinterpret_cast<const void *>(baseAddress + rva);
}


} }

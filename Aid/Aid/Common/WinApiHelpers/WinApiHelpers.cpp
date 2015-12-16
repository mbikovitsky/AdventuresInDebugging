#include <vector>
#include <array>

#include "WinApiHelpers.hpp"


namespace Aid { namespace Utils { namespace WinApiHelpers {


void __declspec(noreturn) ThrowLastError(const char *apiFunction)
{
    BOOST_THROW_EXCEPTION(AidException() << Win32ErrorInfo(apiFunction, GetLastError()));
}

FileHandle CreateFile(const std::wstring & fileName,
                      DWORD desiredAccess,
                      DWORD creationDisposition,
                      DWORD shareMode /* = 0 */,
                      boost::optional<SECURITY_ATTRIBUTES &> securityAttributes /* = boost::none */,
                      DWORD flagsAndAttributes /* = FILE_ATTRIBUTE_NORMAL */,
                      HANDLE templateFile /* = NULL */)
{
    FileHandle file = std::make_scoped_resource(::CreateFileW(fileName.c_str(),
                                                              desiredAccess,
                                                              shareMode,
                                                              (securityAttributes) ? (&*securityAttributes) : (NULL),
                                                              creationDisposition,
                                                              flagsAndAttributes,
                                                              templateFile),
                                                &::CloseHandle,
                                                INVALID_HANDLE_VALUE);
    if (!file.valid())
    {
        ThrowLastError("CreateFileW");
    }

    return file;
}

PlainHandle CreateProcess(const std::wstring & applicationName,
                          boost::optional<const std::wstring &> arguments /* = boost::none */,
                          boost::optional<SECURITY_ATTRIBUTES &> processAttributes /* = boost::none */,
                          boost::optional<SECURITY_ATTRIBUTES &> threadAttributes /* = boost::none */,
                          bool inheritHandles /* = false */,
                          DWORD creationFlags /* = 0 */,
                          PVOID environment /* = NULL */,
                          boost::optional<const std::wstring &> currentDirectory /* = boost::none */,
                          boost::optional<const STARTUPINFOW &> startupInfo /* = boost::none */)
{
    // Construct the command line
    std::wstringstream commandLineStream;
    commandLineStream << L'"' << applicationName << L'"';
    if (arguments)
    {
        commandLineStream << L' ';
        commandLineStream << *arguments;
    }
    std::wstring commandLine(commandLineStream.str());

    // Build the STARTUPINFO structure
    STARTUPINFOW usedStartupInfo = { sizeof(startupInfo) };
    if (startupInfo)
    {
        usedStartupInfo = *startupInfo;
    }

    PROCESS_INFORMATION processInfo = { 0 };
    if (!::CreateProcessW(applicationName.c_str(),
                          &commandLine.at(0),
                          ((processAttributes) ? (&*processAttributes) : (NULL)),
                          ((threadAttributes) ? (&*threadAttributes) : (NULL)),
                          inheritHandles,
                          creationFlags,
                          environment,
                          ((currentDirectory) ? (currentDirectory->c_str()) : (NULL)),
                          &usedStartupInfo,
                          &processInfo))
    {
        ThrowLastError("CreateProcessW");
    }
    ::CloseHandle(processInfo.hThread);

    return std::make_scoped_resource(processInfo.hProcess,
                                     &::CloseHandle,
                                     (HANDLE)NULL);
}

DWORD GetExitCodeProcess(HANDLE process)
{
    DWORD exitCode = 0;
    if (!::GetExitCodeProcess(process, &exitCode))
    {
        ThrowLastError("GetExitCodeProcess");
    }

    return exitCode;
}

std::wstring GetModuleFileName(HMODULE module /* = NULL */)
{
    std::vector<wchar_t> fileName(UNICODE_STRING_MAX_CHARS + 1);
    DWORD filledChars = ::GetModuleFileNameW(module,
                                             fileName.data(),
                                             static_cast<DWORD>(fileName.size()));
    if (0 == filledChars)
    {
        ThrowLastError("GetModuleFileNameW");
    }

    // Check that we got the whole name
    if (fileName.size() == filledChars)
    {
        // Windows XP does not set LE to ERROR_INSUFFICIENT_BUFFER, so we do it
        // ourselves.
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        ThrowLastError("GetModuleFileNameW");
    }

    return std::wstring(fileName.data());
}

CONSOLE_SCREEN_BUFFER_INFOEX GetConsoleScreenBufferInfoEx(HANDLE consoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info = { sizeof(info) };
    if (!::GetConsoleScreenBufferInfoEx(consoleOutput, &info))
    {
        ThrowLastError("GetConsoleScreenBufferInfoEx");
    }

    return info;
}

void SetConsoleTextAttribute(HANDLE consoleOutput, WORD attributes)
{
    if (!::SetConsoleTextAttribute(consoleOutput, attributes))
    {
        ThrowLastError("SetConsoleTextAttribute");
    }
}

VirtualAllocPtr VirtualAlloc(PVOID address,
                             SIZE_T size,
                             DWORD allocationType,
                             DWORD protect)
{
    PVOID buffer = ::VirtualAlloc(address,
                                  size,
                                  allocationType,
                                  protect);
    if (NULL == buffer)
    {
        ThrowLastError("VirtualAlloc");
    }

    return VirtualAllocPtr(buffer, [](PVOID memory){ ::VirtualFree(memory, 0, MEM_RELEASE); });
}

HMODULE GetModuleHandle()
{
    return GetModuleHandle(GetModuleFileName());
}

HMODULE GetModuleHandle(const std::wstring &moduleName)
{
    HMODULE module = ::GetModuleHandleW(moduleName.c_str());
    if (NULL == module)
    {
        ThrowLastError("GetModuleHandleW");
    }

    return module;
}

AtomHandle AddAtom(const std::wstring &string)
{
    ATOM atom = ::AddAtomW(string.c_str());
    if (0 == atom)
    {
        ThrowLastError("AddAtomW");
    }

    return std::make_scoped_resource(atom, &::DeleteAtom, (ATOM)0);
}

std::wstring GetAtomName(ATOM atom)
{
    std::array<wchar_t, MAX_PATH> atomName;

    if (0 == ::GetAtomNameW(atom,
                            atomName.data(),
                            static_cast<int>(atomName.size())))
    {
        ThrowLastError("GetAtomNameW");
    }

    return std::wstring(atomName.data());
}

static BOOL CALLBACK CallbackWrapper(HWND window, LPARAM param)
{
    auto userCallback = reinterpret_cast<WindowEnumCallback *>(param);

    try
    {
        return (*userCallback)(window);
    }
    catch (...)
    {
        std::cerr << boost::current_exception_diagnostic_information(true) << std::endl;

        SetLastError(ERROR_UNHANDLED_EXCEPTION);
        return false;
    }
}

void EnumWindows(WindowEnumCallback callback)
{
    LPARAM param = reinterpret_cast<LPARAM>(&callback);

    if (!::EnumWindows(&CallbackWrapper, param))
    {
        ThrowLastError("EnumWindows");
    }
}

std::wstring GetWindowText(HWND window)
{
    std::vector<wchar_t> windowText(UNICODE_STRING_MAX_CHARS + 1);

    (void)::GetWindowTextW(window,
                           windowText.data(),
                           static_cast<int>(windowText.size()));

    return std::wstring(windowText.data());
}

std::tuple<DWORD, DWORD> GetWindowThreadProcessId(HWND window)
{
    DWORD processId = 0;
    DWORD threadId = ::GetWindowThreadProcessId(window, &processId);

    return std::make_tuple(threadId, processId);
}

PlainHandle OpenProcess(DWORD desiredAccess, bool inheritHandle, DWORD processId)
{
    auto process = std::make_scoped_resource(::OpenProcess(desiredAccess,
                                                           inheritHandle,
                                                           processId),
                                             &::CloseHandle,
                                             (HANDLE)NULL);
    if (!process.valid())
    {
        ThrowLastError("OpenProcess");
    }

    return process;
}

void TerminateProcess(HANDLE process, UINT exitCode)
{
    if (!::TerminateProcess(process, exitCode))
    {
        ThrowLastError("TerminateProcess");
    }
}

PlainHandle CreateRemoteThread(HANDLE process,
                               PTHREAD_START_ROUTINE startAddress,
                               PVOID parameter /* = NULL */,
                               boost::optional<SECURITY_ATTRIBUTES &> threadAttributes /* = boost::none */,
                               SIZE_T stackSize /* = 0 */,
                               DWORD creationFlags /* = 0 */)
{
    auto thread = std::make_scoped_resource(::CreateRemoteThread(process,
                                                                 (threadAttributes) ? (&*threadAttributes) : (NULL),
                                                                 stackSize,
                                                                 startAddress,
                                                                 parameter,
                                                                 creationFlags,
                                                                 NULL),
                                            &::CloseHandle,
                                            (HANDLE)NULL);
    if (!thread.valid())
    {
        ThrowLastError("CreateRemoteThread");
    }

    return thread;
}


} } }

#pragma once

#include <tuple>

#include <boost\optional.hpp>
#include <boost\exception\errinfo_api_function.hpp>
#include <boost\exception\info_tuple.hpp>

#pragma warning(push)
#pragma warning(disable: 4127)
    #include "..\scoped_resource.hpp"
#pragma warning(pop)


namespace Aid { namespace Utils { namespace WinApiHelpers {


typedef boost::error_info<struct tag_win32_error_info, DWORD> Win32LastError;
typedef boost::tuple<boost::errinfo_api_function, Win32LastError> Win32ErrorInfo;

void __declspec(noreturn) ThrowLastError(const char *apiFunction);

typedef std::scoped_resource<decltype(&::CloseHandle), HANDLE> PlainHandle;
typedef std::scoped_resource<decltype(&::CloseHandle), HANDLE> FileHandle;
typedef std::scoped_resource<decltype(&::DeleteAtom), ATOM> AtomHandle;

FileHandle CreateFile(const std::wstring & fileName,
                      DWORD desiredAccess,
                      DWORD creationDisposition,
                      DWORD shareMode = 0,
                      boost::optional<SECURITY_ATTRIBUTES &> securityAttributes = boost::none,
                      DWORD flagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
                      HANDLE templateFile = NULL);

PlainHandle CreateProcess(const std::wstring & applicationName,
                          boost::optional<const std::wstring &> arguments = boost::none,
                          boost::optional<SECURITY_ATTRIBUTES &> processAttributes = boost::none,
                          boost::optional<SECURITY_ATTRIBUTES &> threadAttributes = boost::none,
                          bool inheritHandles = false,
                          DWORD creationFlags = 0,
                          PVOID environment = NULL,
                          boost::optional<const std::wstring &> currentDirectory = boost::none,
                          boost::optional<const STARTUPINFOW &> startupInfo = boost::none);

DWORD GetExitCodeProcess(HANDLE process);

std::wstring GetModuleFileName(HMODULE module = NULL);

CONSOLE_SCREEN_BUFFER_INFOEX GetConsoleScreenBufferInfoEx(HANDLE consoleOutput);

void SetConsoleTextAttribute(HANDLE consoleOutput, WORD attributes);

typedef std::unique_ptr<VOID, std::function<void(PVOID)>> VirtualAllocPtr;
VirtualAllocPtr VirtualAlloc(PVOID address,
                             SIZE_T size,
                             DWORD allocationType,
                             DWORD protect);

HMODULE GetModuleHandle();
HMODULE GetModuleHandle(const std::wstring &moduleName);

AtomHandle AddAtom(const std::wstring &string);
std::wstring GetAtomName(ATOM atom);

typedef std::function<bool(HWND)> WindowEnumCallback;
void EnumWindows(WindowEnumCallback callback);

std::wstring GetWindowText(HWND window);

std::tuple<DWORD, DWORD> GetWindowThreadProcessId(HWND window);

PlainHandle OpenProcess(DWORD desiredAccess, bool inheritHandle, DWORD processId);

void TerminateProcess(HANDLE process, UINT exitCode);

PlainHandle CreateRemoteThread(HANDLE process,
                               PTHREAD_START_ROUTINE startAddress,
                               PVOID parameter = NULL,
                               boost::optional<SECURITY_ATTRIBUTES &> threadAttributes = boost::none,
                               SIZE_T stackSize = 0,
                               DWORD creationFlags = 0);


} } }

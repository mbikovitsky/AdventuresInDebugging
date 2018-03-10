#include <vector>
#include <thread>

#include <boost\foreach.hpp>

#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"
#include "..\..\..\Common\WinApiHelpers\WinApiHelpers.hpp"

#include "DeadInTheWater.hpp"


static const std::vector<std::wstring> g_badTitles =
{
    L"WinDbg",
    /*L"IDA",
    L"idaq",
    L"Olly",
    L"Sysinternals",
    L"cmd",
    L"python"*/
};

struct TerminatorEnumerator
{
private:
    std::function<void(HANDLE)> m_terminatorFunc;

public:
    TerminatorEnumerator(decltype(m_terminatorFunc) terminatorFunc) :
        m_terminatorFunc(terminatorFunc)
    {
    }

    bool operator()(HWND window)
    {
        auto windowTitle = Aid::Utils::WinApiHelpers::GetWindowText(window);

        bool found = false;
        BOOST_FOREACH(auto &badTitle, g_badTitles)
        {
            if (std::wstring::npos != windowTitle.find(badTitle))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            // This window is okay. Continue.
            return true;
        }

        // Extract the process ID
        auto result = Aid::Utils::WinApiHelpers::GetWindowThreadProcessId(window);
        auto processId = std::get<1>(result);

        try
        {
            // Open a handle to the process
            auto process = Aid::Utils::WinApiHelpers::OpenProcess(PROCESS_ALL_ACCESS,
                                                                  false,
                                                                  processId);

            // :)
            m_terminatorFunc(process);
        }
        catch (...)
        {
            return true;
        }

        return true;
    }
};

void Aid::DeadInTheWater()
{
    std::wcout << LR"EOS(You're almost there, buddy. One last push.
Hit 'g' when ready.
)EOS";

    Utils::ExerciseHelpers::DebugBreak();

    std::thread terminatorThread([] () {
        Utils::WinApiHelpers::EnumWindows(TerminatorEnumerator([] (HANDLE process) {
            Utils::WinApiHelpers::TerminateProcess(process, ERROR_BAD_COMMAND);
        }));
    });

    terminatorThread.join();

    // If we're here it means WinDbg has not been terminated.
    // Remember that debugger termination causes the debuggee to die, as well.

    std::wcout << LR"EOS(
One more time with feeling!
)EOS";

    Utils::ExerciseHelpers::DebugBreak();

    std::thread crasherThread([] () {
        Utils::WinApiHelpers::EnumWindows(TerminatorEnumerator([] (HANDLE process) {
            (void)Utils::WinApiHelpers::CreateRemoteThread(process,
                                                           reinterpret_cast<PTHREAD_START_ROUTINE>(0xDEADBEEF));
        }));
    });

    crasherThread.join();

    std::wcout << LR"EOS(
That's it. This journey is over, and another one begins.
Truly, you know no fear, for you have mastered this course.
Onwards, then, to exterminate the bugs that plague our code!
Onwards, to kernel space.

Hit 'g' to exit.
)EOS";

    Utils::ExerciseHelpers::DebugBreak();
}

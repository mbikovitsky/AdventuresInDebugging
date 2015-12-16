#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"
#include "..\..\..\Common\WinApiHelpers\WinApiHelpers.hpp"
#include "..\..\..\Common\Utils.hpp"

#include "Children.hpp"


namespace Aid {


static const wchar_t g_childString[] = L"\n>>> Luke, I'm your father!\n";

static void ChildFunction()
{
    SYSTEM_INFO info = { 0 };
    ::GetSystemInfo(&info);

    auto deleter = [](PVOID memory) {
        (void)::VirtualFree(memory, 0, MEM_RELEASE);
    };

    // Allocate some memory
    auto safeBuffer = Utils::WinApiHelpers::VirtualAlloc(NULL,
                                                         info.dwPageSize * 2,
                                                         MEM_RESERVE,
                                                         PAGE_READWRITE);

    // Get a pointer to the second page
    auto badString = reinterpret_cast<wchar_t *>(reinterpret_cast<char *>(safeBuffer.get()) + info.dwPageSize);

    // Make sure we can write there
    auto result = Utils::WinApiHelpers::VirtualAlloc(badString,
                                                     sizeof(g_childString),
                                                     MEM_COMMIT,
                                                     PAGE_READWRITE);
    result.release();   // Make sure we don't free prematurely

    // Copy the string
    std::memmove(badString, g_childString, sizeof(g_childString));

    // :)
    --badString;

    for (const wchar_t *currentChar = badString;
         *currentChar != L'\0';
         ++currentChar)
    {
        std::wcout << *currentChar;
    }
}

void Children::operator()()
{
    std::wcout << LR"EOS(This next exercise is a bit complicated, and requires some setup.
Lemme just launch a process here...
Hit 'g' when ready.
)EOS";

    Utils::ExerciseHelpers::DebugBreak();

    // Create a process
    auto child = Utils::ForkToAddress(&ChildFunction);

    // Wait for it to crash :)
    if (WAIT_OBJECT_0 != ::WaitForSingleObject(child, INFINITE))
    {
        Utils::WinApiHelpers::ThrowLastError("WaitForSingleObject");
    }

    if (STATUS_ACCESS_VIOLATION != Utils::WinApiHelpers::GetExitCodeProcess(child))
    {
        std::wcout << LR"EOS(
Totally not cool, man! Not cool at all!
Well if you're so smart and all, just go ahead.
I don't care.
)EOS";

        Utils::ExerciseHelpers::DebugBreak();

        return;
    }

    std::wcout << LR"EOS(
It crashed?! Oh, boy.
Better figure out what happened.
After you hit 'g' I'll launch the process again for you.
Just make sure you catch it this time (.childdbg).
)EOS";

    Utils::ExerciseHelpers::VerifyLoop([]() {
        // Spawn the process again
        auto newChild = Utils::ForkToAddress(&ChildFunction);

        // Wait for it to return
        if (WAIT_OBJECT_0 != ::WaitForSingleObject(newChild, INFINITE))
        {
            Utils::WinApiHelpers::ThrowLastError("WaitForSingleObject");
        }

        return ERROR_SUCCESS == Utils::WinApiHelpers::GetExitCodeProcess(newChild);
    });

    std::wcout << LR"EOS(
Amaziballs! You made it!
)EOS";

    Utils::ExerciseHelpers::DebugBreak();
}


}

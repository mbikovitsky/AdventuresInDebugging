#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"
#include "..\..\..\Common\WinApiHelpers\WinApiHelpers.hpp"

#include "Structures.hpp"


namespace Aid {


void Structures::operator()()
{
    std::wcout << LR"EOS(Reading and writing bytes in memory is well and good,
but when you need to patch a specific member in a 4K structure
you quickly begin to doubt WinDbg as an efficient debugging tool.

Until today!

WinDbg has the ability to display structures in memory,
provided you have the correct symbols loaded (it's not psychic,
after all). The 'dt' command is what you want to be looking at.

Also, while we're at it, take a minute to examine a random
structure #defined in the Windows headers. Say, SECURITY_ATTRIBUTES.
Examine the definition carefully, for it might come in handy.

)EOS";

    // Initialize a critical section
    CRITICAL_SECTION criticalSection = { 0 };
    ::InitializeCriticalSection(&criticalSection);
    auto criticalSectionResource = std::make_scoped_resource_unchecked(&criticalSection,
                                                                       &::DeleteCriticalSection);

    std::wcout << L"Please standby for exercise initialization ...\n";

    LONG recursionCount = static_cast<LONG>(Utils::ExerciseHelpers::Rand(0, MAXLONG));
    for (LONG iteration = 0;
         iteration < recursionCount;
         ++iteration)
    {
        ::EnterCriticalSection(&criticalSection);
    }

    std::wcout << L"\nHere's an address: " << &criticalSection << std::endl;
    std::wcout << LR"EOS(At that address is an RTL_CRITICAL_SECTION object.
Tell me how many times it has been recursively entered.
)EOS";

    Utils::ExerciseHelpers::VerifyLoop([&criticalSection](){
        return criticalSection.RecursionCount == Utils::ExerciseHelpers::ReadNumberHex();
    });

    std::wcout << LR"EOS(
Please standby for exercise termination ...
)EOS";

    for (LONG iteration = 0;
         iteration < recursionCount;
         ++iteration)
    {
        ::LeaveCriticalSection(&criticalSection);
    }
}


}

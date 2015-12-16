#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "WhatIfs.hpp"


void Aid::WhatIfs()
{
    std::wcout << LR"EOS(Fine, I lied. That wasn't the last exercise in basic.
But this one definitely is. Promise!

All right, here's the deal.
)EOS";

    volatile uint64_t number = 0;

    std::wcout << L"Here's an address: " << (void *)&number << std::endl;

    std::wcout << LR"EOS(After you hit 'g', I'll write several QWORDs to that address.
That is, I'm going to write a value, then I'm going to overwrite it with another value, and so on.
Your job is to tell me how many _even_ QWORDs I've written there.
To assist you, I'll break into the debugger before writing the values, and after writing them.

Also, read about pseudo-registers, the '.if' command, the 'gc' command and revisit
the documentation on breakpoints
Oh yeah, and the 'ba' command should come in handy.
And the 'qwo' command, too.

Best of luck!
)EOS";

    uint64_t evenNumbers = 0;

    Utils::ExerciseHelpers::VerifyLoop([&evenNumbers] () {
        return evenNumbers == Utils::ExerciseHelpers::ReadNumberHex();
    }, [&evenNumbers, &number] () {
        number = 0;
        evenNumbers = 0;

        Utils::ExerciseHelpers::DebugBreak();

        for (uint8_t iteration = 0;
             iteration < UINT8_MAX;
             ++iteration)
        {
            number = Utils::ExerciseHelpers::Rand();
            if (0 == number % 2)
            {
                ++evenNumbers;
            }
        }

        Utils::ExerciseHelpers::DebugBreak();
    });

    std::wcout << LR"EOS(
Very nice work indeed! Now delete your breakpoint so it won't screw anything
up in future exercises. Hit 'g' when ready.
)EOS";
}

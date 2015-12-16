#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "FailingFunction.hpp"


static bool ImpossibleLoop()
{
    for (volatile uint16_t index = 0;
         index < UINT16_MAX;
         ++index)
    {
        if (32954 == index)
        {
            return false;
        }
    }

    return true;
}

void Aid::FailingFunction()
{
    std::wcout << LR"EOS(Welcome to the challenges!
If you're here, you're either making some very good progress, or are cheating masterfully.
Either way, congratulations!

But enough with the celebrations. Onwards!

Here's your first task. Find out why this function fails and fix it: )EOS";

    std::wcout << &ImpossibleLoop << std::endl;

    Utils::ExerciseHelpers::VerifyLoop(&ImpossibleLoop);
}

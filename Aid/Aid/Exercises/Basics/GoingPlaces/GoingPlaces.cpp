#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "GoingPlaces.hpp"


namespace Aid {


void GoingPlaces::operator()()
{
    std::wcout <<LR"EOS(Howdy fellas!
This here is a short course on debugging with WinDbg.

Before we begin, I want to get some things out of the way:
1. All numbers in this tutorial are in HEX. This includes
   numbers printed by the program, and numbers input by you.
2. However, WinDbg sometimes displays numbers in other bases.
   Here's the short version (for the long one, '.hh' is where to go):
     - 0x means HEX.
     - 0n means DECIMAL.
     - 0t means OCTAL.
     - 0y means BINARY.
3. WinDbg's default base (AKA radix) is HEX.
4. This program saves progress via the .state file located in the program
   directory. Progress is automatically saved at the beginning of each
   exercise. Deleting the file will reset your progress.

Whew... Enough with the introductions!

As you can see, the program is currently stuck at a breakpoint.
But, if we're going to get any work done, we should probably get out of here.
Read about the 'g' command, and you'll know what to do :)

Good luck!
)EOS";

	Utils::ExerciseHelpers::DebugBreak();
}


}

#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "ReadingMemory.hpp"


namespace Aid
{
	void ReadingMemory::operator()()
	{
    std::wcout << LR"EOS(That's some fine progress there, dude/dudette!

Next up: memory manipulation.
First, reading memory.
)EOS";

		uint64_t random_number = 0;

        std::wcout << L"\nHere's an address: " << &random_number << std::endl;
        std::wcout << LR"EOS(Tell me the QWORD that's stored there.
So you should get the QWORD, resume execution, and type it at the command line.
That's the 'dq' command for you :)
Of course, this is just one of many commands for displaying memory contents.
However, I trust in your '.hh' skills enough to not go over them here.

Notice that WinDbg will insert the <`> character to separate the number in two.
Omit the character when inputting the result.
)EOS";

		random_number = Utils::ExerciseHelpers::Rand();
		Utils::ExerciseHelpers::VerifyLoop([&random_number]()
		{
			// Read input from the user
			uint64_t user_input = Utils::ExerciseHelpers::ReadNumberHex();

			// Determine whether the user got it right
			bool result = (user_input == random_number);

			// In case she hasn't, generate a new number
			// for the next iteration.
			if (!result)
			{
				random_number = Utils::ExerciseHelpers::Rand();
			}

			return result;
		});
	}
}

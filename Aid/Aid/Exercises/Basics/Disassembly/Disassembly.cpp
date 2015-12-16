#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "Disassembly.hpp"


namespace Aid
{
	void Disassembly::operator()()
	{
		std::wcout << LR"EOS(Jolly good! You're making real progress here, fellas!
Now then, let's continue.

A debugger wouldn't be of any actual use if it couldn't display
the code it is debugging.
So next up, disassembly!
)EOS";

		std::wcout << L"\nHere's an address: " << &DummyFunction << std::endl;

		std::wcout << LR"EOS(
Your job is to figure out what the function at that address returns,
and type it at the command line.

The 'u' command and its variants will be of use here.
Remember, you're unassembling a function here.

Good luck!

)EOS";

		uint8_t result = DummyFunction();

		Utils::ExerciseHelpers::VerifyLoop([result]()
		{
			return Utils::ExerciseHelpers::ReadNumberHex() == result;
		});

        std::wcout << LR"EOS(
When I think about it, I don't like that return value.
Assembly is really all about registers, and you need to learn how to manipulate them.
The 'r' command is your friend.
)EOS";

        uint8_t value = static_cast<uint8_t>(Utils::ExerciseHelpers::Rand(0, UINT8_MAX));

        std::wcout << L"Same function as above. Make it return " << std::hex << value << L".\n";

        Utils::ExerciseHelpers::VerifyLoop([value](){
            // Screw the optimizer!
            volatile decltype(&DummyFunction) funcPtr = &DummyFunction;

            return funcPtr() == value;
        });
	}

	uint8_t Disassembly::DummyFunction()
	{
		volatile uint8_t value;

		value = 1;
		value *= 2;
		value += 8;
		value /= 2;

		return value;
	}
}

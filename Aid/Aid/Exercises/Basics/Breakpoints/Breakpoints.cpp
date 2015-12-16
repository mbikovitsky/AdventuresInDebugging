#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "Breakpoints.hpp"


namespace Aid {


void Breakpoints::operator()()
{
	std::wcout << LR"EOS(Good job there!

I know you're eager to get some action, but first you need to master some basics.
The most important thing you can do in a debugger is set breakpoints.
Let's do that now.
)EOS";

	std::wcout << L"Here's an address: " << &DummyFunction << std::endl;
	std::wcout << L"Read about the 'bp' command, then set a breakpoint there and hit go." << std::endl;

	Utils::ExerciseHelpers::VerifyLoop([]()
	{
		return IsBreakpointSet(reinterpret_cast<std::uint8_t *>(&DummyFunction));
	});

	std::wcout << LR"EOS(
Alrighty then, let's do something else.
Same thing as above, but now with a symbolic name: kernel32!LoadLibraryW
)EOS";

	Utils::ExerciseHelpers::VerifyLoop([]()
	{
		return IsBreakpointSet(reinterpret_cast<std::uint8_t *>(&::LoadLibraryW));
	});

	HMODULE module_base = GetModuleHandle(NULL);
	ptrdiff_t offset = reinterpret_cast<std::uint8_t *>(&DummyFunction2) - reinterpret_cast<std::uint8_t *>(module_base);

	std::wcout << std::endl;
	std::wcout << L"And now, here's an address: " << module_base << L" ...\n";
    std::wcout << L"And an offset: " << std::hex << offset << L".\n";
	std::wcout << LR"EOS(Set a breakpoint there. You know the drill.
(BTW, if you haven't done so already, read about WinDbg's "Address and Address Range Syntax".)
)EOS";

	Utils::ExerciseHelpers::VerifyLoop([]()
	{
		return IsBreakpointSet(reinterpret_cast<std::uint8_t *>(&DummyFunction2));
	});

	std::wcout << LR"EOS(
Woah! That's a lot of breakpoints you have there!
Let's delete them so they won't get in the way.
Read about the 'bl' command and the 'bc' command, and carry on.
)EOS";

	Utils::ExerciseHelpers::VerifyLoop([]()
	{
		bool result =
			(!IsBreakpointSet(reinterpret_cast<std::uint8_t *>(&DummyFunction))) &&
			(!IsBreakpointSet(reinterpret_cast<std::uint8_t *>(&::LoadLibraryW))) &&
			(!IsBreakpointSet(reinterpret_cast<std::uint8_t *>(&DummyFunction2)));

		return result;
	});
}

bool Breakpoints::IsBreakpointSet(const std::uint8_t * address)
{
	return kBreakpointOpcode == address[0];
}

void Breakpoints::DummyFunction()
{
	BOOST_THROW_EXCEPTION(AidException());
}

void Breakpoints::DummyFunction2()
{
	BOOST_THROW_EXCEPTION(AidException());
}


}

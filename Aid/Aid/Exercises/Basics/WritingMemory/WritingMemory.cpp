#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"
#include "..\..\..\Common\WinApiHelpers\WinApiHelpers.hpp"

#include "WritingMemory.hpp"


namespace Aid
{
	void WritingMemory::operator()()
	{
        std::wcout << LR"EOS(So you think you can read, eh?
Let's see how well you can write!
)EOS";

		// Open the console
        auto console = Utils::WinApiHelpers::CreateFile(L"CONOUT$",
                                                        GENERIC_READ | GENERIC_WRITE,
                                                        OPEN_EXISTING,
                                                        FILE_SHARE_WRITE);

		// Obtain some info about it
		auto screen_buffer_info = Utils::WinApiHelpers::GetConsoleScreenBufferInfoEx(console);

		// Make a copy for posterity
		WORD original_foreground_attributes = GetForegroundAttributes(screen_buffer_info);
		WORD original_background_attributes = GetBackgroundAttributes(screen_buffer_info);

		WORD new_foreground_attrs = 0;
		std::wcout << L"\nAlrighty then. Here's an address: " << &new_foreground_attrs << std::endl;
		std::wcout << L"Here's a value: " << original_background_attributes << std::endl;
		std::wcout << L"Read about the 'ew' command, and stick the value at the address.\n";

		Utils::ExerciseHelpers::VerifyLoop([&new_foreground_attrs, original_background_attributes]()
		{
			return new_foreground_attrs == original_background_attributes;
		});

		WORD new_background_attrs = 0;
		std::wcout << std::endl << L"Here's another address: " << &new_background_attrs << std::endl;
		std::wcout << L"Here's another value: " << original_foreground_attributes << std::endl;
		std::wcout << L"Well, you know what they say... I like to move it, move it!" << std::endl;

		Utils::ExerciseHelpers::VerifyLoop([&new_background_attrs, original_foreground_attributes]()
		{
			return new_background_attrs == original_foreground_attributes;
		});

		// Set the console to the new colors
		SetColorAttributes(new_foreground_attrs, new_background_attrs, screen_buffer_info);
        Utils::WinApiHelpers::SetConsoleTextAttribute(console, screen_buffer_info.wAttributes);

		std::wcout << LR"EOS(
Oh man! What've you done?!
Better reverse the damage before it's too late!
Use the same addresses as above, but reverse the values you put there.
)EOS";

		Utils::ExerciseHelpers::VerifyLoop([&new_foreground_attrs, original_foreground_attributes, &new_background_attrs, original_background_attributes]()
		{
			return
				(new_foreground_attrs == original_foreground_attributes) &&
				(new_background_attrs == original_background_attributes);
		});

		// Restore everything
		SetColorAttributes(new_foreground_attrs, new_background_attrs, screen_buffer_info);
		Utils::WinApiHelpers::SetConsoleTextAttribute(console, screen_buffer_info.wAttributes);

		std::wcout << LR"EOS(
Whew, that was a close one!
With great power comes great responsibility, eh?
Hit 'g' to carry on.
)EOS";

		Utils::ExerciseHelpers::DebugBreak();
	}

	WORD WritingMemory::GetForegroundAttributes(const CONSOLE_SCREEN_BUFFER_INFOEX & screen_buffer_info)
	{
		return screen_buffer_info.wAttributes & kForegroundAttrsMask;
	}

	WORD WritingMemory::GetBackgroundAttributes(const CONSOLE_SCREEN_BUFFER_INFOEX & screen_buffer_info)
	{
		return (screen_buffer_info.wAttributes & kBackgroundAttrsMask) >> 4;
	}

	void WritingMemory::SetColorAttributes(WORD foreground, WORD background, CONSOLE_SCREEN_BUFFER_INFOEX & screen_buffer_info)
	{
		screen_buffer_info.wAttributes &= ~(kForegroundAttrsMask | kBackgroundAttrsMask);
		screen_buffer_info.wAttributes |= foreground;
		screen_buffer_info.wAttributes |= background << 4;
	}
}

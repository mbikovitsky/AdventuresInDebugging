#include <thread>

#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"
#include "..\..\..\Common\WinApiHelpers\WinApiHelpers.hpp"

#include "Threads.hpp"


namespace Aid
{
	void Threads::operator()()
	{
		std::wcout << LR"EOS(Is it progress I'm seeing? I think so!

Concurrency. We all want it, we all hate it.
Concurrent code is a pain to debug (there's one more argument in favor of writing bug-free code).
So why should I be the only one to suffer? Join in!

After you hit 'g', I'll spawn a thread and wait for it to complete.

The '~' command and its variants will help you.
)EOS";

		Utils::ExerciseHelpers::DebugBreak();

		std::thread busyThread([]() {
			volatile uint32_t value = 0;

			while (0x1337BEEF != value)
			{
			}
		});

		busyThread.join();

        std::wcout << LR"EOS(
Pshaw, that was too easy!
Let's try this again!

Read about the "!handle" command, then do what needs to be done.
And always remember: calling a function is just shuffling bits around.
)EOS";

        Utils::ExerciseHelpers::DebugBreak();

        std::thread waitThread([](){
            auto event = std::make_scoped_resource(::CreateEventW(NULL,
                                                                  TRUE,
                                                                  FALSE,
                                                                  L"MyAwesomeEvent"),
                                                   &::CloseHandle,
                                                   (HANDLE)NULL);
            if (!event.valid())
            {
                Utils::WinApiHelpers::ThrowLastError("CreateEventW");
            }

            ::WaitForSingleObject(event, INFINITE);
        });

        waitThread.join();

		std::wcout << LR"EOS(
Looks like you made it! Carry on.
)EOS";

		Utils::ExerciseHelpers::DebugBreak();
	}
}

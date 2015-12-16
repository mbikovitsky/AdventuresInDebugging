#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "Exceptions.hpp"


namespace Aid {


const ULONG_PTR Exceptions::kArguments[] =
{
#ifndef _WIN64
	1701017669,
	1869182064,
	1629516654,
	1713399154,
	2190965
#else
	8028075836850796613,
	7358993333053584238,
	2190965
#endif // _WIN64
};

void Exceptions::operator()()
{
    std::wcout << LR"EOS(Well, well, well! Looks like you made it to the last exercise
in basic training! Congrats! Let's get it over with so you can get to the challenges!

Right now the topic is exceptions.

This is not the place to describe the SEH mechanism, and anyway
I'm not very good at explaining it. However, you should know that
WinDbg has some nice facilities for dealing with Structured Exceptions.

Let's try some of that.
After you hit 'g' the program will throw an exception.
Figure out what type of exception it was, and type it at the prompt.

To do that, you'll have to tell the debugger to continue execution
despite the exception. The 'g' command has some interesting variants
to that end.

As to figuring out what the exception is... Well...
You'll just have to !analyze it :)

Good luck!
)EOS";

	Utils::ExerciseHelpers::DebugBreak();

	Utils::ExerciseHelpers::VerifyLoop([]()
	{
        return kExceptionCode == Utils::ExerciseHelpers::ReadNumberHex();
	}, &RaiseException);

    std::wcout << LR"EOS(
Well, that was too easy, if you ask me :)
That exception also had some parameters attached to it.
)EOS";

	uint64_t requested_parameter = Utils::ExerciseHelpers::Rand(0, _countof(kArguments) - 1);

    std::wcout << L"Tell me parameter no. " << requested_parameter << L" (0-based)" << std::endl;

	Utils::ExerciseHelpers::VerifyLoop([requested_parameter]()
	{
		return kArguments[requested_parameter] == Utils::ExerciseHelpers::ReadNumberHex();
	}, &RaiseException);
}

void Exceptions::RaiseException()
{
	static_assert(_countof(kArguments) <= EXCEPTION_MAXIMUM_PARAMETERS,
                  "Number of arguments must not exceed EXCEPTION_MAXIMUM_PARAMETERS");

	::RaiseException(kExceptionCode,
						0,
						_countof(kArguments),
						kArguments);
}


}

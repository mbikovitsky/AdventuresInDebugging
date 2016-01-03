#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"

#include "Stack.hpp"


namespace Aid
{
	void Stack::operator()()
	{
        std::wcout << LR"EOS(Mighty fine work! You now know how to display the opcodes you're debugging.

But displaying opcodes is only doing half the job.
When your code crashes on you, it's not enough to look at where you are right now - 
you should also figure out where you came from. That, my friends, is stack analysis.

"Stack analysis" is my fancy way of saying you should look at the call stack
and figure out the chain of calls that led you to this place.

You should also remember that the call stack holds addresses to which each
function *returns* (remember how the 'call' opcode works?)

Let's do some of that now.
Begin by telling me the return address of this very function.
That's the 'k' command and its variations for you.
)EOS";

		Utils::ExerciseHelpers::VerifyLoop([]()
		{
			// Use SEH to force creation of correct stack frame
			// (Optimizations are fun :))
			__try
			{
				// Inline the break
				__debugbreak();
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}

			auto returnAddress = reinterpret_cast<ptrdiff_t>(_ReturnAddress());
			auto userInput = static_cast<decltype(returnAddress)>(Utils::ExerciseHelpers::ReadNumberHex());
			
			return returnAddress == userInput;
		}, &Utils::ExerciseHelpers::NoBreak);

		std::wcout << LR"EOS(
So you think you're good, eh?
Tell you what: give me the value of the parameter that was
passed to this function, and then I'll be really impressed.
)EOS";

		Utils::ExerciseHelpers::VerifyLoop([]()
		{
			return FunctionWithParams(1801548883);
		}, &Utils::ExerciseHelpers::NoBreak);
	}

#pragma optimize("", off)
	bool Stack::FunctionWithParams(ptrdiff_t param)
	{
		// Inline the break
		__debugbreak();

		return param == static_cast<decltype(param)>(Utils::ExerciseHelpers::ReadNumberHex());
	}
#pragma optimize("", on)
}

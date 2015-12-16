#pragma once

#include <functional>


namespace Aid { namespace Utils { namespace ExerciseHelpers {


struct RetryExerciseStepException : virtual AidException
{
};

void VerifyLoop(std::function<bool()> predicate,
                std::function<void()> break_func = &DebugBreak);

void DebugBreak();
void NoBreak();

uint64_t Rand();
uint64_t Rand(uint64_t range_start, uint64_t range_end);

uint64_t ReadNumberHex();


} } }

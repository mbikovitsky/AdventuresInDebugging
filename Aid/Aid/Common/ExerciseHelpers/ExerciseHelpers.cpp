#include <chrono>
#include <boost\random\mersenne_twister.hpp>
#include <boost\random\uniform_int_distribution.hpp>

#include "ExerciseHelpers.hpp"


namespace Aid { namespace Utils { namespace ExerciseHelpers {


static boost::random::mt19937_64 g_generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());

typedef std::vector<const std::wstring> MessageVector;
static const MessageVector g_tryAgainMessages =
{
    L"Let's try that again, shall we?",
    L"Too bad, try again :(",
    L"\"If at first you don't succeed, get a bigger hammer.\" (- Alan Lewis)",
    L"Looks like you should debug it.",
    L"The answer here is probably not 42 (but it may be).",
    L"If \"Plan A\" didn't work, the alphabet has 25 more letters! Stay cool.",
    L"There are no bugs in this program.",
    L"There may be bugs in this program."
};


void VerifyLoop(std::function<bool()> predicate,
                std::function<void()> break_func)
{
    for (;;)
    {
        break_func();

        try
        {
            if (predicate())
            {
                break;
            }
        }
        catch (const RetryExerciseStepException &)
        {
        }

        auto messageIndex = Rand(0, g_tryAgainMessages.size() - 1);
        auto message = g_tryAgainMessages.at(static_cast<MessageVector::size_type>(messageIndex));
        std::wcout << message << std::endl;
    }
}

void DebugBreak()
{
    __debugbreak();
}

void NoBreak()
{
    // Don't break, as it will be implemented inside
    // the predicate.
    return;
}

std::uint64_t Rand()
{
    return g_generator();
}

std::uint64_t Rand(std::uint64_t range_start, std::uint64_t range_end)
{
    boost::random::uniform_int_distribution<std::uint64_t> distribution(range_start, range_end);

    return distribution(g_generator);
}

uint64_t ReadNumberHex()
{
    // Read a line from input
    std::wcout << L"> ";
    std::wstring user_input;
    std::getline(std::wcin, user_input);

    // Construct a stream
    std::wstringstream stream(user_input);

    // Obtain the value
    uint64_t target;
    stream >> std::hex >> target;

    // Abort if extraction failed
    if (!stream)
    {
        BOOST_THROW_EXCEPTION(RetryExerciseStepException());
    }

    return target;
}


} } }

#include <array>
#include <thread>

#include <boost\foreach.hpp>

#include "..\..\..\Common\ExerciseHelpers\ExerciseHelpers.hpp"
#include "..\..\..\Common\WinApiHelpers\WinApiHelpers.hpp"

#include "RocketScience.hpp"


typedef std::array<std::thread, MAXIMUM_WAIT_OBJECTS> ArrayType;

static const wchar_t *volatile g_atomName = L"Hello.txt";

// This is the good function.
// It access the global only for reading.
static bool GoodFunction()
{
    auto atom = Aid::Utils::WinApiHelpers::AddAtom(g_atomName);

    auto retrievedAtomName = Aid::Utils::WinApiHelpers::GetAtomName(atom);

    return retrievedAtomName == g_atomName;
}

// This is the bad function.
// It screws the global over.
static bool BadFunction()
{
    g_atomName = reinterpret_cast<const wchar_t *>(0xDEADBABE);

    return true;
}

void Aid::RocketScience()
{
    std::wcout << LR"EOS(Nice work! But your journey isn't over yet.
)EOS";

    Utils::ExerciseHelpers::DebugBreak();

    // Create an array of threads
    ArrayType threads;
    
    // Choose when the bad thread should be started
    ArrayType::size_type badIteration = Utils::ExerciseHelpers::Rand(0, threads.size() - 1);

    // Launch the threads
    for (auto iteration = 0;
         iteration < threads.size();
         ++iteration)
    {
        if (iteration == badIteration)
        {
            threads.at(iteration) = std::thread(&BadFunction);
            continue;
        }

        threads.at(iteration) = std::thread(&GoodFunction);
    }

    // Wait for the threads to return
    BOOST_FOREACH(auto &thread, threads)
    {
        thread.join();
    }
}

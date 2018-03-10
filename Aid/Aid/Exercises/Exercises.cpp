#include <fstream>
#include <boost\uuid\uuid_generators.hpp>
#include <boost\uuid\uuid_io.hpp>

#include "..\Common\Utils.hpp"

#include "Basics\Basics.hpp"
#include "Advanced\Advanced.hpp"

#include "Exercises.hpp"


namespace Aid { namespace Exercises {


extern ExerciseExecutor g_exercises =
{
    //
    // Basics
    //

    // Going Places
	{
		boost::uuids::string_generator()("{2B5D578C-CAFA-4ADD-A6F2-530218CDD748}"),
		L"Going Places",
		GoingPlaces()
	},

	// Breakpoints
	{
		boost::uuids::string_generator()("{55BE0186-3901-4D40-A892-6673152F2D2B}"),
		L"Breakpoints",
		Breakpoints()
	},

	// Reading memory
	{
		boost::uuids::string_generator()("{AA9F1A9E-88C6-4167-AF9F-A3CC33399E40}"),
		L"Reading memory",
		ReadingMemory()
	},

	// Writing memory
	{
		boost::uuids::string_generator()("{7DE2314C-7FBD-4F73-9326-56B77E964F81}"),
		L"Writing memory",
		WritingMemory()
	},

	// Disassembly
	{
		boost::uuids::string_generator()("{F03FB08E-2507-4C2D-8326-82FF259C395F}"),
		L"Disassembly",
		Disassembly()
	},

    // Structures
    {
        boost::uuids::string_generator()("{50FA4339-39C1-428A-9260-0F7285DDD22D}"),
        L"Structures",
        Structures()
    },

	// Stack
	{
		boost::uuids::string_generator()("{A75C1953-20F5-4648-8D35-6E27213EF71C}"),
		L"Stack",
		Stack()
	},

	// Exceptions
	{
		boost::uuids::string_generator()("{A536C225-BFFD-4042-B608-BE4EC1646E1D}"),
		L"Exceptions",
		Exceptions()
	},

    // WhatIfs
    {
        boost::uuids::string_generator()("{1F6EDE14-A527-4214-ACB5-F56427357F31}"),
        L"What Ifs",
        &WhatIfs
    },


    //
    // Advanced
    //

    // Failing Function
    {
        boost::uuids::string_generator()("{D5B4BBC0-1300-4C14-BE9C-BD9B92E9FDB5}"),
        L"Failing Function",
        &FailingFunction
    },

    // Children
    {
        boost::uuids::string_generator()("{95CD4DA9-C531-49FE-AB81-826D4CB4836D}"),
        L"Children",
        Children()
    },

    // Rocket Science
    {
        boost::uuids::string_generator()("{CE270227-25E8-4A9E-A941-5536EF45477E}"),
        L"Rocket Science",
        &RocketScience
    },

	// Threads
	{
		boost::uuids::string_generator()("{04D6A33B-430C-4693-8B57-6EA35366D42F}"),
		L"Threads",
		Threads()
	},

    // DeadInTheWater
    {
        boost::uuids::string_generator()("{CC2F61B8-A11B-4197-ADB3-AAF42F4330E1}"),
        L"Dead in the Water",
        &DeadInTheWater
    }
};

ExerciseExecutor::ExerciseExecutor(std::initializer_list<Exercise> exercises) :
    m_exercises(exercises),
    m_currentExercise(m_exercises.cbegin())
{
}

void ExerciseExecutor::Seek()
{
    Seek(m_exercises.cbegin()->id);
}

void ExerciseExecutor::Seek(const boost::uuids::uuid &id)
{
    for (auto exercise = m_exercises.cbegin();
         exercise != m_exercises.cend();
         ++exercise)
    {
        if (id == exercise->id)
        {
            m_currentExercise = exercise;
            return;
        }
    }

    //BOOST_THROW_EXCEPTION();
}

void ExerciseExecutor::Load(const std::wstring &fileName)
{
    std::wifstream inputFile(fileName, std::wifstream::binary);
    if (inputFile)
    {
        boost::uuids::uuid id;
        inputFile >> id;

        Seek(id);
    }
}

void ExerciseExecutor::Save(const std::wstring &fileName)
{
    std::wofstream outputFile(fileName, std::wofstream::binary);
    outputFile << m_currentExercise->id;
}

bool ExerciseExecutor::ExecuteOne()
{
    Utils::ClearScreen();

    std::wcout << L"Exercise: " << m_currentExercise->name << L" <" << m_currentExercise->id << L">" << std::endl;
    std::wcout << std::wstring(20, L'=') << std::endl << std::endl;

    m_currentExercise->implementation();

    ++m_currentExercise;

    return m_currentExercise != m_exercises.cend();
}


}}

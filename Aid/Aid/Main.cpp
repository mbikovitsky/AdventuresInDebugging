#include <deque>

#include <boost\lexical_cast.hpp>
#include <boost\filesystem.hpp>

#include "Common\Utils.hpp"

#include "Exercises\Exercises.hpp"

namespace Aid {


void DoStuff(const std::deque<std::wstring> & arguments)
{
	if (1 == arguments.size())
	{
        boost::filesystem::path modulePath(Utils::WinApiHelpers::GetModuleFileName());
        auto stateFileName = modulePath.parent_path() / L"aid.state";

        Exercises::g_exercises.Load(stateFileName.wstring());

        do 
        {
            Exercises::g_exercises.Save(stateFileName.wstring());
        } while (Exercises::g_exercises.ExecuteOne());
	}
	else
	{
        // Retrieve the function RVA
		auto rva = boost::lexical_cast<intptr_t>(arguments.at(1));

        // Obtain a pointer to the function
		auto rawPointer = Utils::RvaToPointer(rva);
		auto childFunction = reinterpret_cast<Utils::ChildFunction>(rawPointer);

		childFunction();
	}
}


}


int wmain(int argc, wchar_t **argv)
{
	try
	{
        const std::deque<std::wstring> arguments(argv, argv + argc);

		Aid::DoStuff(arguments);
	}
	catch (const std::exception &)
	{
		std::cerr << boost::current_exception_diagnostic_information(true) << std::endl;
	}

	return 0;
}

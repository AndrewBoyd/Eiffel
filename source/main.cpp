#include "eiffel/VisualStudioProjectFile.h"
#include "eiffel/SolutionManager.h"
#include <utility/ConsoleArguments.h>
#include <utility/global_config.h>
#include <iostream>
#include <set>

int main_impl(int num_args, const char* args[])
{
    using namespace eiffel;

    global_config::init(global_config::appdataPath() / "Eiffel" / "global_config.json");

    auto const arguments = ConsoleArguments(num_args, args);
    auto const project = arguments.getString("project");

    auto solution = createSolution(project);
    exportSolution(solution);
	
    return 0;
}

int main(int num_args, const char * args [])
{
	try
	{
		return main_impl(num_args, args);
	}
	catch (std::exception const& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}
}

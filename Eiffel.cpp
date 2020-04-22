#include "ConsoleArguments.h"
#include "VisualStudioProjectFile.h"
#include "SolutionManager.h"
#include <tinyxml.h>
#include <iostream>
#include <set>


int main(int number_of_args, const char * args [])
{
    using namespace eiffel;

    auto const arguments = ConsoleArguments(number_of_args, args);
    auto const project = arguments.getString("project");

    createSolution(project);

    //auto const project_file_data = createProjectFile(project_info);
    //exportProjectFiles(project_info, project_file_data);


    return 0;
}

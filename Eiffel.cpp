#include "ConsoleArguments.h"
#include "VisualStudioProjectFile.h"
#include "guid.h"
#include <tinyxml.h>
#include <iostream>
#include <fstream>
#include <set>

std::vector< std::filesystem::path > findDependencySearchPaths(eiffel::ProjectInfo const & project_info)
{ 
    auto global_config_path = "C:/Users/aboyd/AppData/Roaming/Eiffel/global_config.json";
    //auto global_config_path = "%APPDATA%/Eiffel/global_config.json"
    auto global_config = nlohmann::json::parse(std::ifstream(global_config_path));
    auto result = std::vector< std::filesystem::path >{ project_info.paths.root_directory / ".." };

    for (auto path_json : global_config["search_paths"])
    {
        result.push_back(path_json.get<std::string>());
    }

    result.push_back("");
    return result;
}

void findDependencies_impl(eiffel::ProjectInfo const& project_info,
    std::vector< std::filesystem::path > const & search_paths,
    std::set< std::filesystem::path >& result)
{
    auto dependencies_json = project_info.config["dependencies"];
    for (auto dependency_json : dependencies_json)
    {
        auto dependency = dependency_json.get<std::string>();
        auto found_dependency = false;
        for (auto path : search_paths)
        {
            auto test_path = path / dependency;
            if (std::filesystem::is_directory(test_path))
            {
                auto canon = std::filesystem::canonical( test_path );
                found_dependency = true;
                if (result.find(canon) == result.end())
                {
                    result.insert(canon);
                    if (eiffel::isEiffelProject(canon))
                    {
                        auto dependency_info = eiffel::getProjectInfo(canon);
                        findDependencies_impl(dependency_info, search_paths, result);
                    }
                }
                break;
            }
        }

        if (!found_dependency)
        {
            std::cout << "WARNING: Could not find dependency: " << dependency;
        }
    }
}

std::vector< std::filesystem::path > findDependencies(eiffel::ProjectInfo const & project_info, 
    std::vector< std::filesystem::path > search_paths) 
{
    auto result = std::set< std::filesystem::path >();
    findDependencies_impl(project_info, search_paths, result);
    return std::vector(result.begin(), result.end());
}

int main(int number_of_args, const char * args [])
{
    auto const arguments = ConsoleArguments(number_of_args, args);
    auto const project = arguments.getString("project");

    if (!eiffel::isEiffelProject(project))
    {
        std::cout << project << " is not an Eiffel project";
        return 1;
    }

    auto const project_info = eiffel::getProjectInfo(project);
    auto const search_paths = findDependencySearchPaths(project_info);
    auto const dependencies = findDependencies(project_info, search_paths);
    for (auto dep : dependencies)
        std::cout << dep << std::endl;

    auto const project_file_data = eiffel::createProjectFile(project_info);
    eiffel::exportProjectFiles(project_info, project_file_data);

    return 0;
}

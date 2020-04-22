#include "SolutionManager.h"
#include <fmt/format.h>
#include <set>
#include <fstream>
#include <iostream>

namespace eiffel 
{

    std::vector< std::filesystem::path > findDependencySearchPaths(eiffel::ProjectInfo const& project_info)
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

    void findDependencies_impl(ProjectId const & project_id, 
        ProjectInfo const& project_info,
        Solution& solution,
        std::vector< std::filesystem::path > const& search_paths,
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
                    auto canon = std::filesystem::canonical(test_path);
                    solution.dependency_tree[project_id].push_back(canon);
                    found_dependency = true;
                    if (result.find(canon) == result.end())
                    {
                        result.insert(canon);
                        if (eiffel::isEiffelProject(canon))
                        {
                            auto dependency_info = eiffel::getProjectInfo(canon);
                            findDependencies_impl(canon, dependency_info, solution, search_paths, result);
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

    ProjectIds findDependencies(ProjectId const& project_id, 
        ProjectInfo const& project_info,
        Solution& solution,
        std::vector< std::filesystem::path > search_paths)
    {
        auto result = std::set< std::filesystem::path >();
        findDependencies_impl(project_id, project_info, solution, search_paths, result);
        return std::vector(result.begin(), result.end());
    }

    ProjectIds findDependencies(ProjectId const& project_id,
        Solution& solution)
    {
        auto& project_info = solution.all_projects.at(project_id);
        auto search_paths = findDependencySearchPaths(project_info);
        return findDependencies(project_id, project_info, solution, search_paths);
    }
 
    void addNugetPackages(ProjectInfo const& project_info, Solution& solution)
    {
        auto nugets = findNugetPackages(project_info);
        for (auto& nuget : nugets) {
            solution.nuget_packages.insert(nuget);
        }
    }

    Solution createSolution(ProjectId project_id)
    {
        if (!isEiffelProject(project_id))
        {
            auto exception_string = fmt::format("Error: \"{}\" is not an Eiffel project", project_id.string());
            throw std::exception(exception_string.c_str());
        }

        project_id = std::filesystem::canonical(project_id);

        auto result = Solution{};
        result.main_project = project_id;
        auto & project_info = result.all_projects[ project_id ] = getProjectInfo(project_id);
        addNugetPackages(project_info, result);

        for (auto dep : findDependencies(project_id, result))
        {
            auto & info = result.all_projects[dep] = getLibProjectInfo(dep, project_id);
            addNugetPackages(info, result);
        }

        return result;
    }
}


#include "SolutionManager.h"
#include <utility/global_config.h>
#include <fmt/format.h>
#include <set>
#include <fstream>
#include <iostream>

namespace eiffel 
{

    std::vector< std::filesystem::path > findDependencySearchPaths(eiffel::ProjectInfo const& project_info)
    {
        auto& conf = global_config::get();
        auto result = std::vector< std::filesystem::path >{ project_info.paths.root_directory / ".." };

        for (auto path_json : conf["search_paths"])
        {
            result.push_back(path_json.get<std::string>());
        }

        result.push_back("");
        return result;
    }

    ProjectIds findTopLevelDependencies(ProjectInfo const& project_info)
    {
        auto dependencies_it = project_info.config.find("dependencies");
        if (dependencies_it == project_info.config.end()) return {};
        auto search_paths = findDependencySearchPaths(project_info);
        auto result = ProjectIds{};

        for (auto dependency_json : *dependencies_it)
        {
            auto dependency = dependency_json.get<std::string>();
            auto found_dependency = false;
            for (auto path : search_paths)
            {
                auto test_path = path / dependency;
                if (std::filesystem::is_directory(test_path))
                {
                    auto canon = std::filesystem::canonical(test_path);
                    result.push_back(canon);
                    found_dependency = true;
                    break;
                }
            }

            if (!found_dependency)
            {
                std::cout << "WARNING: Could not find dependency: " << dependency;
            }
        }

        return result;
    }

    void findDependencies_impl(ProjectId const & project_id, 
        ProjectInfo const& project_info,
        Solution& solution,
        std::vector< std::filesystem::path > const& search_paths,
        std::set< std::filesystem::path >& result)
    {
        // TODO: Pass in search paths
        auto dependencies = findTopLevelDependencies(project_info);
        solution.dependency_tree[project_id] = dependencies;
        for (auto& dep : dependencies)
        {
            if (result.find(dep) == result.end())
            {
                result.insert(dep);
                if (eiffel::isEiffelProject(dep))
                {
                    auto dependency_info = eiffel::getProjectInfo(dep);
                    findDependencies_impl(dep, dependency_info, solution, search_paths, result);
                }
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


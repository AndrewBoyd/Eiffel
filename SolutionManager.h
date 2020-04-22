#pragma once
#include "ProjectInfo.h"
#include "NugetPackage.h"
#include "guid.h"
#include <set>

namespace eiffel 
{
	using ProjectId = std::filesystem::path;
	using ProjectIds = std::vector< ProjectId >;

	struct Solution 
	{
		ProjectId main_project = {};
		std::set< NugetPackage > nuget_packages = {};
		std::map< ProjectId, ProjectInfo > all_projects = {};
	};

	std::vector< std::filesystem::path > findDependencySearchPaths(eiffel::ProjectInfo const& project_info);

	ProjectIds findDependencies(eiffel::ProjectInfo const& project_info,
		std::vector< std::filesystem::path > search_paths);

	ProjectIds findDependencies(eiffel::ProjectInfo const& project_info);

	Solution createSolution(ProjectId project_id);
}

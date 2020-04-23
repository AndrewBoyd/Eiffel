#pragma once
#include "ProjectInfo.h"
#include <utility/cpp20.h>
#include <vector>
#include <string>

namespace eiffel 
{
	struct NugetPackage
	{
		std::string package_id;
		std::string version;

		auto operator<=>(NugetPackage const& other) const = default;
	};

	std::vector<NugetPackage> findNugetPackages(ProjectInfo const& project_info);

	std::string getNugetTargetsFile(NugetPackage const& nuget_package, std::filesystem::path const& nugets_directory);

	bool installNugetPackage(NugetPackage package, std::filesystem::path location);
}

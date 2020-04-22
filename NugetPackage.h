#pragma once
#include "ProjectInfo.h"
#include "cpp20.h"
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

	std::string getNugetTargetsFile(NugetPackage const& nuget_package);

}

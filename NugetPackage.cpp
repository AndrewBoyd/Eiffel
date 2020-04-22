#include "NugetPackage.h"
#include "string_utils.h"
#include <fmt/format.h>

namespace eiffel {
	std::vector<NugetPackage> findNugetPackages(ProjectInfo const& project_info)
	{
		auto result = std::vector<NugetPackage>{};
		auto found_it = project_info.config.find("nuget");
		if (found_it == project_info.config.end()) return {};

		auto nugets_json = *found_it;
		for (auto nuget_json : nugets_json)
		{
			auto nuget_string = nuget_json.get<std::string>();
			auto split = string_utils::split(nuget_string, '|');
			if (split.size() != 2) throw std::exception("Malformed nuget string");

			auto nuget_package = NugetPackage{};
			nuget_package.package_id = split[0];
			nuget_package.version = split[1];
			result.push_back(nuget_package);
		}
		return result;
	}

	std::string getNugetTargetsFile(NugetPackage const& nuget_package)
	{
		return fmt::format("packages\\{0}.{1}\\build\\{0}.targets", nuget_package.package_id, nuget_package.version);
	}
}

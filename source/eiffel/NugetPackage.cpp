#include "NugetPackage.h"
#include <utility/string_utils.h>
#include <fmt/format.h>
#include <fstream>

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

	// This is a bit grim and perhaps unexpected behaviour due to the ..
	// But its what I need.
	std::string getNugetTargetsFile(NugetPackage const& nuget_package, std::filesystem::path const & nugets_directory)
	{
		auto search_path = nugets_directory / fmt::format("{}.{}", nuget_package.package_id, nuget_package.version);
		auto filename = fmt::format("{}.targets", nuget_package.package_id);
		for (auto dir_it : std::filesystem::recursive_directory_iterator(search_path))
		{
			if (std::filesystem::is_regular_file(dir_it)
				&& dir_it.path().filename() == filename)
			{
				return std::filesystem::relative(dir_it.path(), nugets_directory / "..").string();
			}
		}

		throw std::exception(fmt::format("Could not find {}", filename).c_str());
	}

	bool installNugetPackage(NugetPackage package, std::filesystem::path location)
	{
		auto bin_directory = getBinDirectory();
		auto nuget_path = std::filesystem::canonical(bin_directory / "nuget.exe");
		auto command = fmt::format(R"({} install {} -OutputDirectory "{}" -Version "{}")", 
			nuget_path.string(), package.package_id, location.string(), package.version);

		system(command.c_str());

		return false;
	}
}

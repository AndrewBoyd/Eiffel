#include "ProjectInfo.h"
#include <fstream>

auto const kProjectConfigFilename = "project_config.json";
auto const kVisualStudioProjectDirectory = "temp/vs";
auto const kCppFilesDirectory = "source";

namespace eiffel 
{
	bool isEiffelProject(std::filesystem::path directory)
	{
		auto config_file = directory / kProjectConfigFilename;
		return std::filesystem::exists(config_file);
	}

	ProjectPaths getProjectPaths(std::filesystem::path project_directory)
	{
		auto result = ProjectPaths{};
		result.root_directory = project_directory;
		result.vs_directory = project_directory / kVisualStudioProjectDirectory;
		result.source_directory = project_directory / kCppFilesDirectory;
		result.config_file = project_directory / kProjectConfigFilename;
		return result;
	}

	ProjectInfo getProjectInfo(std::filesystem::path project_directory)
	{
		auto result = ProjectInfo{};
		result.paths = getProjectPaths(project_directory);
		result.name = (--project_directory.end())->string();
		result.config = nlohmann::json::parse(std::ifstream(result.paths.config_file));
		return result;
	}
}


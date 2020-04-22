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

	bool isEiffelProject(ProjectInfo const& project)
	{
		return project.is_eiffel;
	}

	ProjectPaths getProjectPaths_Eiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectPaths{};
		result.root_directory = project_directory;
		result.vs_directory = main_project_directory / kVisualStudioProjectDirectory;
		result.source_directory = project_directory / kCppFilesDirectory;
		result.config_file = project_directory / kProjectConfigFilename;
		return result;
	}

	ProjectPaths getProjectPaths_NotEiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectPaths{};
		result.root_directory = project_directory;
		result.vs_directory = main_project_directory / kVisualStudioProjectDirectory;
		result.source_directory = project_directory;
		result.config_file = project_directory / kProjectConfigFilename;
		return result;
	}

	ProjectInfo getProjectInfo_Eiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectInfo{};
		result.paths = getProjectPaths_Eiffel(project_directory, main_project_directory);
		result.name = (--project_directory.end())->string();
		result.config = nlohmann::json::parse(std::ifstream(result.paths.config_file));
		result.guid = guid::generateGuid();
		result.is_eiffel = true;
		return result;
	}

	ProjectInfo getProjectInfo_NotEiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectInfo{};
		result.paths = getProjectPaths_Eiffel(project_directory, main_project_directory);
		result.name = (--project_directory.end())->string();
		result.config = nlohmann::json{};
		result.guid = guid::generateGuid();
		return result;
	}

	ProjectInfo getProjectInfo(std::filesystem::path project_directory)
	{
		return getProjectInfo(project_directory, project_directory);
	}

	ProjectInfo getProjectInfo(std::filesystem::path project_directory, 
		std::filesystem::path main_project_directory)
	{
		if (isEiffelProject(project_directory))
		{
			return getProjectInfo_Eiffel(project_directory, main_project_directory);
		}
		return getProjectInfo_NotEiffel(project_directory, main_project_directory);
	}
}


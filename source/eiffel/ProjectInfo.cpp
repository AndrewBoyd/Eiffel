#include "ProjectInfo.h"
#include <fstream>
#include <utility/global_config.h>

auto const kProjectConfigFilename = "project_config.json";
auto const kVisualStudioProjectDirectory = "temp/vs";
auto const kCppFilesDirectory = "source";
auto const kNugetDirectory = "temp/vs/packages";
auto const kAssetsDirectory = "assets";

namespace eiffel 
{
	std::filesystem::path getBinDirectory()
	{
		auto& conf = global_config::get();
		return conf["bin_directory"].get<std::string>();
	}

	bool isEiffelProject(std::filesystem::path directory)
	{
		auto config_file = directory / kProjectConfigFilename;
		return std::filesystem::exists(config_file);
	}

	bool isEiffelProject(ProjectInfo const& project)
	{
		return project.project_type != ProjectType::NotEiffel;
	}

	ProjectPaths getProjectPaths_Eiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectPaths{};
		result.root_directory = project_directory;
		result.source_directory = project_directory / kCppFilesDirectory;
		result.assets_directory = project_directory / kAssetsDirectory;

		result.nuget_directory = main_project_directory / kNugetDirectory;
		result.vs_directory = main_project_directory / kVisualStudioProjectDirectory;
		
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

	void determineIfTargetsProject(ProjectInfo& info)
	{
		if (auto found_it = info.config.find("targets"); 
			found_it != info.config.end())
		{
			auto file = found_it->get<std::string>();
			info.paths.targets_path = info.paths.root_directory / file;
			info.project_type = ProjectType::Targets;
		}
	}

	ProjectInfo getProjectInfo_Eiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectInfo{};
		result.paths = getProjectPaths_Eiffel(project_directory, main_project_directory);

		if (std::filesystem::exists(result.paths.source_directory / "main.cpp"))
			result.project_type = ProjectType::SourceCode_Application;
		else
			result.project_type = ProjectType::SourceCode_StaticLib;
		
		result.name = project_directory.filename().string();
		result.config = nlohmann::json::parse(std::ifstream(result.paths.config_file));
		result.guid = guid::generateGuid();
		determineIfTargetsProject( result );
		return result;
	}

	ProjectInfo getProjectInfo_NotEiffel(std::filesystem::path project_directory,
		std::filesystem::path main_project_directory)
	{
		auto result = ProjectInfo{};
		result.paths = getProjectPaths_NotEiffel(project_directory, main_project_directory);
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

	ProjectInfo getLibProjectInfo(std::filesystem::path project_directory, std::filesystem::path main_project_directory)
	{
		auto result = getProjectInfo(project_directory, main_project_directory);
		if (result.project_type == ProjectType::SourceCode_Application)
		{
			result.project_type = ProjectType::SourceCode_StaticLib;
		}
		return result;
	}

	bool requiresVcxproj(ProjectType project_type)
	{
		return (project_type == ProjectType::SourceCode_Application)
			|| (project_type == ProjectType::SourceCode_StaticLib);
	}

	bool dependencyRequiresReference(ProjectType project_type)
	{
		return (project_type == ProjectType::SourceCode_StaticLib);
	}
}


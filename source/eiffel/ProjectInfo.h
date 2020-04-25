#pragma once
#include <utility/guid.h>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace eiffel {
	using ProjectConfig = nlohmann::json;

	struct ProjectPaths
	{
		using Path = std::filesystem::path;
		Path root_directory = {};
		Path source_directory = {};
		Path vs_directory = {};
		Path config_file = {};
		Path nuget_directory = {};
		Path targets_path = {};
		Path assets_directory = {};
	};

	enum class ProjectType
	{
		NotEiffel,
		SourceCode_Application,
		SourceCode_StaticLib,
		Targets,
	};

	struct ProjectInfo
	{
		std::string name = "";
		ProjectConfig config = {};
		ProjectPaths paths = {};
		guid::Guid guid = {};
		ProjectType project_type = {};
	};

	bool isEiffelProject(std::filesystem::path directory);
	bool isEiffelProject(ProjectInfo const & project);
	ProjectInfo getProjectInfo(std::filesystem::path project_directory);
	ProjectInfo getProjectInfo(std::filesystem::path project_directory, std::filesystem::path main_project_directory);
	ProjectInfo getLibProjectInfo(std::filesystem::path project_directory, std::filesystem::path main_project_directory);
	bool requiresVcxproj(ProjectType project_type);
	bool dependencyRequiresReference(ProjectType project_type);
	std::filesystem::path getBinDirectory();
}



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
	};

	struct ProjectInfo
	{
		std::string name = "";
		ProjectConfig config = {};
		ProjectPaths paths = {};
		guid::Guid guid = {};
		bool is_eiffel = false;
		bool is_static_lib = false;
	};

	bool isEiffelProject(std::filesystem::path directory);
	bool isEiffelProject(ProjectInfo const & project);
	ProjectInfo getProjectInfo(std::filesystem::path project_directory);
	ProjectInfo getProjectInfo(std::filesystem::path project_directory, std::filesystem::path main_project_directory);
	ProjectInfo getLibProjectInfo(std::filesystem::path project_directory, std::filesystem::path main_project_directory);
}



#pragma once
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
	};

	struct ProjectInfo
	{
		std::string name = "";
		ProjectConfig config = {};
		ProjectPaths paths = {};
	};

	bool isEiffelProject(std::filesystem::path directory);
	ProjectInfo getProjectInfo(std::filesystem::path project_directory);


}

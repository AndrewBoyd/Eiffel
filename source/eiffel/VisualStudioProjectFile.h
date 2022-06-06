#pragma once
#include "ProjectInfo.h"
#include "NugetPackage.h"
#include <utility/cpp20.h>
#include <utility/guid.h>
#include "SolutionManager.h"
#include <filesystem>
#include <compare>
#include <string>
#include <vector>
#include <map>

namespace eiffel 
{
	struct Configuration 
	{
		std::string configuration = "";
		std::string platform = ""; 
		auto operator<=>(Configuration const &) const = default;
	};

	struct PchInfo {};

	struct CompileInfo 
	{
		PchInfo pch_info = {};
		std::string warning_level = "Level3";
		std::vector< std::string > preprocessor_definitions = { "_CONSOLE", "%(PreprocessorDefinitions)" };
		bool conformance_mode = true;
		std::string language_standard = "stdcpplatest";
		bool function_level_linking = false;
		bool intrinsic_functions = false;
		bool sdl_check = true;
		std::string additional_include_directories = "";
		std::string additional_options = "/experimental:preprocessor";
	};

	struct LinkInfo 
	{
		std::string subsystem = "Console";
		bool generate_debug_information = true;
		bool enable_comdat_folding = false;
		bool optimize_references = false;
	};

	struct PostBuildInfo
	{
		std::string event_command = {};
	};

	struct ConfigurationInfo
	{
		CompileInfo compile_info;
		LinkInfo link_info;
		PostBuildInfo post_build_info;
		bool link_incrmental = false;
		std::string platform_toolset = "v142";
		std::string character_set = "Unicode";
		bool whole_program_optimisation = false;
		std::string platform = "";
		bool use_debug_libraries = true;
		std::string configuration_type = "Application";
	};

	struct XmlInfo
	{
		std::string version = "1.0";
		std::string encoding = "utf-8";
	};

	struct VisualStudioProjectFile 
	{
		std::string vcproj_version = "16.0";
		std::string project_guid = guid::Guid{};
		std::string keyword = "Win32Proj";
		std::string windows_target_platform_version = "10.0";
		std::vector< std::string > configurations = { "Debug", "Release" };
		std::vector< std::string > platforms = { "x64" };
		std::map< Configuration, ConfigurationInfo > configuration_infos = {};

		std::vector< std::string > cpp_filenames;
		std::vector< std::string > h_filenames;

		std::vector< NugetPackage > nuget_packages;
		bool is_main_project = true;
		std::filesystem::path nuget_package_directory;
	};

	VisualStudioProjectFile createProjectFile(ProjectInfo const & project_info);

	void exportSolution(Solution const& solution);
}

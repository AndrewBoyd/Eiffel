#include "VisualStudioProjectFile.h"
#include "xylo_xml.h"
#include "string_utils.h"
#include <fmt/format.h>
#include <fstream>
#include <sstream>
#include <iostream>

auto const kNugetErrorText = "This project references NuGet package(s) that are missing on this computer. Use NuGet Package Restore to download them. For more information, see http ://go.microsoft.com/fwlink/?LinkID=322105. The missing file is {0}.";

namespace eiffel 
{
	void set32bit(ConfigurationInfo& info) 
	{
		info.platform = "Win32";
		info.compile_info.preprocessor_definitions.push_back("WIN32");
	}

	void set64bit(ConfigurationInfo& info) 
	{
		info.platform = "x64";
	}

	void setDebugBuild(ConfigurationInfo& info) 
	{
		info.compile_info.preprocessor_definitions.push_back("_DEBUG");
		info.link_incrmental = true;
	}

	void setReleaseBuild(ConfigurationInfo& info)
	{
		info.link_incrmental = false;
		info.whole_program_optimisation = true;
		info.use_debug_libraries = false;

		info.compile_info.preprocessor_definitions.push_back("NDEBUG");
		info.compile_info.intrinsic_functions = true;
		info.compile_info.function_level_linking = true;

		info.link_info.enable_comdat_folding = true;
		info.link_info.optimize_references = true;
	}
	
	void initProjectFile(VisualStudioProjectFile& project_file)
	{
		for (auto configuration : project_file.configurations)
		{
			for (auto platform : project_file.platforms)
			{
				auto config = Configuration{ configuration, platform };
				auto& config_info = project_file.configuration_infos[config];

				if (platform == "Win32") set32bit(config_info);
				else if (platform == "x64") set64bit(config_info);

				if (configuration == "Debug") setDebugBuild(config_info);
				else if (configuration == "Release") setReleaseBuild(config_info);
			}
		}
	}

	void findCppAndHFiles(ProjectInfo const& project_info, VisualStudioProjectFile & project_file)
	{
		auto search_location = project_info.paths.source_directory;

		if (!std::filesystem::is_directory(search_location)) return;

		for (auto file : std::filesystem::recursive_directory_iterator(search_location)) 
		{
			if (!file.is_regular_file()) continue;
			auto extension = file.path().extension().string();
			auto relative_path = std::filesystem::relative(file.path(), project_info.paths.vs_directory);
			
			if (extension == ".cpp") project_file.cpp_filenames.push_back(relative_path.string());
			if (extension == ".h") project_file.h_filenames.push_back(relative_path.string());
		}
	}

	void findNugetPackages(ProjectInfo const& project_info, VisualStudioProjectFile& project_file)
	{
		project_file.nuget_packages = findNugetPackages(project_info);
	}

	VisualStudioProjectFile createProjectFile(ProjectInfo const& project_info)
	{
		auto result = VisualStudioProjectFile{};
		initProjectFile(result);
		result.project_guid = project_info.guid;
		findCppAndHFiles(project_info, result);
		findNugetPackages(project_info, result);
		return result;
	}
	
	std::string getConfigurationSlug(Configuration const& conf)
	{
		return conf.configuration + "|" + conf.platform;
	}


	/////////////////////////////////////////////////////
	// Project //////////////////////////////////////////
	/////////////////////////////////////////////////////

	namespace vcxproj 
	{
		using Info = ProjectInfo;
		using Node = xylo::XmlNode;
		using Vs = VisualStudioProjectFile;

		std::string getConfigCondition(Configuration const& conf)
		{
			return fmt::format(R"('$(Configuration)|$(Platform)'=='{}|{}')", conf.configuration, conf.platform);
		}

		void setConfigCondition(Node& node, Configuration const& conf) 
		{
			node.setAttribute("Condition", getConfigCondition(conf));
		}

		void addProjectConfiguration(Node& parent, Configuration conf, ConfigurationInfo info)
		{
			auto node = parent.addNode( "ProjectConfiguration" );
			node.setAttribute("Include", getConfigurationSlug(conf));
			node.addNode("Configuration").setText(conf.configuration);
			node.addNode("Platform").setText(conf.platform);
		}

		void addItemGroup(Node& parent, Vs const& vs)
		{
			auto node = parent.addNode("ItemGroup");
			node.setAttribute("Label", "ProjectConfigurations");

			for (auto & [config, info] : vs.configuration_infos) 
			{
				addProjectConfiguration(node, config, info);
			}
		}

		void addGlobals(Node& parent, Vs const& vs, Info const& info)
		{
			auto node = parent.addNode("PropertyGroup");
			node.setAttribute("Label", "Globals");
			node.addNode("VCProjectVersion").setText(vs.vcproj_version);
			node.addNode("ProjectGuid").setText(vs.project_guid);
			node.addNode("Keyword").setText(vs.keyword);
			node.addNode("RootNamespace").setText(info.name);
			node.addNode("WindowsTargetPlatformVersion").setText(vs.windows_target_platform_version);
		}

		void addWindowsDefaultProps(Node& parent)
		{
			parent.addNode("Import").setAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
		}

		void addOtherWindowsProps(Node& parent)
		{
			parent.addNode("Import").setAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.props");
		}

		void addUserProps(Node& parent, Vs const& vs)
		{
			for (auto& [config, info] : vs.configuration_infos)
			{
				auto import_group = parent.addNode("ImportGroup");
				setConfigCondition(import_group, config);
				import_group.setAttribute("Label", "PropertySheets");
				{
					auto import = import_group.addNode("Import");
					import.setAttribute("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
					import.setAttribute("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
					import.setAttribute("Label", "LocalAppDataPlatform");
				}
			}
		}

		void addConfigurationPropertyGroup(Node& parent, Configuration conf, ConfigurationInfo info)
		{
			using namespace string_utils;
			auto node = parent.addNode("PropertyGroup");
			setConfigCondition(node, conf);
			node.setAttribute("Label", "Configutation");
			node.addNode("ConfigurationType").setText("Application");
			node.addNode("UseDebugLibraries").setText(b_to_s(info.use_debug_libraries));
			node.addNode("PlatformToolset").setText(info.platform_toolset);
			node.addNode("WholeProgramOptimization").setText(b_to_s(info.whole_program_optimisation));
			node.addNode("CharacterSet").setText(info.character_set);
		}

		void addConfigurationPropertyGroups(Node& parent, Vs const& vs) 
		{
			for (auto& [config, info] : vs.configuration_infos)
			{
				addConfigurationPropertyGroup(parent, config, info);
			}
		}		
		
		void addUserMacrosPropertyGroup(Node& parent)
		{
			parent.addNode("PropertyGroup").setAttribute("Label", "UserMacros");
		}

		void addIncLinkPropertyGroups(Node& parent, Vs const& vs)
		{
			using namespace string_utils;
			for (auto& [config, info] : vs.configuration_infos)
			{
				auto node = parent.addNode("PropertyGroup");
				setConfigCondition(node, config);
				node.addNode("LinkIncremental").setText(b_to_s(info.link_incrmental));
			}
		}

		void setItemDefCompile(Node& parent, ConfigurationInfo info)
		{
			using namespace string_utils;
			auto& ci = info.compile_info;
			auto node = parent.addNode("ClCompile");
			node.addNode("PrecompiledHeader");
			node.addNode("WarningLevel").setText(ci.warning_level);
			node.addNode("SDLCheck").setText(b_to_s(ci.sdl_check));
			node.addNode("PreprocessorDefinitions").setText( concat( ci.preprocessor_definitions, ";" ) );
			node.addNode("ConformanceMode").setText(b_to_s(ci.conformance_mode));
			node.addNode("LanguageStandard").setText(ci.language_standard);
		}

		void setItemDefLink(Node& parent, ConfigurationInfo info)
		{
			using namespace string_utils;
			auto& li = info.link_info;
			auto node = parent.addNode("Link");
			node.addNode("SubSystem").setText(li.subsystem);
			node.addNode("EnableCOMDATFolding").setText(b_to_s(li.enable_comdat_folding));
			node.addNode("OptimizeReferences").setText(b_to_s(li.optimize_references));
			node.addNode("GenerateDebugInformation").setText(b_to_s(li.generate_debug_information));
		}

		void addItemDefinitionGroups(Node& parent, Vs const& vs)
		{
			for (auto& [config, info] : vs.configuration_infos)
			{
				auto node = parent.addNode("ItemDefinitionGroup");
				setConfigCondition(node, config);
				setItemDefCompile(node, info);
				setItemDefLink(node, info);
			}
		}

		void addCompileFiles(Node& parent, Vs const& vs)
		{
			auto node = parent.addNode("ItemGroup");
			for (auto file : vs.cpp_filenames) 
			{
				node.addNode("ClCompile").setAttribute("Include", file);
			}
		}
		
		void addIncludeFiles(Node& parent, Vs const& vs)
		{
			auto node = parent.addNode("ItemGroup");
			for (auto file : vs.h_filenames)
			{
				node.addNode("ClInclude").setAttribute("Include", file);
			}
		}

		void addWindowsTargets(Node& parent)
		{
			parent.addNode("Import").setAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Targets");
		}

		void addNugetImports(Node& parent, Vs const& vs)
		{
			auto node = parent.addNode("ImportGroup");
			node.setAttribute("Label", "ExtensionTargets");
			for (auto nuget : vs.nuget_packages)
			{
				auto targets_file = getNugetTargetsFile(nuget);
				auto import_node = node.addNode("Import");
				import_node.setAttribute("Project", targets_file);
				import_node.setAttribute("Condition", fmt::format("Exists('{}')", targets_file));
			}
		}

		void addNugetErrorMessage(Node& parent)
		{
			auto node = parent.addNode("PropertyGroup");
			node.addNode("ErrorText").setText(kNugetErrorText);
		}
		
		void addNugetErrors(Node& parent, Vs const& vs)
		{
			auto node = parent.addNode("Target");
			node.setAttribute("Name", "EnsureNuGetPackageBuildImports");
			node.setAttribute("BeforeTargets", "PrepareForBuild");
			addNugetErrorMessage(node);
			for (auto& nuget_package : vs.nuget_packages)
			{
				auto targets_file = getNugetTargetsFile(nuget_package);
				auto condition = fmt::format("!Exists('{}')", targets_file);
				auto text = fmt::format("$([System.String]::Format('$(ErrorText)', '{}'))", targets_file);
				auto error_node = node.addNode("Error");
				error_node.setAttribute("Condition", condition);
				error_node.setAttribute("Text", text);
			}
		}		

		void writeProjectNode(std::stringstream & stream, Vs const& vs, Info const & info)
		{
			auto node = Node(&stream, "Project");
			node.setAttribute("DefaultTargets", "Build");
			node.setAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
			addItemGroup(node, vs);
			addGlobals(node, vs, info);
			addWindowsDefaultProps(node);
			addConfigurationPropertyGroups(node, vs);
			addOtherWindowsProps(node);
			addUserProps(node, vs);
			addUserMacrosPropertyGroup(node);
			addIncLinkPropertyGroups(node, vs);
			addItemDefinitionGroups(node, vs);
			addCompileFiles(node, vs);
			addIncludeFiles(node, vs);
			addWindowsTargets(node);
			addNugetImports(node, vs);
			addNugetErrors(node, vs);
		}

		void exportFile(std::filesystem::path filename, Vs const & vs, Info const & info)
		{
			auto stream = std::stringstream(std::ios_base::out);
			xylo::addXmlEncodingLine(stream);
			writeProjectNode(stream, vs, info);
			std::ofstream(filename) << stream.str();
		}
	}

	/////////////////////////////////////////////////////
	// Filters //////////////////////////////////////////
	/////////////////////////////////////////////////////


	/////////////////////////////////////////////////////
	// Packages /////////////////////////////////////////
	/////////////////////////////////////////////////////

	namespace packages_config
	{
		using Info = ProjectInfo;
		using Node = xylo::XmlNode;
		using Vs = VisualStudioProjectFile;

		void writePackagesNode(std::stringstream& stream, std::set<NugetPackage> const& nuget_packages)
		{
			auto packages_node = Node(&stream, "packages");
			for (auto& nuget_package : nuget_packages)
			{
				auto package_node = packages_node.addNode("package");
				package_node.setAttribute("id", nuget_package.package_id);
				package_node.setAttribute("version", nuget_package.version);
				package_node.setAttribute("targetFramework", "native");
			}
		}

		void exportFile(std::filesystem::path filename, std::set<NugetPackage> const& nuget_packages)
		{
			auto stream = std::stringstream(std::ios_base::out);
			xylo::addXmlEncodingLine(stream);
			writePackagesNode(stream, nuget_packages);
			std::ofstream(filename) << stream.str();
		}
	}

	/////////////////////////////////////////////////////
	// Solution /////////////////////////////////////////
	/////////////////////////////////////////////////////

	void exportProjectFile(ProjectInfo const& project_info, VisualStudioProjectFile const& project_file)
	{
		auto filename = project_info.paths.vs_directory / (project_info.name + ".vcxproj");
		std::cout << "Writing file : " << filename << std::endl;
		vcxproj::exportFile(filename, project_file, project_info);
	}

	void exportFiltersFile(ProjectInfo const& project_info, VisualStudioProjectFile const& project_file)
	{
	}

	void exportSolutionFile(ProjectInfo const& project_info, VisualStudioProjectFile const& project_file)
	{
	}

	void exportPackagesFile(Solution const & solution)
	{
		auto& project_info = solution.all_projects.at(solution.main_project);
		auto filename = project_info.paths.vs_directory / "packages.config";
		std::cout << "Writing file : " << filename << std::endl;
		packages_config::exportFile(filename, solution.nuget_packages);
	}

	void exportSolution(Solution const& solution)
	{
		exportPackagesFile(solution);

		for (auto& [project, project_info] : solution.all_projects)
		{
			auto project_file = createProjectFile(project_info);
			std::filesystem::create_directories(project_info.paths.vs_directory);
			exportProjectFile(project_info, project_file);
		}
	}



}


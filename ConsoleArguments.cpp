#include "ConsoleArguments.h"
#include <iostream>

ConsoleArguments::ConsoleArguments(int number_of_arguments, const char* arguments[])
{
	for (auto i = 1; i < number_of_arguments; ++i) 
	{
		auto argument_value_pair = std::string( arguments[i] );
		auto equals_sign_it = std::find(argument_value_pair.begin(), argument_value_pair.end(), '=');
		
		auto argument = std::string(argument_value_pair.begin() + 2, equals_sign_it);
		auto value = std::string(equals_sign_it + 1, argument_value_pair.end());
		arguments_[argument] = value;
	}
}

std::map<std::string, std::string> const& ConsoleArguments::getArguments() const
{
	return arguments_;
}

std::string ConsoleArguments::getString(std::string argument_name, std::string default_value) const
{
	if (auto optional_arg = findString(argument_name)) 
	{
		return *optional_arg;
	}
	return default_value;
}

std::string ConsoleArguments::getString(std::string argument_name) const
{
	return arguments_.at(argument_name);
}

std::optional<std::string> ConsoleArguments::findString(std::string argument_name) const
{
	if (auto found_it = arguments_.find(argument_name); found_it != arguments_.end()) 
	{
		return found_it->second;
	}
	return {};
}



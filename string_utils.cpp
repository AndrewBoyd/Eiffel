#include "string_utils.h"
#include <sstream>

namespace string_utils 
{
	std::string b_to_s(bool x)
	{
		if (x) return "true";
		return "false";
	}

	std::string concat(std::vector<std::string> const& strings, std::string delimiter)
	{
		auto result = std::stringstream();
		for (auto string_it = strings.begin(); string_it != strings.end(); string_it++)
		{
			if (string_it != strings.begin()) result << delimiter;
			result << *string_it;
		}
		return result.str();
	}

	std::vector<std::string> split(std::string string, char delimiter)
	{
		auto result = std::vector<std::string>();
		auto stream = std::stringstream(string);
		auto buffer = std::string{};

		while (std::getline(stream, buffer, delimiter)) 
		{
			result.push_back(buffer);
		}

		return result;
	}
}

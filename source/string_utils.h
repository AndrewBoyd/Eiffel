#pragma once
#include <string>
#include <vector>

namespace string_utils 
{
	std::string b_to_s(bool x);
	std::string concat(std::vector<std::string> const& strings, std::string delimiter);
	std::vector< std::string > split(std::string string, char delimiter);

}

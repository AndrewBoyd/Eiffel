#pragma once

#include <string>
#include <compare>

namespace std 
{
	std::strong_ordering operator<=>(std::string const& lhs, std::string const& rhs);
}

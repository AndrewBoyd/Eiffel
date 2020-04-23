#include "cpp20.h"

namespace std {
	std::strong_ordering operator<=>(std::string const& lhs, std::string const& rhs)
	{
		if (lhs < rhs) return std::strong_ordering::less;
		else if (lhs > rhs) return std::strong_ordering::greater;
		else return std::strong_ordering::equal;
	}
}

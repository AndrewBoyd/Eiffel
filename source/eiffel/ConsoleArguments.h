#include <map>
#include <string>
#include <optional>

class ConsoleArguments {
public:
	ConsoleArguments(int number_of_arguments, const char* arguments[]);

	std::map < std::string, std::string > const& getArguments() const;

	std::string getString( std::string argument_name, std::string default_value ) const;
	std::string getString(std::string argument_name) const;
	std::optional<std::string> findString(std::string argument_name) const;

private:
	std::map < std::string, std::string > arguments_;
};


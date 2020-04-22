#pragma once
#include <string>
#include <sstream>
#include <map>

namespace xylo 
{
	using stream_t = std::stringstream;

	void addXmlEncodingLine( stream_t & stream, std::string version = "1.0", std::string encoding = "utf-8");

	class XmlNode 
	{
	public:
		XmlNode(stream_t * stream, std::string node_name);
		~XmlNode();
		XmlNode addNode(std::string node_name);
		stream_t & getStream();

		void setAttribute(std::string attribute, std::string value);
		void setText(std::string text);
	private:
		bool has_children_ = false;
		std::map<std::string, std::string> attributes_ = {};
		stream_t* stream_ = nullptr;
		std::stringstream children_stream;
		std::string node_name_ = {};
		std::string text_ = {};
	};
}

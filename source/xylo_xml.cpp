#include "xylo_xml.h"
#include <fmt/format.h>

namespace xylo 
{
	void addXmlEncodingLine(stream_t & stream, std::string version, std::string encoding)
	{
		auto to_format = R"(<?xml version="{}" encoding="{}"?>)";
		stream << fmt::format(to_format, version, encoding) << "\n";
	}

	XmlNode::XmlNode(stream_t * stream, std::string node_name)
		: stream_(stream)
		, node_name_(std::move(node_name))
	{ }

	XmlNode::~XmlNode()
	{
		*stream_ << "<" << node_name_;
		for (auto [attribute, value] : attributes_) 
		{
			auto to_format = R"( {}="{}")";
			*stream_ << fmt::format(to_format, attribute, value);
		}

		if (has_children_)
		{
			auto to_format = ">\n{}</{}>\n";
			children_stream << text_;
			*stream_ << fmt::format(to_format, children_stream.str(), node_name_);
		}
		else if (!text_.empty())
		{
			auto to_format = ">{}</{}>\n";
			*stream_ << fmt::format(to_format, text_, node_name_);
		}
		else
		{
			*stream_ << "/>\n";
		}
	}

	XmlNode XmlNode::addNode(std::string node_name)
	{
		has_children_ = true;
		return XmlNode(&children_stream, std::move(node_name));
	}

	stream_t& XmlNode::getStream()
	{
		return *stream_;
	}

	void XmlNode::setAttribute(std::string attribute, std::string value)
	{
		attributes_[attribute] = value;
	}

	void XmlNode::setText(std::string text)
	{
		text_ = text;
	}

}


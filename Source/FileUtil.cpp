#include "FileUtil.h"
#include "Log.h"

#include <fstream>

namespace PeopleNetwork::FileUtil
{
	bool Read(std::string_view path, std::string& out)
	{
		std::ifstream file(path.data(), std::ios::in);
		if (!file)
		{
			LOG_ERROR("Cannot read from file '" << path << "', failed to open a stream to it.");
			return false;
		}
		file.seekg(0, std::ios::end);
		long size = (long) file.tellg();
		file.seekg(0, std::ios::beg);

		out.clear();
		out.resize(size);
		file.read(out.data(), (std::streamsize) size);
		
		return true;
	}

	bool ReadJson(std::string_view path, Json::Value& out)
	{
		std::string content;
		if (!Read(path, content))
		{
			return false;
		}

		Json::Reader rd;
		if (!rd.parse(content.data(), out))
		{
			LOG_ERROR("Failed to parse JSON file '" << path << "'. Json::Reader parsing error: " << rd.getFormattedErrorMessages());
			return false;
		}

		return true;
	}
}

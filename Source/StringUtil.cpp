#include "StringUtil.h"

#include <codecvt>

namespace PeopleNetwork::StringUtil
{
	std::wstring u8tow(const std::string& string)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.from_bytes(string);
	}
}

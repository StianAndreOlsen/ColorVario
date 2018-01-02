#include "StringFunctions.h"

std::string Kystsoft::trim(const std::string& str, const std::string& trim_chars /*= " \t\v\f\r\n\0"*/)
{
	auto first = str.find_first_not_of(trim_chars);
	if (first == std::string::npos)
		return std::string();
	auto last = str.find_last_not_of(trim_chars);
	return str.substr(first, last + 1 - first);
}

std::string Kystsoft::trimStart(const std::string& str, const std::string& trim_chars /*= " \t\v\f\r\n\0"*/)
{
	auto first = str.find_first_not_of(trim_chars);
	if (first == std::string::npos)
		return std::string();
	return str.substr(first);
}

std::string Kystsoft::trimEnd(const std::string& str, const std::string& trim_chars /*= " \t\v\f\r\n\0"*/)
{
	auto last = str.find_last_not_of(trim_chars);
	if (last == std::string::npos)
		return std::string();
	return str.substr(0, last + 1);
}

bool Kystsoft::isQuoted(const std::string& str, char quote /*= '"'*/)
{
	if (str.size() < 2)
		return false;
	return str.front() == quote && str.back() == quote;
}

std::string Kystsoft::quote(const std::string& str, char quote /*= '"'*/)
{
	if (isQuoted(str, quote))
		return str;
	return quote + str + quote;
}

std::string Kystsoft::unquote(const std::string& str, char quote /*= '"'*/)
{
	if (!isQuoted(str, quote))
		return str;
	return str.substr(1, str.size() - 2);
}

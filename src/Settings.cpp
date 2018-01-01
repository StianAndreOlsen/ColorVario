#include "Settings.h"
#include "dlog.h"
#include "StringFunctions.h"
#include <fstream>

bool Kystsoft::Settings::load(const std::string& fileName)
{
	std::ifstream is(fileName);
	if (!is.is_open() || !load(is))
	{
		dlog(DLOG_ERROR) << "Unable to load settings from \"" << fileName << "\"";
		return false;
	}
	dlog(DLOG_DEBUG) << "Settings loaded from \"" << fileName << "\"";
	return true;
}

bool Kystsoft::Settings::load(std::istream& is)
{
	std::ostringstream os;
	removeComments(is, os);
	std::istringstream iss(os.str());
	return loadValues(iss) > 0;
}

void Kystsoft::Settings::removeComments(std::istream& is, std::ostream& os)
{
	char c = 0;
	while (is.get(c))
	{
		if (c == ';' || c == '#')
		{
			// bypass line
			while (is.get(c))
				if (c == '\n')
					{ os << c; break; }
		}
		else
		{
			os << c;
		}
	}
}

std::string Kystsoft::Settings::value(const std::string& key) const
{
	auto i = valueMap.find(key);
	if (i == valueMap.end())
		return std::string();
	return i->second;
}

std::vector<std::string> Kystsoft::Settings::values(const std::string& key) const
{
	std::vector<std::string> values;
	auto range = valueMap.equal_range(key);
	for (auto i = range.first; i != range.second; ++i)
		values.push_back(i->second);
	return values;
}

size_t Kystsoft::Settings::loadValues(std::istream& is)
{
	size_t initialSize = valueMap.size();
	std::string section;
	std::string line;
	while (std::getline(is, line))
	{
		// [section]
		{
			std::istringstream iss(line);
			char c = 0;
			if (iss >> c && c == '[' && std::getline(iss, section, ']'))
			{
				section = trim(section);
				continue; // read next line
			}
		}
		// name=value
		{
			std::istringstream iss(line);
			std::string name, value;
			if (std::getline(iss, name, '=') && std::getline(iss, value))
			{
				name = trim(name);
				value = trim(value);
				if (!name.empty() && !value.empty())
					valueMap.insert(std::pair<std::string, std::string>(section + '.' + name, value));
			}
		}
	}
	return valueMap.size() - initialSize;
}

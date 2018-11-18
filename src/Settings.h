#ifndef KYSTSOFT_SETTINGS_H
#define KYSTSOFT_SETTINGS_H

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace Kystsoft {

// Reads ini-files and store the contents as key-value pairs
// The final keys contain the section name as in 'section.name'
// https://en.wikipedia.org/wiki/INI_file
class Settings
{
public:
	Settings() {}
	Settings(const std::string& fileName) { load(fileName); }
	bool load(const std::string& fileName);
	bool load(std::istream& is);
	static void removeComments(std::istream& is, std::ostream& os);
	size_t size() const { return valueMap.size(); }
	bool hasValue(const std::string& key) const;
	std::string value(const std::string& key) const;
	std::vector<std::string> values(const std::string& key) const;
	template<typename T>
	T value(const std::string& key, T defaultValue) const;
private:
	size_t loadValues(std::istream& is);
	std::multimap<std::string, std::string> valueMap;
};

} // namespace Kystsoft

template<typename T>
T Kystsoft::Settings::value(const std::string& key, T defaultValue) const
{
	std::istringstream is(value(key));
	is.setf(std::ios_base::boolalpha);
	T val;
	if (is >> val)
		return val;
	return defaultValue;
}

#endif // KYSTSOFT_SETTINGS_H

#ifndef KYSTSOFT_SETTINGS_H
#define KYSTSOFT_SETTINGS_H

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace Kystsoft {

// Reads ini-files (https://en.wikipedia.org/wiki/INI_file) and store the
// contents as key-value pairs. The final keys contain the section name as
// in 'section.name'
class Settings
{
public:
	Settings() {}
	Settings(const std::string& fileName) { load(fileName); }
	bool load(const std::string& fileName);
	bool load(std::istream& is);
	static void removeComments(std::istream& is, std::ostream& os);
	size_t size() const { return valueMap.size(); }
	std::string value(const std::string& key) const;
	std::vector<std::string> values(const std::string& key) const;
	template<class T>
	T value(const std::string& key, T defaultValue) const;
private:
	size_t loadValues(std::istream& is);
	std::multimap<std::string, std::string> valueMap;
};

} // namespace Kystsoft

template<class T>
T Kystsoft::Settings::value(const std::string& key, T defaultValue) const
{
	std::istringstream is(value(key));
	T val;
	if (is >> val)
		return val;
	return defaultValue;
}

#endif // KYSTSOFT_SETTINGS_H

#ifndef KYSTSOFT_SYSTEMINFO_H
#define KYSTSOFT_SYSTEMINFO_H

#include <string>

namespace Kystsoft {
namespace SystemInfo {

// https://developer.tizen.org/development/guides/native-application/device-settings-and-systems/system-information
bool platformBool(const std::string& key);
int platformInt(const std::string& key);
double platformDouble(const std::string& key);
std::string platformString(const std::string& key);

} // namespace SystemInfo
} // namespace Kystsoft

#endif // KYSTSOFT_SYSTEMINFO_H

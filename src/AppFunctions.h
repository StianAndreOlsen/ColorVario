#ifndef KYSTSOFT_APPFUNCTIONS_H
#define KYSTSOFT_APPFUNCTIONS_H

#include <string>

namespace Kystsoft {

std::string appId();
std::string appName();
std::string appVersion();
std::string appDataPath();
std::string appResourcePath();
std::string appSharedDataPath();
std::string appSharedResourcePath();

} // namespace Kystsoft

#endif // KYSTSOFT_APPFUNCTIONS_H

#ifndef KYSTSOFT_STRINGFUNCTIONS_H
#define KYSTSOFT_STRINGFUNCTIONS_H

#include <string>

namespace Kystsoft {

std::string trim(const std::string& str, const std::string& trim_chars = " \t\v\f\r\n\0");
std::string trimStart(const std::string& str, const std::string& trim_chars = " \t\v\f\r\n\0");
std::string trimEnd(const std::string& str, const std::string& trim_chars = " \t\v\f\r\n\0");
bool isQuoted(const std::string& str, char quote = '"');
std::string quote(const std::string& str, char quote = '"');
std::string unquote(const std::string& str, char quote = '"');

} // namespace Kystsoft

#endif // KYSTSOFT_STRINGFUNCTIONS_H

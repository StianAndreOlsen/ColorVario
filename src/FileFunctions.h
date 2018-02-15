#ifndef KYSTSOFT_FILEFUNCTIONS_H
#define KYSTSOFT_FILEFUNCTIONS_H

#include <string>

namespace Kystsoft {

class FileFunctionsInitializer
{
public:
	FileFunctionsInitializer();
	~FileFunctionsInitializer();
};

bool fileExists(const std::string& file);
bool isDirectory(const std::string& dir);
bool makeDirectory(const std::string& dir);
bool copyFile(const std::string& src, const std::string& dst);
bool moveFile(const std::string& src, const std::string& dst);

} // namespace Kystsoft

#endif // KYSTSOFT_FILEFUNCTIONS_H

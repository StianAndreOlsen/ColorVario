#ifndef KYSTSOFT_FILEFUNCTIONS_H
#define KYSTSOFT_FILEFUNCTIONS_H

#include <string>

namespace Kystsoft {

class FileFunctionsInitializer
{
public:
	FileFunctionsInitializer();
	~FileFunctionsInitializer() noexcept;
};

bool fileExists(const std::string& file);
bool isDirectory(const std::string& dir);
bool makeDirectory(const std::string& dir);
bool makePath(const std::string& path);
bool copyFile(const std::string& src, const std::string& dst);
bool moveFile(const std::string& src, const std::string& dst);
bool removeFile(const std::string& file);

} // namespace Kystsoft

#endif // KYSTSOFT_FILEFUNCTIONS_H

#include "FileFunctions.h"
#include <Ecore_File.h>

Kystsoft::FileFunctionsInitializer::FileFunctionsInitializer()
{
	ecore_file_init();
}

Kystsoft::FileFunctionsInitializer::~FileFunctionsInitializer()
{
	ecore_file_shutdown();
}

bool Kystsoft::fileExists(const std::string& file)
{
	return (ecore_file_exists(file.c_str()) == EINA_TRUE) ? true : false;
}

bool Kystsoft::isDirectory(const std::string& dir)
{
	return (ecore_file_is_dir(dir.c_str()) == EINA_TRUE) ? true : false;
}

bool Kystsoft::makeDirectory(const std::string& dir)
{
	return (ecore_file_mkdir(dir.c_str()) == EINA_TRUE) ? true : false;
}

bool Kystsoft::makePath(const std::string& path)
{
	return (ecore_file_mkpath(path.c_str()) == EINA_TRUE) ? true : false;
}

bool Kystsoft::copyFile(const std::string& src, const std::string& dst)
{
	return (ecore_file_cp(src.c_str(), dst.c_str()) == EINA_TRUE) ? true : false;
}

bool Kystsoft::moveFile(const std::string& src, const std::string& dst)
{
	return (ecore_file_mv(src.c_str(), dst.c_str()) == EINA_TRUE) ? true : false;
}

bool Kystsoft::removeFile(const std::string& file)
{
	return (ecore_file_remove(file.c_str()) == EINA_TRUE) ? true : false;
}

#include "FileFunctions.h"
#include "dlog.h"
#include <Ecore_File.h>

Kystsoft::FileFunctionsInitializer::FileFunctionsInitializer()
{
	ecore_file_init();
}

Kystsoft::FileFunctionsInitializer::~FileFunctionsInitializer() noexcept
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
	if (ecore_file_mkdir(dir.c_str()) == EINA_TRUE)
	{
		dlog(DLOG_DEBUG) << "\"" << dir << "\" is created";
		return true;
	}
	dlog(DLOG_ERROR) << "Unable to create \"" << dir << "\"";
	return false;
}

bool Kystsoft::makePath(const std::string& path)
{
	if (ecore_file_mkpath(path.c_str()) == EINA_TRUE)
	{
		dlog(DLOG_DEBUG) << "\"" << path << "\" is created";
		return true;
	}
	dlog(DLOG_ERROR) << "Unable to create \"" << path << "\"";
	return false;
}

bool Kystsoft::copyFile(const std::string& src, const std::string& dst)
{
	if (ecore_file_cp(src.c_str(), dst.c_str()) == EINA_TRUE)
	{
		dlog(DLOG_DEBUG) << "\"" << src << "\" is copied to \"" << dst << "\"";
		return true;
	}
	dlog(DLOG_ERROR) << "Unable to copy \"" << src << "\" to \"" << dst << "\"";
	return false;
}

bool Kystsoft::moveFile(const std::string& src, const std::string& dst)
{
	if (ecore_file_mv(src.c_str(), dst.c_str()) == EINA_TRUE)
	{
		dlog(DLOG_DEBUG) << "\"" << src << "\" is moved to \"" << dst << "\"";
		return true;
	}
	dlog(DLOG_ERROR) << "Unable to move \"" << src << "\" to \"" << dst << "\"";
	return false;
}

bool Kystsoft::removeFile(const std::string& file)
{
	if (ecore_file_remove(file.c_str()) == EINA_TRUE)
	{
		dlog(DLOG_DEBUG) << "\"" << file << "\" is removed";
		return true;
	}
	dlog(DLOG_ERROR) << "Unable to remove \"" << file << "\"";
	return false;
}

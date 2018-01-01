#include "dlog.h"
#include "AppFunctions.h"
#include "TimeFunctions.h"
#include <fstream>

Kystsoft::dlog::~dlog() noexcept
{
	// print to system log
	dlog_print(priority, tag.c_str(), str().c_str());

	// TODO: Remove comments when we are able to get the log file from the watch
	/*
	// print to file log
	static bool failed = false;
	if (!failed)
	{
		try
		{
			static const std::string fileName(appDataPath() + "Kystsoft.log");
			static std::ofstream os(fileName);
			if (!os.is_open())
				throw std::ios_base::failure("Cannot open \"" + fileName + "\" for writing");
			if (!(os << currentDateAndTime() << '\t' << priorityString() << '\t' << tag << '\t' << str() << std::endl))
				throw std::ios_base::failure("Cannot print to \"" + fileName + "\"");
		}
		catch (std::exception& e)
		{
			dlog_print(DLOG_ERROR, tag.c_str(), e.what());
			failed = true;
		}
		catch (...)
		{
			dlog_print(DLOG_ERROR, tag.c_str(), "Cannot print to file log");
			failed = true;
		}
	}
	*/
}

std::string Kystsoft::dlog::priorityString(log_priority priority)
{
	switch (priority)
	{
	case DLOG_UNKNOWN:  return "Unknown";
	case DLOG_DEFAULT:  return "Default";
	case DLOG_VERBOSE:  return "Verbose";
	case DLOG_DEBUG:    return "Debug";
	case DLOG_INFO:     return "Info";
	case DLOG_WARN:     return "Warning";
	case DLOG_ERROR:    return "Error";
	case DLOG_FATAL:    return "Fatal";
	case DLOG_SILENT:   return "Silent";
	case DLOG_PRIO_MAX: return "Maximum";
	}
	return "Invalid";
}

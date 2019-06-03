#include "dlog.h"
#include "AppFunctions.h"
#include "TimeFunctions.h"
#include <fstream>

namespace
{
	std::ostringstream oss; // string stream buffer
	std::ofstream ofs; // file stream
	std::ostream* os = &oss; // current output stream
}

std::string Kystsoft::dlog::tag = Kystsoft::appName(); // TODO: What happens if appName throws an exception?

Kystsoft::dlog::~dlog() noexcept
{
	// print to system log
	dlog_print(priority, tag.c_str(), str().c_str());

	// print to current output stream
	if (os != nullptr)
	{
		try
		{
			if (!(*os << currentDateAndTime() << '\t' << priorityString() << '\t' << str() << std::endl))
			{
				std::string error = "Unable to print debug logs";
				if (os == &oss)
					error += " to string stream";
				else if (os == &ofs)
					error += " to file stream";
				throw std::runtime_error(error);
			}
		}
		catch (std::exception& e)
		{
			dlog_print(DLOG_ERROR, tag.c_str(), e.what());
			os = nullptr; // stop stream logging
		}
		catch (...)
		{
			dlog_print(DLOG_ERROR, tag.c_str(), "Unable to print debug logs");
			os = nullptr; // stop stream logging
		}
	}
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

void Kystsoft::setDebugLogFile(const std::string& fileName)
{
	if (fileName.empty())
	{
		os = nullptr; // stop stream logging
		oss.str(""); // clear string stream buffer
		return;
	}
	ofs.open(fileName, std::ios_base::app); // open file stream in append mode
	if (ofs.fail())
	{
		os = nullptr; // stop stream logging
		oss.str(""); // clear string stream buffer
		dlog(DLOG_ERROR) << "Unable to print debug logs to \"" << fileName << "\"";
		return;
	}
	os = &ofs; // from now on; print logs directly to file stream
	ofs << oss.str() << std::flush; // print string stream buffer to file stream
	oss.str(""); // clear string stream buffer
}

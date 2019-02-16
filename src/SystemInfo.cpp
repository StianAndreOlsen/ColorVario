#include "SystemInfo.h"
#include "TizenError.h"
#include <system_info.h>

namespace
{
	std::string prefix("http://tizen.org/");
}

bool Kystsoft::SystemInfo::platformBool(const std::string& key)
{
	bool value = false;
	int error = system_info_get_platform_bool((prefix + key).c_str(), &value);
	if (error != SYSTEM_INFO_ERROR_NONE)
		throw TizenError("system_info_get_platform_bool", error);
	return value;
}

int Kystsoft::SystemInfo::platformInt(const std::string& key)
{
	int value = 0;
	int error = system_info_get_platform_int((prefix + key).c_str(), &value);
	if (error != SYSTEM_INFO_ERROR_NONE)
		throw TizenError("system_info_get_platform_int", error);
	return value;
}

double Kystsoft::SystemInfo::platformDouble(const std::string& key)
{
	double value = 0;
	int error = system_info_get_platform_double((prefix + key).c_str(), &value);
	if (error != SYSTEM_INFO_ERROR_NONE)
		throw TizenError("system_info_get_platform_double", error);
	return value;
}

std::string Kystsoft::SystemInfo::platformString(const std::string& key)
{
	char* value = nullptr;
	int error = system_info_get_platform_string((prefix + key).c_str(), &value);
	if (error != SYSTEM_INFO_ERROR_NONE)
		throw TizenError("system_info_get_platform_string", error);
	std::string str(value);
	free(value);
	return str;
}

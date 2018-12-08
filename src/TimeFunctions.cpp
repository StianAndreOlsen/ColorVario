#include "TimeFunctions.h"
#include <ctime>

std::string Kystsoft::currentDate()
{
	// get local time
	time_t t = std::time(nullptr);
	tm* lt = std::localtime(&t);
	if (!lt)
		return "?";

	// create formatted date string
	char date[11] = "";
	if (std::strftime(date, sizeof(date), "%Y-%m-%d", lt) != 10)
		return "?";

	return date;
}

std::string Kystsoft::currentTime()
{
	// get local time
	time_t t = std::time(nullptr);
	tm* lt = std::localtime(&t);
	if (!lt)
		return "?";

	// create formatted time string
	char time[9] = "";
	if (std::strftime(time, sizeof(time), "%H:%M:%S", lt) != 8)
		return "?";

	return time;
}

std::string Kystsoft::currentDateAndTime()
{
	// get local time
	time_t t = std::time(nullptr);
	tm* lt = std::localtime(&t);
	if (!lt)
		return "?";

	// create formatted date and time string
	char date_time[20] = "";
	if (std::strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", lt) != 19)
		return "?";

	return date_time;
}

#ifndef KYSTSOFT_TIZENERROR_H
#define KYSTSOFT_TIZENERROR_H

#include <tizen.h>
#include <stdexcept>

namespace Kystsoft {

class TizenError : public std::runtime_error
{
public:
	TizenError(const std::string& functionName, int errorCode)
		: std::runtime_error(functionName + ": " + get_error_message(errorCode) + " (" + std::to_string(errorCode) + ')')
	{
	}
};

} // namespace Kystsoft

#endif // KYSTSOFT_TIZENERROR_H

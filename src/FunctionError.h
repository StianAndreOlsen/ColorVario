#ifndef KYSTSOFT_FUNCTIONERROR_H
#define KYSTSOFT_FUNCTIONERROR_H

#include <stdexcept>

namespace Kystsoft {

class FunctionError : public std::runtime_error
{
public:
	FunctionError(const std::string& functionName, int errorCode)
		: std::runtime_error(functionName + " returned the error code " + std::to_string(errorCode))
	{
	}
};

} // namespace Kystsoft

#endif // KYSTSOFT_FUNCTIONERROR_H

#ifndef KYSTSOFT_DLOG_H
#define KYSTSOFT_DLOG_H

#include <sstream>
#include <string>
#include <dlog.h>

namespace Kystsoft {

class dlog : public std::ostringstream
{
public:
	dlog(log_priority logPriority, const std::string& logTag = "Kystsoft")
		: priority(logPriority), tag(logTag) {}
	~dlog() noexcept;
	static std::string priorityString(log_priority priority);
	std::string priorityString() const { return priorityString(priority); }
private:
	log_priority priority;
	std::string tag;
};

} // namespace Kystsoft

#endif // KYSTSOFT_DLOG_H

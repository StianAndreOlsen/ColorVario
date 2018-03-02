#ifndef KYSTSOFT_DLOG_H
#define KYSTSOFT_DLOG_H

#include <sstream>
#include <string>
#include <dlog.h>

namespace Kystsoft {

class dlog : public std::ostringstream
{
public:
	dlog(log_priority logPriority) : priority(logPriority) {}
	~dlog() noexcept;
	static std::string priorityString(log_priority priority);
	std::string priorityString() const { return priorityString(priority); }
	static std::string logTag() { return tag; }
	static void setLogTag(const std::string& logTag) { tag = logTag; }
private:
	log_priority priority;
	static std::string tag;
};

void setDebugLogFile(const std::string& fileName);

} // namespace Kystsoft

#endif // KYSTSOFT_DLOG_H

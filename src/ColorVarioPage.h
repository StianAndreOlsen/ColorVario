#ifndef KYSTSOFT_COLORVARIO_PAGE_H
#define KYSTSOFT_COLORVARIO_PAGE_H

#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {
namespace ColorVario {

class Page : public Dali::Toolkit::Control
{
public:
	// Note: The pages can be re-arranged just by changing the order in the below enum
	enum class Type : int
	{
		Quit,
		Altitude,
		Climb,
		Speed,
		Error
	};
	Page() {}
	Page(const Dali::Toolkit::Control& other) : Dali::Toolkit::Control(other) {}
	Page& operator=(const Dali::Toolkit::Control& rhs) { Dali::Toolkit::Control::operator=(rhs); return *this; }
	static Page New(Type type);
	static Page DownCast(BaseHandle handle) { return Dali::Toolkit::Control::DownCast(handle); }
private:
	static void createQuitPageContents(const Dali::Vector2& pageSize, Dali::Actor parent);
	static void createAltitudePageContents(const Dali::Vector2& pageSize, Dali::Actor parent);
	static void createClimbPageContents(const Dali::Vector2& pageSize, Dali::Actor parent);
	static void createSpeedPageContents(const Dali::Vector2& pageSize, Dali::Actor parent);
	static void createErrorPageContents(const Dali::Vector2& pageSize, Dali::Actor parent);
};

} // namespace ColorVario
} // namespace Kystsoft

#endif // KYSTSOFT_COLORVARIO_PAGE_H

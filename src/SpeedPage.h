#ifndef KYSTSOFT_SPEEDPAGE_H
#define KYSTSOFT_SPEEDPAGE_H

#include "Page.h"
#include "TextLabel.h"

namespace Kystsoft {

class SpeedPage : public Page
{
public:
	SpeedPage() {}
	SpeedPage(const SpeedPage& other) : Page(other) {}
	SpeedPage& operator=(const SpeedPage& rhs) { Page::operator=(rhs); return *this; }
	static SpeedPage New(const Dali::Vector2& size);
	auto altitudeLabel() const { return TextLabel::DownCast(GetChildAt(0)); }
	auto climbLabel() const { return TextLabel::DownCast(GetChildAt(1)); }
	auto speedLabel() const { return TextLabel::DownCast(GetChildAt(2)); }
private:
	SpeedPage(const Page& other) : Page(other) {}
};

} // namespace Kystsoft

#endif // KYSTSOFT_SPEEDPAGE_H

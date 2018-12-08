#ifndef KYSTSOFT_CLIMBPAGE_H
#define KYSTSOFT_CLIMBPAGE_H

#include "Page.h"
#include "TextLabel.h"

namespace Kystsoft {

class ClimbPage : public Page
{
public:
	ClimbPage() {}
	ClimbPage(const ClimbPage& other) : Page(other) {}
	ClimbPage& operator=(const ClimbPage& rhs) { Page::operator=(rhs); return *this; }
	static ClimbPage New(const Dali::Vector2& size);
	auto climbLabel() const { return TextLabel::DownCast(GetChildAt(0)); }
	auto altitudeLabel() const { return TextLabel::DownCast(GetChildAt(1)); }
private:
	ClimbPage(const Page& other) : Page(other) {}
};

} // namespace Kystsoft

#endif // KYSTSOFT_CLIMBPAGE_H

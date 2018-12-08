#ifndef KYSTSOFT_ALTITUDEPAGE_H
#define KYSTSOFT_ALTITUDEPAGE_H

#include "Page.h"
#include "TextLabel.h"

namespace Kystsoft {

class AltitudePage : public Page
{
public:
	AltitudePage() {}
	AltitudePage(const AltitudePage& other) : Page(other) {}
	AltitudePage& operator=(const AltitudePage& rhs) { Page::operator=(rhs); return *this; }
	static AltitudePage New(const Dali::Vector2& size);
	auto altitudeLabel() const { return TextLabel::DownCast(GetChildAt(0)); }
private:
	AltitudePage(const Page& other) : Page(other) {}
};

} // namespace Kystsoft

#endif // KYSTSOFT_ALTITUDEPAGE_H

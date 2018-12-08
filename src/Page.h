#ifndef KYSTSOFT_PAGE_H
#define KYSTSOFT_PAGE_H

#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class Page : public Dali::Toolkit::Control
{
public:
	Page() {}
	Page(const Page& other) : Dali::Toolkit::Control(other) {}
	Page& operator=(const Page& rhs) { Dali::Toolkit::Control::operator=(rhs); return *this; }
	static Page New(const Dali::Vector2& size);
private:
	Page(const Dali::Toolkit::Control& other) : Dali::Toolkit::Control(other) {}
};

} // namespace Kystsoft

#endif // KYSTSOFT_PAGE_H

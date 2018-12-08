#include "Page.h"

Kystsoft::Page Kystsoft::Page::New(const Dali::Vector2& size)
{
	auto page = Dali::Toolkit::Control::New();
	page.SetSize(size);
	page.SetParentOrigin(Dali::ParentOrigin::CENTER);
	page.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	page.SetPosition(0, 0);
	return page;
}

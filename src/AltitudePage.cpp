#include "AltitudePage.h"
#include "Color.h"

Kystsoft::AltitudePage Kystsoft::AltitudePage::New(const Dali::Vector2& size)
{
	auto page = Page::New(size);

	auto width = size.width;
	auto height = size.height / 4;

	auto altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	altitudeLabel.SetPosition(0, 0);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
	altitudeLabel.setPointSize(15);
	page.Add(altitudeLabel);

	return page;
}

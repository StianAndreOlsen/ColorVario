#include "ClimbPage.h"
#include "Color.h"

Kystsoft::ClimbPage Kystsoft::ClimbPage::New(const Dali::Vector2& size)
{
	auto page = Page::New(size);

	auto width = size.width;
	auto height = size.height / 4;

	auto climbLabel = TextLabel::New("Climb");
	climbLabel.SetSize(width, height);
	climbLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	climbLabel.SetPosition(0, 0);
	climbLabel.setVerticalAlignment("CENTER");
	climbLabel.setHorizontalAlignment("CENTER");
	climbLabel.setTextColor(Color::mainText());
	climbLabel.setPointSize(15);
	page.Add(climbLabel);

	auto altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	altitudeLabel.SetPosition(0, 0);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
	altitudeLabel.setPointSize(13);
	page.Add(altitudeLabel);

	return page;
}

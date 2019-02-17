#include "SpeedPage.h"
#include "Color.h"

Kystsoft::SpeedPage Kystsoft::SpeedPage::New(const Dali::Vector2& size)
{
	auto page = Page::New(size);

	auto width = size.width;
	auto height = size.height / 4;

	auto altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height * 3 / 4);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	altitudeLabel.SetPosition(0, -height / 2);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
	altitudeLabel.setPointSize(13);
	page.Add(altitudeLabel);

	auto climbLabel = TextLabel::New("Climb");
	climbLabel.SetSize(width, height);
	climbLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	climbLabel.SetPosition(0, 0);
	climbLabel.setVerticalAlignment("CENTER");
	climbLabel.setHorizontalAlignment("CENTER");
	climbLabel.setTextColor(Color::mainText());
	climbLabel.setPointSize(15);
	page.Add(climbLabel);

	auto speedLabel = TextLabel::New("Speed");
	speedLabel.SetSize(width, height * 3 / 4);
	speedLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	speedLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	speedLabel.SetPosition(0, height / 2);
	speedLabel.setVerticalAlignment("CENTER");
	speedLabel.setHorizontalAlignment("CENTER");
	speedLabel.setTextColor(Color::subText());
	speedLabel.setPointSize(13);
	page.Add(speedLabel);

	return page;
}

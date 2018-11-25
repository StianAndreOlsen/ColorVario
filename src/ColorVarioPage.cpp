#include "ColorVarioPage.h"
#include "AppFunctions.h"
#include "Color.h"
#include "PushButton.h"
#include "TextLabel.h"
#include <cmath>

Kystsoft::ColorVario::Page Kystsoft::ColorVario::Page::New(Type type)
{
	Dali::Vector2 size = Dali::Stage::GetCurrent().GetSize();

	// empty page
	Dali::Toolkit::Control page = Dali::Toolkit::Control::New();
	page.SetSize(size);

	// page contents
	switch (type)
	{
	case Type::Quit:
		createQuitPageContents(size, page);
		break;
	case Type::Altitude:
		createAltitudePageContents(size, page);
		break;
	case Type::Climb:
		createClimbPageContents(size, page);
		break;
	case Type::Speed:
		createSpeedPageContents(size, page);
		break;
	case Type::Error:
		createErrorPageContents(size, page);
		break;
	}

	return Page(page);
}

void Kystsoft::ColorVario::Page::createQuitPageContents(const Dali::Vector2& pageSize, Dali::Actor parent)
{
	float width = pageSize.height / 2;
	float height = width;
	std::string resourceDir = appSharedResourcePath();

	PushButton quitButton = PushButton::New();
	quitButton.SetSize(width, height);
	quitButton.SetParentOrigin(Dali::ParentOrigin::CENTER);
	quitButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton.SetPosition(0, 0);
	quitButton.setUnselectedImage(resourceDir + "QuitUnselected.png");
	quitButton.setSelectedImage(resourceDir + "QuitSelected.png");
	parent.Add(quitButton);
}

void Kystsoft::ColorVario::Page::createAltitudePageContents(const Dali::Vector2& pageSize, Dali::Actor parent)
{
	float width = pageSize.width;
	float height = pageSize.height / 4;

	TextLabel altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	altitudeLabel.SetPosition(0, 0);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
	altitudeLabel.setPointSize(15);
	parent.Add(altitudeLabel);
}

void Kystsoft::ColorVario::Page::createClimbPageContents(const Dali::Vector2& pageSize, Dali::Actor parent)
{
	float width = pageSize.width;
	float height = pageSize.height / 4;

	TextLabel climbLabel = TextLabel::New("Climb");
	climbLabel.SetSize(width, height);
	climbLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	climbLabel.SetPosition(0, 0);
	climbLabel.setVerticalAlignment("CENTER");
	climbLabel.setHorizontalAlignment("CENTER");
	climbLabel.setTextColor(Color::mainText());
	climbLabel.setPointSize(15);
	parent.Add(climbLabel);

	TextLabel altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	altitudeLabel.SetPosition(0, 0);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
	parent.Add(altitudeLabel);
}

void Kystsoft::ColorVario::Page::createSpeedPageContents(const Dali::Vector2& pageSize, Dali::Actor parent)
{
	float width = pageSize.width;
	float height = pageSize.height / 4;

	TextLabel altitudeLabel = TextLabel::New("Altitude");
	altitudeLabel.SetSize(width, height * 3 / 4);
	altitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudeLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	altitudeLabel.SetPosition(0, -height / 2);
	altitudeLabel.setVerticalAlignment("CENTER");
	altitudeLabel.setHorizontalAlignment("CENTER");
	altitudeLabel.setTextColor(Color::error());
	parent.Add(altitudeLabel);

	TextLabel climbLabel = TextLabel::New("Climb");
	climbLabel.SetSize(width, height);
	climbLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	climbLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	climbLabel.SetPosition(0, 0);
	climbLabel.setVerticalAlignment("CENTER");
	climbLabel.setHorizontalAlignment("CENTER");
	climbLabel.setTextColor(Color::mainText());
	climbLabel.setPointSize(15);
	parent.Add(climbLabel);

	TextLabel speedLabel = TextLabel::New("Speed");
	speedLabel.SetSize(width, height * 3 / 4);
	speedLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	speedLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	speedLabel.SetPosition(0, height / 2);
	speedLabel.setVerticalAlignment("CENTER");
	speedLabel.setHorizontalAlignment("CENTER");
	speedLabel.setTextColor(Color::subText());
	parent.Add(speedLabel);
}

void Kystsoft::ColorVario::Page::createErrorPageContents(const Dali::Vector2& pageSize, Dali::Actor parent)
{
	float width = pageSize.width / std::sqrt(2);
	float height = pageSize.height / std::sqrt(2);

	std::string about
	(
		"<font size='7' weight='bold'>"
		"<color value='" + Color::information().hexStringARGB() + "'>"
			"ColorVario 2.0.0\n"
			"\n"
		"</color>"
		"</font>"
		"<font size='7'>"
			"Developed by\n"      // hard line breaks are required
			"Kyrre Holm and\n"    // since TextLabel breaks lines
			"Stian Andre Olsen\n" // even at no-break spaces
			"\n"
			"Please visit https://www.facebook.com/ColorVariometer/"
		"</font>"
	);
//	TextLabel errorLabel = TextLabel::New(about);
	TextLabel errorLabel = TextLabel::New();
	errorLabel.SetSize(width, height);
	errorLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	errorLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	errorLabel.SetPosition(0, 0);
	errorLabel.setVerticalAlignment("CENTER");
	errorLabel.setHorizontalAlignment("CENTER");
	errorLabel.setTextColor(Color::defaultText());
	errorLabel.setPointSize(8);
	errorLabel.setMultiLine(true);
	errorLabel.enableMarkup();
//	errorLabel.enableAutoScroll();
//	errorLabel.setAutoScrollSpeed(200);
//	errorLabel.setAutoScrollLoopCount(5);
//	errorLabel.setAutoScrollGap(100);
	errorLabel.setText(about);
//	errorLabel.setText("Dette er en lang tekst som må scrolles for å vises i sin helhet!");
	parent.Add(errorLabel);
}

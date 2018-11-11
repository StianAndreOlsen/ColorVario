#include "ColorVarioUserInterface.h"
#include "AppFunctions.h"
#include <cmath>

void Kystsoft::ColorVario::UserInterface::create()
{
	// page size
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 pageSize = stage.GetSize();

	// create page strip
	pageStrip = Dali::Toolkit::Control::New();
	pageStrip.SetSize(pageCount * pageSize.width, pageSize.height);
	pageStrip.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	pageStrip.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	pageStrip.SetPosition(-int(curPage) * pageSize.width, 0);
//	pageStrip.SetBackgroundColor(Dali::Color::BLUE);
	stage.Add(pageStrip);

	// create empty pages
	for (size_t i = 0; i < pageCount; ++i)
	{
		Dali::Toolkit::Control page = Dali::Toolkit::Control::New();
		page.SetSize(pageSize);
		page.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
		page.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
		page.SetPosition(i * pageSize.width, 0);
		pages.push_back(page);
		pageStrip.Add(page);
	}

	// create page contents
	createQuitPageContents(pageSize);
	createAltitudePageContents(pageSize);
	createClimbPageContents(pageSize);
	createSpeedPageContents(pageSize);
	createErrorPageContents(pageSize);

	// create menu
	createMenu();

	// make sure the 'drivers' are active and connected correctly to the initial page
	climbAudio.start();
	altitudeRing = altitudePage();
	climbRing = climbPage();
	altitudeLabel = climbPageAltitudeLabel;
	climbLabel = climbPageClimbLabel;
	speedLabel = speedPageSpeedLabel;

	// connect stage signals
	stage.TouchSignal().Connect(this, &UserInterface::onTouch);
	stage.WheelEventSignal().Connect(this, &UserInterface::onWheelEvent);

	// for some reason the animation doesn't work at this stage
	// --> calling showPage at this stage doesn't work
	// TODO: Maybe it is possible to check if animation is ok and if not we can just position the page strip without any animations
	// TODO: If I am able to call showPage here, go back to set curPage = Page::Quit in the header, and don't start the audio above
	// TODO: I suspect something is wrong with the pages current sizes. Maybe they haven't been calculated properly.
//	showPage(Page::Climb);
}

void Kystsoft::ColorVario::UserInterface::load(const Settings& settings)
{
	// sound settings
	altitudeAudio.load(settings);
	climbAudio.load(settings);

	// color settings
	altitudeRing.load(settings);
	climbRing.load(settings);

	// label settings
	altitudeLabel.load(settings);
	climbLabel.load(settings);
	speedLabel.load(settings);
}

void Kystsoft::ColorVario::UserInterface::showPage(Page page)
{
	if (page == Page::Error && errorLabel.text().empty())
		return; // stay on the existing page
	if (page == curPage)
		return;

	// sound
	if (page == Page::Altitude)
	{
		climbAudio.stop();
		altitudeAudio.start();
	}
	else if (curPage == Page::Altitude)
	{
		altitudeAudio.stop();
		climbAudio.start();
	}

	// color and labels
	switch (page)
	{
	case Page::Quit:
		climbRing = quitPage();
		break;
	case Page::Altitude:
		// altitudeRing always paints to altitudePage
		altitudeLabel = altitudePageAltitudeLabel;
		break;
	case Page::Climb:
		climbRing = climbPage();
		climbLabel = climbPageClimbLabel;
		altitudeLabel = climbPageAltitudeLabel;
		break;
	case Page::Speed:
		climbRing = speedPage();
		altitudeLabel = speedPageAltitudeLabel;
		climbLabel = speedPageClimbLabel;
		// speedLabel always writes to speedPageSpeedLabel
		break;
	case Page::Error:
		climbRing = errorPage();
		break;
	}

	Dali::Animation animation = Dali::Animation::New(0.25f);
	Dali::Vector3 pos = pageStrip.GetCurrentPosition();
	pos.x = -int(page) * quitPage().GetCurrentSize().width;
	animation.AnimateTo(Dali::Property(pageStrip, Dali::Actor::Property::POSITION), pos);
	animation.Play();

	curPage = page;
}

void Kystsoft::ColorVario::UserInterface::showMenu(bool show)
{
	Dali::Animation animation = Dali::Animation::New(0.25f);
	Dali::Vector3 pos = menu.GetCurrentPosition();
	pos.y = show ? 0 : -menu.GetCurrentSize().height;
	animation.AnimateTo(Dali::Property(menu, Dali::Actor::Property::POSITION), pos);
	animation.Play();
}

void Kystsoft::ColorVario::UserInterface::setAltitudeSamplingInterval(double interval)
{
	altitudeAudio.setSamplingInterval(interval);
	altitudeRing.setSamplingInterval(interval);
	altitudeLabel.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setClimbSamplingInterval(double interval)
{
	climbAudio.setSamplingInterval(interval);
	climbRing.setSamplingInterval(interval);
	climbLabel.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setSpeedSamplingInterval(double interval)
{
	speedLabel.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setAltitudeTextColor(const Dali::Vector4& color)
{
	altitudePageAltitudeLabel.setTextColor(color);
	climbPageAltitudeLabel.setTextColor(color);
	speedPageAltitudeLabel.setTextColor(color);
}

void Kystsoft::ColorVario::UserInterface::setClimbTextColor(const Dali::Vector4& color)
{
	climbPageClimbLabel.setTextColor(color);
	speedPageClimbLabel.setTextColor(color);
}

void Kystsoft::ColorVario::UserInterface::setSpeedTextColor(const Dali::Vector4& color)
{
	speedPageSpeedLabel.setTextColor(color);
}

void Kystsoft::ColorVario::UserInterface::setAltitude(double altitude)
{
	// hide menu
	if (menu.GetCurrentPosition().y == 0 && std::difftime(std::time(nullptr), lastMenuTouch) > 15)
		showMenu(false);

	// synchronize mute audio button
	if (curPage == Page::Altitude)
		muteAudioButton.setChecked(!altitudeAudio.isStarted());
	else
		muteAudioButton.setChecked(!climbAudio.isStarted());

	altitudeAudio.setAltitude(altitude);
	altitudeRing.setAltitude(altitude);
	altitudeLabel.setAltitude(altitude);
}

void Kystsoft::ColorVario::UserInterface::setClimb(double climb)
{
	climbAudio.setClimb(climb);
	climbRing.setClimb(climb);
	climbLabel.setClimb(climb);
}

void Kystsoft::ColorVario::UserInterface::setSpeed(double speed)
{
	speedLabel.setSpeed(speed);
}

void Kystsoft::ColorVario::UserInterface::setErrorMessage(const std::string& message)
{
	// TODO: Add the possibility of several error messages instead of overwriting the previous
	// TODO: Don't add an error already added, and then don't show the error page again!
	errorLabel.setText(message);
	showPage(Page::Error);
}

void Kystsoft::ColorVario::UserInterface::setAudioMuted(bool muted)
{
	muteAudioButton.setChecked(muted);
	onMuteAudioButtonClicked(muteAudioButton);
}

void Kystsoft::ColorVario::UserInterface::setDisplayLocked(bool locked)
{
	lockDisplayButton.setChecked(locked);
	onLockDisplayButtonClicked(lockDisplayButton);
}

void Kystsoft::ColorVario::UserInterface::setBluetoothEnabled(bool enabled)
{
	enableBluetoothButton.setChecked(enabled);
	onEnableBluetoothButtonClicked(enableBluetoothButton);
}

void Kystsoft::ColorVario::UserInterface::setBluetoothConnected(bool connected)
{
	std::string file = connected ? "BluetoothConnected.png" : "BluetoothEnabled.png";
	enableBluetoothButton.setSelectedImage(appSharedResourcePath() + file);
}

void Kystsoft::ColorVario::UserInterface::createQuitPageContents(const Dali::Vector2& pageSize)
{
	float height = pageSize.height / 2;
	float width = height;
	std::string resourceDir = appSharedResourcePath();

	quitButton = PushButton::New();
	quitButton.SetSize(width, height);
	quitButton.SetParentOrigin(Dali::ParentOrigin::CENTER);
	quitButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton.SetPosition(0, 0);
	quitButton.setUnselectedImage(resourceDir + "QuitUnselected.png");
	quitButton.setSelectedImage(resourceDir + "QuitSelected.png");
	quitButton.ClickedSignal().Connect(this, &UserInterface::onQuitButtonClicked);
	quitPage().Add(quitButton);
}

void Kystsoft::ColorVario::UserInterface::createAltitudePageContents(const Dali::Vector2& pageSize)
{
	float width = pageSize.width;
	float height = pageSize.height / 4;

	altitudePageAltitudeLabel = TextLabel::New("Altitude");
	altitudePageAltitudeLabel.SetSize(width, height);
	altitudePageAltitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	altitudePageAltitudeLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	altitudePageAltitudeLabel.SetPosition(0, 0);
	altitudePageAltitudeLabel.setVerticalAlignment("CENTER");
	altitudePageAltitudeLabel.setHorizontalAlignment("CENTER");
	altitudePageAltitudeLabel.setTextColor(Dali::Color::RED);
	altitudePageAltitudeLabel.setPointSize(15);
	altitudePage().Add(altitudePageAltitudeLabel);
}

void Kystsoft::ColorVario::UserInterface::createClimbPageContents(const Dali::Vector2& pageSize)
{
	float width = pageSize.width;
	float height = pageSize.height / 4;

	climbPageClimbLabel = TextLabel::New("Climb");
	climbPageClimbLabel.SetSize(width, height);
	climbPageClimbLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	climbPageClimbLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	climbPageClimbLabel.SetPosition(0, 0);
	climbPageClimbLabel.setVerticalAlignment("CENTER");
	climbPageClimbLabel.setHorizontalAlignment("CENTER");
	climbPageClimbLabel.setTextColor(Dali::Color::WHITE);
	climbPageClimbLabel.setPointSize(15);
	climbPage().Add(climbPageClimbLabel);

	climbPageAltitudeLabel = TextLabel::New("Altitude");
	climbPageAltitudeLabel.SetSize(width, height);
	climbPageAltitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	climbPageAltitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	climbPageAltitudeLabel.SetPosition(0, 0);
	climbPageAltitudeLabel.setVerticalAlignment("CENTER");
	climbPageAltitudeLabel.setHorizontalAlignment("CENTER");
	climbPageAltitudeLabel.setTextColor(Dali::Color::RED);
	climbPage().Add(climbPageAltitudeLabel);
}

void Kystsoft::ColorVario::UserInterface::createSpeedPageContents(const Dali::Vector2& pageSize)
{
	float width = pageSize.width;
	float height = pageSize.height / 4;

	speedPageAltitudeLabel = TextLabel::New("Altitude");
	speedPageAltitudeLabel.SetSize(width, height * 3 / 4);
	speedPageAltitudeLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	speedPageAltitudeLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	speedPageAltitudeLabel.SetPosition(0, -height / 2);
	speedPageAltitudeLabel.setVerticalAlignment("CENTER");
	speedPageAltitudeLabel.setHorizontalAlignment("CENTER");
	speedPageAltitudeLabel.setTextColor(Dali::Color::RED);
	speedPage().Add(speedPageAltitudeLabel);

	speedPageClimbLabel = TextLabel::New("Climb");
	speedPageClimbLabel.SetSize(width, height);
	speedPageClimbLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	speedPageClimbLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	speedPageClimbLabel.SetPosition(0, 0);
	speedPageClimbLabel.setVerticalAlignment("CENTER");
	speedPageClimbLabel.setHorizontalAlignment("CENTER");
	speedPageClimbLabel.setTextColor(Dali::Color::WHITE);
	speedPageClimbLabel.setPointSize(15);
	speedPage().Add(speedPageClimbLabel);

	speedPageSpeedLabel = TextLabel::New("Speed");
	speedPageSpeedLabel.SetSize(width, height * 3 / 4);
	speedPageSpeedLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	speedPageSpeedLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	speedPageSpeedLabel.SetPosition(0, height / 2);
	speedPageSpeedLabel.setVerticalAlignment("CENTER");
	speedPageSpeedLabel.setHorizontalAlignment("CENTER");
	speedPageSpeedLabel.setTextColor(Dali::Color::WHITE);
	speedPage().Add(speedPageSpeedLabel);
}

void Kystsoft::ColorVario::UserInterface::createErrorPageContents(const Dali::Vector2& pageSize)
{
	float width = pageSize.width / std::sqrt(2);
	float height = pageSize.height / std::sqrt(2);

	errorLabel = TextLabel::New("Kyrre og Stian er to skikkelig kule fyrer!");
//	errorLabel = TextLabel::New();
	errorLabel.SetSize(width, height);
	errorLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	errorLabel.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	errorLabel.SetPosition(0, 0);
	errorLabel.setVerticalAlignment("CENTER");
	errorLabel.setHorizontalAlignment("CENTER");
	errorLabel.setTextColor(Dali::Color::WHITE);
	errorLabel.setMultiLine(true);
	errorPage().Add(errorLabel);
}

void Kystsoft::ColorVario::UserInterface::createMenu()
{
	// menu size and position
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 stageSize = stage.GetSize();
	float height = stageSize.height / 3;
	float width = stageSize.width;
	float radius = stageSize.height;

	// make sure the menu is drawn on top
	Dali::Layer menuLayer = Dali::Layer::New();
	menuLayer.SetSize(width, height);
	menuLayer.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	menuLayer.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	menuLayer.SetPosition(0, 0);
	menuLayer.RaiseToTop();
	stage.Add(menuLayer);

	// create menu
	menu = Dali::Toolkit::Control::New();
	menu.SetSize(width, height);
	menu.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	menu.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	menu.SetPosition(0, 0);
	Dali::Property::Map map;
	map[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
	map[Dali::Toolkit::GradientVisual::Property::UNITS] = Dali::Toolkit::GradientVisual::Units::USER_SPACE;
	map[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, -radius + height / 2);
	map[Dali::Toolkit::GradientVisual::Property::RADIUS] = radius;
	Dali::Property::Array stopOffsets;
	stopOffsets.PushBack(0.99f);
	stopOffsets.PushBack(0.99f);
	map[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
	Dali::Property::Array stopColors;
	stopColors.PushBack(Color(0.0f, 0.207f, 0.29f, 0.9f)); // standard button color, https://developer.tizen.org/design/wearable/visual-design/colors
//	stopColors.PushBack(Color(0.0f, 0.18f, 0.255f, 1.0f)); // standard button color merged with a black background
	stopColors.PushBack(Dali::Color::TRANSPARENT);
	map[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	menu.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, map);
	menuLayer.Add(menu);
	menu.TouchSignal().Connect(this, &UserInterface::onMenuTouched);
	lastMenuTouch = std::time(nullptr);

	// button size and position
	height = stageSize.height / 8;
	width = height;
	float dy = height / 2 + height / 3;
	float r = radius - dy;
	float dx2 = width + width * 3 / 4;
	float dy2 = radius - std::sqrt(r*r - dx2*dx2);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create lock display button
	lockDisplayButton = PushButton::New();
	lockDisplayButton.SetSize(width, height);
	lockDisplayButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	lockDisplayButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	lockDisplayButton.SetPosition(-dx2, -dy2);
	lockDisplayButton.setUnselectedImage(resourceDir + "DisplayUnlocked.png");
	lockDisplayButton.setSelectedImage(resourceDir + "DisplayLocked.png");
	lockDisplayButton.setCheckable(true);
	lockDisplayButton.setChecked(false);
	lockDisplayButton.ClickedSignal().Connect(this, &UserInterface::onLockDisplayButtonClicked);
	menu.Add(lockDisplayButton);

	// create mute audio button
	muteAudioButton = PushButton::New();
	muteAudioButton.SetSize(width, height);
	muteAudioButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	muteAudioButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	muteAudioButton.SetPosition(0, -dy);
	muteAudioButton.setUnselectedImage(resourceDir + "AudioUnmuted.png");
	muteAudioButton.setSelectedImage(resourceDir + "AudioMuted.png");
	muteAudioButton.setCheckable(true);
	muteAudioButton.setChecked(true);
	muteAudioButton.ClickedSignal().Connect(this, &UserInterface::onMuteAudioButtonClicked);
	menu.Add(muteAudioButton);

	// create enable bluetooth button
	enableBluetoothButton = PushButton::New();
	enableBluetoothButton.SetSize(width, height);
	enableBluetoothButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	enableBluetoothButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	enableBluetoothButton.SetPosition(dx2, -dy2);
	enableBluetoothButton.setUnselectedImage(resourceDir + "BluetoothDisabled.png");
	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothEnabled.png");
	enableBluetoothButton.setCheckable(true);
	enableBluetoothButton.setChecked(false);
	enableBluetoothButton.ClickedSignal().Connect(this, &UserInterface::onEnableBluetoothButtonClicked);
	menu.Add(enableBluetoothButton);

	// icon size and position
	height = height * 2 / 3;
	width = height;
	dy = height / 4;

	// create location icon
	locationIcon = Dali::Toolkit::ImageView::New(resourceDir + "Location.png");
	locationIcon.SetSize(width, height);
	locationIcon.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	locationIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	locationIcon.SetPosition(0, dy);
	menu.Add(locationIcon);
}

void Kystsoft::ColorVario::UserInterface::onTouch(const Dali::TouchData& touch)
{
	if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
	{
		float y = touch.GetScreenPosition(0).y;
		float height = Dali::Stage::GetCurrent().GetSize().height;
		showMenu(y < height / 4);
		lastMenuTouch = std::time(nullptr);
	}
}

void Kystsoft::ColorVario::UserInterface::onWheelEvent(const Dali::WheelEvent& event)
{
	if (event.z > 0)
		showNextPage();
	else if (event.z < 0)
		showPreviousPage();
}

bool Kystsoft::ColorVario::UserInterface::onMenuTouched(Dali::Actor /*actor*/, const Dali::TouchData& /*touch*/)
{
	lastMenuTouch = std::time(nullptr);
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onQuitButtonClicked(Dali::Toolkit::Button /*button*/)
{
	quitSignl.emit();
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onMuteAudioButtonClicked(Dali::Toolkit::Button /*button*/)
{
	if (muteAudioButton.isChecked())
		ValueAudio::mute();
	else
	{
		ValueAudio::unmute();
		if (curPage == Page::Altitude)
			altitudeAudio.start();
		else
			climbAudio.start();
	}
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onLockDisplayButtonClicked(Dali::Toolkit::Button /*button*/)
{
	lockDisplaySignl.emit(lockDisplayButton.isChecked());
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onEnableBluetoothButtonClicked(Dali::Toolkit::Button /*button*/)
{
	enableBluetoothSignl.emit(enableBluetoothButton.isChecked());
	return true;
}

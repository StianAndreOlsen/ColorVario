#include "ColorVarioUserInterface.h"
#include "AppFunctions.h"
#include <cmath>

void Kystsoft::ColorVario::UserInterface::create()
{
	// stage background
	Dali::Stage stage = Dali::Stage::GetCurrent();
	stage.SetBackgroundColor(Color::window());

	// page size and count
	Dali::Vector2 pageSize = stage.GetSize();
	constexpr size_t pageCount = 5; // TODO: Find a better solution

	// create page strip
	pageStrip = Dali::Toolkit::Control::New();
	pageStrip.SetSize(pageCount * pageSize.width, pageSize.height);
	pageStrip.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	pageStrip.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	pageStrip.SetPosition(-curPageIndex * pageSize.width, 0);
	stage.Add(pageStrip);

	// create pages
	for (size_t i = 0; i < pageCount; ++i)
	{
		Page page = Page::New(Page::Type(i));
		page.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
		page.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
		page.SetPosition(i * pageSize.width, 0);
		pages.push_back(page);
		pageStrip.Add(page);
	}

	// connect page signals
	quitButton().ClickedSignal().Connect(this, &UserInterface::onQuitButtonClicked);

	// create menu
	createMenu();

	// make sure the 'drivers' are active and connected correctly to the initial page
	climbAudio.start();
	altitudeRing = altitudePage();
	climbRing = climbPage();
	altitudeLabel = climbPageAltitudeLabel();
	climbLabel = climbPageClimbLabel();
	speedLabel = speedPageSpeedLabel();

	// connect stage signals
	stage.TouchSignal().Connect(this, &UserInterface::onTouch);
	stage.WheelEventSignal().Connect(this, &UserInterface::onWheelEvent);

	// for some reason the animation doesn't work at this stage
	// --> calling showPage at this stage doesn't work
	// TODO: Maybe it is possible to check if animation is ok and if not we can just position the page strip without any animations
	// TODO: If I am able to call showPage here, go back to set curPage = Page::Quit in the header, and don't start the audio above
	// TODO: I suspect something is wrong with the pages current sizes. Maybe they haven't been calculated properly.
//	showPage(Page::Type::Climb);
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

void Kystsoft::ColorVario::UserInterface::showPage(int pageIndex)
{
	pageIndex = std::clamp(pageIndex, 0, int(pages.size()) - 1);
	Page::Type pageType = Page::Type(pageIndex);
	if (pageType == Page::Type::Error && errorLabel().text().empty())
		return; // stay on the existing page
	if (pageIndex == curPageIndex)
		return;

	// sound
	if (pageType == Page::Type::Altitude)
	{
		climbAudio.stop();
		altitudeAudio.start();
	}
	else if (curPageIndex == int(Page::Type::Altitude))
	{
		altitudeAudio.stop();
		climbAudio.start();
	}

	// color and labels
	switch (pageType)
	{
	case Page::Type::Quit:
		climbRing = quitPage();
		break;
	case Page::Type::Altitude:
		// altitudeRing always paints to altitudePage
		altitudeLabel = altitudePageAltitudeLabel();
		break;
	case Page::Type::Climb:
		climbRing = climbPage();
		climbLabel = climbPageClimbLabel();
		altitudeLabel = climbPageAltitudeLabel();
		break;
	case Page::Type::Speed:
		climbRing = speedPage();
		altitudeLabel = speedPageAltitudeLabel();
		climbLabel = speedPageClimbLabel();
		// speedLabel always writes to speedPageSpeedLabel
		break;
	case Page::Type::Error:
		climbRing = errorPage();
		break;
	}

	Dali::Vector3 pos = pageStrip.GetCurrentPosition();
	float targetX = -pageIndex * quitPage().GetTargetSize().width;
	if (pos.x != targetX)
	{
		pos.x = targetX;
		Dali::Animation animation = Dali::Animation::New(0.25f);
		animation.AnimateTo(Dali::Property(pageStrip, Dali::Actor::Property::POSITION), pos);
		animation.Play();
	}

	curPageIndex = pageIndex;
}

void Kystsoft::ColorVario::UserInterface::setMenuVisible(bool visible)
{
	Dali::Vector3 pos = menu.GetCurrentPosition();
	float targetY = visible ? 0 : -menu.GetTargetSize().height;
	if (pos.y != targetY)
	{
		pos.y = targetY;
		Dali::Animation animation = Dali::Animation::New(0.25f);
		animation.AnimateTo(Dali::Property(menu, Dali::Actor::Property::POSITION), pos);
		animation.Play();
	}
	lastMenuTouch = std::time(nullptr);
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

void Kystsoft::ColorVario::UserInterface::setAltitudeAccuracy(double accuracy)
{
	Color color = Color::error();
	if (accuracy < 10)
		color = Color::subText();
	else if (accuracy < 100)
		color = Color::warning();
	altitudePageAltitudeLabel().setTextColor(color);
	climbPageAltitudeLabel().setTextColor(color);
	speedPageAltitudeLabel().setTextColor(color);
}

void Kystsoft::ColorVario::UserInterface::setAltitude(double altitude)
{
	// hide menu
	if (isMenuVisible() && std::difftime(std::time(nullptr), lastMenuTouch) > 15)
		hideMenu();

	// synchronize mute audio button
	if (curPageIndex == int(Page::Type::Altitude))
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
	errorLabel().setText(message);
	showPage(Page::Type::Error);
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

void Kystsoft::ColorVario::UserInterface::createMenu()
{
	// menu size and position
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 stageSize = stage.GetSize();
	float width = stageSize.width;
	float height = stageSize.height / 2;
	float R = stageSize.height; // menu radius

	// make sure the menu is drawn on top
	Dali::Layer menuLayer = Dali::Layer::New();
	menuLayer.SetSize(width, height);
	menuLayer.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	menuLayer.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	menuLayer.SetPosition(0, 0);
	menuLayer.RaiseToTop();
	stage.Add(menuLayer);

	// menu background
	Dali::Property::Map menuMap;
	{
		menuMap[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
		menuMap[Dali::Toolkit::GradientVisual::Property::UNITS] = Dali::Toolkit::GradientVisual::Units::USER_SPACE;
		menuMap[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, -R + height / 2);
		menuMap[Dali::Toolkit::GradientVisual::Property::RADIUS] = R;
		Dali::Property::Array stopOffsets;
		stopOffsets.PushBack(0.99f);
		stopOffsets.PushBack(0.99f);
		menuMap[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
		Dali::Property::Array stopColors;
		stopColors.PushBack(Color::button());
		stopColors.PushBack(Dali::Color::TRANSPARENT);
		menuMap[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	}

	// create menu
	menu = Dali::Toolkit::Control::New();
	menu.SetSize(width, height);
	menu.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	menu.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	menu.SetPosition(0, 0);
	menu.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, menuMap);
	menuLayer.Add(menu);
	menu.TouchSignal().Connect(this, &UserInterface::onMenuTouched);
	lastMenuTouch = std::time(nullptr);

	// create application label
	TextLabel appLabel = TextLabel::New(appName());
//	appLabel.SetSize(width, height);
	appLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	appLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	appLabel.SetPosition(0, 0);
	appLabel.setVerticalAlignment("CENTER");
	appLabel.setHorizontalAlignment("CENTER");
	appLabel.setTextColor(Color::subText());
	appLabel.setPointSize(8);
	menu.Add(appLabel);

	// buttons size and position
	width = stageSize.height / 6;
	height = width;
	float r = height / 2; // button radius
	float margin = r / 4; // margin towards menu radius
	float spacing = r / 5; // spacing between buttons
	float Rc = R - margin - r; // button center point radius
	float alpha = 2 * std::asin((r + spacing / 2) / Rc); // angle between buttons

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// button background
	Dali::Property::Map buttonMap;
	{
		buttonMap[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
		buttonMap[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, 0);
		buttonMap[Dali::Toolkit::GradientVisual::Property::RADIUS] = 0.5f;
		Dali::Property::Array stopOffsets;
		stopOffsets.PushBack(0.99f);
		stopOffsets.PushBack(0.99f);
		buttonMap[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
		Dali::Property::Array stopColors;
		stopColors.PushBack(Color::button().shaded(0.5f));
		stopColors.PushBack(Dali::Color::TRANSPARENT);
		buttonMap[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	}

	// create enable bluetooth button
	float x = -Rc * std::sin(2 * alpha);
	float y =  Rc * std::cos(2 * alpha) - R;
	enableBluetoothButton = PushButton::New();
	enableBluetoothButton.SetSize(width, height);
	enableBluetoothButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	enableBluetoothButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	enableBluetoothButton.SetPosition(x, y);
	enableBluetoothButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, buttonMap);
	enableBluetoothButton.setUnselectedImage(resourceDir + "BluetoothDisabled.png");
	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothEnabled.png");
//	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothConnected.png");
	enableBluetoothButton.setCheckable(true);
	enableBluetoothButton.setChecked(false);
	enableBluetoothButton.ClickedSignal().Connect(this, &UserInterface::onEnableBluetoothButtonClicked);
	menu.Add(enableBluetoothButton);

	// create lock display button
	x = -Rc * std::sin(alpha);
	y =  Rc * std::cos(alpha) - R;
	lockDisplayButton = PushButton::New();
	lockDisplayButton.SetSize(width, height);
	lockDisplayButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	lockDisplayButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	lockDisplayButton.SetPosition(x, y);
	lockDisplayButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, buttonMap);
	lockDisplayButton.setUnselectedImage(resourceDir + "DisplayUnlocked.png");
	lockDisplayButton.setSelectedImage(resourceDir + "DisplayLocked.png");
	lockDisplayButton.setCheckable(true);
	lockDisplayButton.setChecked(false);
	lockDisplayButton.ClickedSignal().Connect(this, &UserInterface::onLockDisplayButtonClicked);
	menu.Add(lockDisplayButton);

	// create mute audio button
	x = 0;
	y = Rc - R;
	muteAudioButton = PushButton::New();
	muteAudioButton.SetSize(width, height);
	muteAudioButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	muteAudioButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	muteAudioButton.SetPosition(x, y);
	muteAudioButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, buttonMap);
	muteAudioButton.setUnselectedImage(resourceDir + "AudioUnmuted.png");
	muteAudioButton.setSelectedImage(resourceDir + "AudioMuted.png");
	muteAudioButton.setCheckable(true);
	muteAudioButton.setChecked(true);
	muteAudioButton.ClickedSignal().Connect(this, &UserInterface::onMuteAudioButtonClicked);
	menu.Add(muteAudioButton);

	// create information button
	x = Rc * std::sin(alpha);
	y = Rc * std::cos(alpha) - R;
	informationButton = PushButton::New();
	informationButton.SetSize(width, height);
	informationButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	informationButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	informationButton.SetPosition(x, y);
	informationButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, buttonMap);
	informationButton.setUnselectedImage(resourceDir + "Information.png");
	informationButton.setSelectedImage(resourceDir + "InformationPressed.png");
//	informationButton.setUnselectedImage(resourceDir + "Warning.png");
//	informationButton.setSelectedImage(resourceDir + "WarningPressed.png");
//	informationButton.setUnselectedImage(resourceDir + "Error.png");
//	informationButton.setSelectedImage(resourceDir + "ErrorPressed.png");
//	informationButton.ClickedSignal().Connect(this, &UserInterface::onInformationButtonClicked);
	menu.Add(informationButton);

	// quit button background
	{
		Dali::Property::Array stopColors;
		stopColors.PushBack(Color::error().shaded(0.9f));
		stopColors.PushBack(Dali::Color::TRANSPARENT);
		buttonMap[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	}

	// create quit button
	x = Rc * std::sin(2 * alpha);
	y = Rc * std::cos(2 * alpha) - R;
	quitButton2 = PushButton::New();
	quitButton2.SetSize(width, height);
	quitButton2.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	quitButton2.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton2.SetPosition(x, y);
	quitButton2.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, buttonMap);
	quitButton2.setUnselectedImage(resourceDir + "Quit.png");
	quitButton2.setSelectedImage(resourceDir + "QuitPressed.png");
	quitButton2.ClickedSignal().Connect(this, &UserInterface::onQuitButtonClicked);
	menu.Add(quitButton2);

	// icon size and position
	height = height * 2 / 3;
	width = height;
	margin = height / 8;

	// create location icon
	locationIcon = Dali::Toolkit::ImageView::New(resourceDir + "Location.png");
	locationIcon.SetSize(width, height);
	locationIcon.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	locationIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	locationIcon.SetPosition(0, margin);
	menu.Add(locationIcon);
}

void Kystsoft::ColorVario::UserInterface::onTouch(const Dali::TouchData& touch)
{
	if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
	{
		float y = touch.GetScreenPosition(0).y;
		float height = Dali::Stage::GetCurrent().GetSize().height;
		if (y < height / 4)
			showMenu();
		else if (y > height * 3 / 4)
			hideMenu();
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
		if (curPageIndex == int(Page::Type::Altitude))
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

#include "ColorVarioUserInterface.h"
#include "AppFunctions.h"
#include <cmath>

void Kystsoft::ColorVario::UserInterface::create()
{
	createPageLayer();
	createMenuLayer();
	createDialogLayer();

	// gestures for showing and hiding the menu
	tapDetector = Dali::TapGestureDetector::New();
	tapDetector.Attach(pageView);
	tapDetector.DetectedSignal().Connect(this, &UserInterface::onTapGesture);
	panDetector = Dali::PanGestureDetector::New();
	panDetector.AddDirection(Dali::PanGestureDetector::DIRECTION_VERTICAL);
	panDetector.Attach(pageView);
	panDetector.Attach(menu);
	panDetector.DetectedSignal().Connect(this, &UserInterface::onPanGesture);

	// make sure the 'drivers' are active and connected correctly to the initial page
	climbAudio.start();
	altitudeRing = altitudePage;
	climbRing = climbPage;
	altitudeLabel = climbPage.altitudeLabel();
	climbLabel = climbPage.climbLabel();
	speedLabel = speedPage.speedLabel();

	// connect stage signals
	auto stage = Dali::Stage::GetCurrent();
	stage.WheelEventSignal().Connect(this, &UserInterface::onWheelEvent);
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

/*
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
	case Page::Type::Altitude:
		// altitudeRing always paints to altitudePage
		altitudeLabel = altitudePage.altitudeLabel();
		break;
	case Page::Type::Climb:
		climbRing = climbPage;
		climbLabel = climbPage.climbLabel();
		altitudeLabel = climbPage.altitudeLabel();
		break;
	case Page::Type::Speed:
		climbRing = speedPage;
		altitudeLabel = speedPage.altitudeLabel();
		climbLabel = speedPage.climbLabel();
		// speedLabel always writes to speedPage.speedLabel()
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
*/

void Kystsoft::ColorVario::UserInterface::setMenuVisible(bool visible)
{
	auto pos = menu.GetCurrentPosition();
	auto targetY = visible ? 0.0f : -menu.GetTargetSize().height;
	if (pos.y != targetY)
	{
		pos.y = targetY;
		Dali::Animation animation = Dali::Animation::New(0.25f);
		animation.AnimateTo(Dali::Property(menu, Dali::Actor::Property::POSITION), pos);
		animation.Play();
	}
	lastMenuTouch = std::time(nullptr);
}

void Kystsoft::ColorVario::UserInterface::showOrHideMenu(float vy)
{
	if (vy == 0)
	{
		auto posY = menu.GetCurrentPosition().y;
		auto height = menu.GetTargetSize().height;
		auto visibleHeight = height + posY;
		setMenuVisible(visibleHeight >= height / 2);
	}
	else
		setMenuVisible(vy > 0);
}

void Kystsoft::ColorVario::UserInterface::translateMenu(float dy)
{
	auto posY = menu.GetCurrentPosition().y;
	auto minY = -menu.GetTargetSize().height;
	auto maxY = 0.0f;
	auto targetY = std::clamp(posY + dy, minY, maxY);
	if (posY != targetY)
		menu.TranslateBy(Dali::Vector3(0, targetY - posY, 0));
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
	altitudePage.altitudeLabel().setTextColor(color);
	climbPage.altitudeLabel().setTextColor(color);
	speedPage.altitudeLabel().setTextColor(color);
}

void Kystsoft::ColorVario::UserInterface::setAltitude(double altitude)
{
	// hide menu
	if (isMenuVisible() && std::difftime(std::time(nullptr), lastMenuTouch) > 15)
		hideMenu();

	// synchronize mute audio button
	if (pageView.currentPageIndex() == 0)
		menu.muteAudioButton().setChecked(!altitudeAudio.isStarted());
	else
		menu.muteAudioButton().setChecked(!climbAudio.isStarted());

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
//	errorLabel().setText(message);
//	showPage(Page::Type::Error);
}

void Kystsoft::ColorVario::UserInterface::setAudioMuted(bool muted)
{
	menu.muteAudioButton().setChecked(muted);
	onMuteAudioButtonClicked(menu.muteAudioButton());
}

void Kystsoft::ColorVario::UserInterface::setDisplayLocked(bool locked)
{
	menu.lockDisplayButton().setChecked(locked);
	onLockDisplayButtonClicked(menu.lockDisplayButton());
}

void Kystsoft::ColorVario::UserInterface::setBluetoothEnabled(bool enabled)
{
	menu.enableBluetoothButton().setChecked(enabled);
	onEnableBluetoothButtonClicked(menu.enableBluetoothButton());
}

void Kystsoft::ColorVario::UserInterface::setBluetoothConnected(bool connected)
{
	std::string file = connected ? "BluetoothConnected.png" : "BluetoothEnabled.png";
	menu.enableBluetoothButton().setSelectedImage(appSharedResourcePath() + file);
}

void Kystsoft::ColorVario::UserInterface::createPageLayer()
{
	auto stage = Dali::Stage::GetCurrent();
	auto size = stage.GetSize();

	auto layer = Dali::Layer::New();
	layer.SetSize(size);
	layer.SetParentOrigin(Dali::ParentOrigin::CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	layer.SetPosition(0, 0);
	layer.RaiseToTop();
	stage.Add(layer);

	pageView.create(size);
	pageView.currentPageChangedSignal().connect(this, &UserInterface::onCurrentPageChanged);
	layer.Add(pageView);

	altitudePage = AltitudePage::New(size);
	climbPage = ClimbPage::New(size);
	speedPage = SpeedPage::New(size);
	altitudePage.SetPosition(0 * size.width, 0);
	climbPage.SetPosition(1 * size.width, 0);
	speedPage.SetPosition(2 * size.width, 0);
	pageView.addPage(altitudePage);
	pageView.addPage(climbPage);
	pageView.addPage(speedPage);

	pageView.showPage(1);
}

void Kystsoft::ColorVario::UserInterface::createMenuLayer()
{
	auto stage = Dali::Stage::GetCurrent();
	auto size = stage.GetSize();
	size.height /= 2;

	auto layer = Dali::Layer::New();
	layer.SetSize(size);
	layer.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	layer.SetPosition(0, 0);
	layer.RaiseToTop();
	stage.Add(layer);

	menu = Menu::New();
	layer.Add(menu);

	menu.TouchSignal().Connect(this, &UserInterface::onMenuTouched);
	lastMenuTouch = std::time(nullptr);

	menu.enableBluetoothButton().ClickedSignal().Connect(this, &UserInterface::onEnableBluetoothButtonClicked);
	menu.lockDisplayButton().ClickedSignal().Connect(this, &UserInterface::onLockDisplayButtonClicked);
	menu.muteAudioButton().ClickedSignal().Connect(this, &UserInterface::onMuteAudioButtonClicked);
	menu.informationButton().ClickedSignal().Connect(this, &UserInterface::onInformationButtonClicked);
	menu.quitButton().ClickedSignal().Connect(this, &UserInterface::onQuitButtonClicked);
}

void Kystsoft::ColorVario::UserInterface::createDialogLayer()
{
	auto stage = Dali::Stage::GetCurrent();
	auto size = stage.GetSize();

	auto layer = Dali::Layer::New();
	layer.SetSize(size);
	layer.SetParentOrigin(Dali::ParentOrigin::CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	layer.SetPosition(0, 0);
	layer.RaiseToTop();
	stage.Add(layer);

	messageDialog.create(size);
	layer.Add(messageDialog);

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
			"Please visit facebook.com/ColorVariometer"
		"</font>"
	);
	messageDialog.addMessage(Message(Message::Type::Information, about));
}

void Kystsoft::ColorVario::UserInterface::onTapGesture(Dali::Actor /*actor*/, const Dali::TapGesture& gesture)
{
	auto y = gesture.screenPoint.y;
	auto height = Dali::Stage::GetCurrent().GetSize().height;
	if (y < height / 4)
		showMenu();
	else if (y > height * 9 / 16)
		hideMenu();
}

void Kystsoft::ColorVario::UserInterface::onPanGesture(Dali::Actor /*actor*/, const Dali::PanGesture& gesture)
{
	switch (gesture.state)
	{
	case Dali::Gesture::Started:
	case Dali::Gesture::Continuing:
		translateMenu(gesture.displacement.y);
		break;
	case Dali::Gesture::Finished:
		showOrHideMenu(gesture.velocity.y);
		break;
	default:
		hideMenu();
		break;
	}
}

void Kystsoft::ColorVario::UserInterface::onWheelEvent(const Dali::WheelEvent& event)
{
	if (messageDialog.isVisible())
		messageDialog.onWheelEvent(event);
	else
		pageView.onWheelEvent(event);
}

void Kystsoft::ColorVario::UserInterface::onCurrentPageChanged(int newPageIndex)
{
	// sound
	if (newPageIndex == 0)
	{
		climbAudio.stop();
		altitudeAudio.start();
	}
	else if (!climbAudio.isStarted())
	{
		altitudeAudio.stop();
		climbAudio.start();
	}

	// color and labels
	switch (newPageIndex)
	{
	case 0: // altitude
		// altitudeRing always paints to altitudePage
		altitudeLabel = altitudePage.altitudeLabel();
		break;
	case 1: // climb
		climbRing = climbPage;
		climbLabel = climbPage.climbLabel();
		altitudeLabel = climbPage.altitudeLabel();
		break;
	case 2: // speed
		climbRing = speedPage;
		altitudeLabel = speedPage.altitudeLabel();
		climbLabel = speedPage.climbLabel();
		// speedLabel always writes to speedPage.speedLabel()
		break;
	}
}

bool Kystsoft::ColorVario::UserInterface::onMenuTouched(Dali::Actor /*actor*/, const Dali::TouchData& /*touch*/)
{
	lastMenuTouch = std::time(nullptr);
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onEnableBluetoothButtonClicked(Dali::Toolkit::Button /*button*/)
{
	enableBluetoothSignl.emit(menu.enableBluetoothButton().isChecked());
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onLockDisplayButtonClicked(Dali::Toolkit::Button /*button*/)
{
	lockDisplaySignl.emit(menu.lockDisplayButton().isChecked());
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onMuteAudioButtonClicked(Dali::Toolkit::Button /*button*/)
{
	if (menu.muteAudioButton().isChecked())
		ValueAudio::mute();
	else
	{
		ValueAudio::unmute();
		if (pageView.currentPageIndex() == 0)
			altitudeAudio.start();
		else
			climbAudio.start();
	}
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onInformationButtonClicked(Dali::Toolkit::Button /*button*/)
{
	messageDialog.show();
	messageDialog.showMessage(0); // TODO: Fix!
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onQuitButtonClicked(Dali::Toolkit::Button /*button*/)
{
	quitSignl.emit();
	return true;
}

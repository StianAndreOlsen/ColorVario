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

	// connect signals
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
	// synchronize mute audio button
	if (pageView.currentPageIndex() == 0)
		menu.muteAudioButton().setChecked(!altitudeAudio.isStarted());
	else if (pageView.currentPageIndex() > 0)
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

bool Kystsoft::ColorVario::UserInterface::addMessage(const Message& message)
{
	if (messageDialog.add(message))
	{
		updateMessageButton();
		return true;
	}
	return false;
}

bool Kystsoft::ColorVario::UserInterface::removeMessage(const Message& message)
{
	if (messageDialog.remove(message))
	{
		updateMessageButton();
		return true;
	}
	return false;
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
	pageView.goBackSignal().connect(this, &UserInterface::onGoBack);
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

	menu.create(size);
	layer.Add(menu);

	menu.enableBluetoothButton().ClickedSignal().Connect(this, &UserInterface::onEnableBluetoothButtonClicked);
	menu.lockDisplayButton().ClickedSignal().Connect(this, &UserInterface::onLockDisplayButtonClicked);
	menu.muteAudioButton().ClickedSignal().Connect(this, &UserInterface::onMuteAudioButtonClicked);
	menu.messageButton().ClickedSignal().Connect(this, &UserInterface::onMessageButtonClicked);
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
	layer.SetTouchConsumed(true);
	layer.SetHoverConsumed(true);
	layer.SetVisible(false);
	stage.Add(layer);

	messageDialog.create(size);
	layer.Add(messageDialog);

	std::string about
	(
		"Developed by\n"      // hard line breaks are required
		"Kyrre Holm and\n"    // since TextLabel breaks lines
		"Stian Andre Olsen\n" // even at no-break spaces
		"\n"
		"Please visit facebook.com/ColorVariometer"
	);
	addMessage(Message::information("ColorVario 2.0.0", about));
}

void Kystsoft::ColorVario::UserInterface::onTapGesture(Dali::Actor /*actor*/, const Dali::TapGesture& gesture)
{
	auto y = gesture.screenPoint.y;
	auto height = Dali::Stage::GetCurrent().GetSize().height;
	if (y < height / 4)
		menu.show();
	else if (y > height * 9 / 16)
		menu.hide();

	pageTappedSignl.emit();
}

void Kystsoft::ColorVario::UserInterface::onPanGesture(Dali::Actor /*actor*/, const Dali::PanGesture& gesture)
{
	switch (gesture.state)
	{
	case Dali::Gesture::Started:
	case Dali::Gesture::Continuing:
		menu.translate(gesture.displacement.y);
		break;
	case Dali::Gesture::Finished:
		menu.showOrHide(gesture.velocity.y);
		break;
	default:
		menu.hide();
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
		altitudeRing = altitudePage;
		climbRing = Dali::Toolkit::Control();
		altitudeLabel = altitudePage.altitudeLabel();
		climbLabel = TextLabel();
		speedLabel = TextLabel();
		break;
	case 1: // climb
		altitudeRing = Dali::Toolkit::Control();
		climbRing = climbPage;
		altitudeLabel = climbPage.altitudeLabel();
		climbLabel = climbPage.climbLabel();
		speedLabel = TextLabel();
		break;
	case 2: // speed
		altitudeRing = Dali::Toolkit::Control();
		climbRing = speedPage;
		altitudeLabel = speedPage.altitudeLabel();
		climbLabel = speedPage.climbLabel();
		speedLabel = speedPage.speedLabel();
		break;
	}
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
		else if (pageView.currentPageIndex() > 0)
			climbAudio.start();
	}
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onMessageButtonClicked(Dali::Toolkit::Button /*button*/)
{
	messageDialog.show();
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onQuitButtonClicked(Dali::Toolkit::Button /*button*/)
{
	quitSignl.emit();
	return true;
}

void Kystsoft::ColorVario::UserInterface::updateMessageButton()
{
	auto resourceDir = appSharedResourcePath();
	if (messageDialog.hasErrorMessages())
	{
		menu.messageButton().setUnselectedImage(resourceDir + "Error.png");
		menu.messageButton().setSelectedImage(resourceDir + "ErrorPressed.png");
	}
	else if (messageDialog.hasWarningMessages())
	{
		menu.messageButton().setUnselectedImage(resourceDir + "Warning.png");
		menu.messageButton().setSelectedImage(resourceDir + "WarningPressed.png");
	}
	else
	{
		menu.messageButton().setUnselectedImage(resourceDir + "Information.png");
		menu.messageButton().setSelectedImage(resourceDir + "InformationPressed.png");
	}
}

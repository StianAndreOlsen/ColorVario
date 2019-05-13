#include "ColorVarioUserInterface.h"
#include "AppFunctions.h"
#include <cmath>

void Kystsoft::ColorVario::UserInterface::create()
{
	createPageLayer();
	createMenuLayer();
	createDialogLayer();

	// quit timer to allow the user interface to update before quitting
	quitTimer = Dali::Timer::New(100);
	quitTimer.TickSignal().Connect(this, &UserInterface::onQuitTimer);

	// gestures for showing and hiding the menu
	tapDetector = Dali::TapGestureDetector::New();
	tapDetector.Attach(menu);
	tapDetector.Attach(pageView);
	tapDetector.DetectedSignal().Connect(this, &UserInterface::onTapDetected);
	verticalPanDetector = Dali::PanGestureDetector::New();
	verticalPanDetector.AddDirection(Dali::PanGestureDetector::DIRECTION_VERTICAL);
	verticalPanDetector.Attach(menu);
	verticalPanDetector.Attach(pageView);
	verticalPanDetector.DetectedSignal().Connect(this, &UserInterface::onVerticalPanDetected);

	// gesture for showing the altitude offset dialog
	longPressDetector = Dali::LongPressGestureDetector::New();
	longPressDetector.Attach(menu);
	longPressDetector.Attach(altitudePage.altitudeLabel());
	longPressDetector.Attach(climbPage.altitudeLabel());
	longPressDetector.Attach(speedPage.altitudeLabel());
	longPressDetector.DetectedSignal().Connect(this, &UserInterface::onLongPressDetected);

	// connect signals
	menu.visibleSignal().connect(this, &UserInterface::onMenuVisible);
	auto stage = Dali::Stage::GetCurrent();
	stage.WheelEventSignal().Connect(this, &UserInterface::onWheelEvent);
}

void Kystsoft::ColorVario::UserInterface::load(const Settings& settings)
{
	// sound settings
	altitudeAudio.load(settings);
	climbAudio.load(settings);

	// color settings
	altitudePainter.load(settings);
	climbPainter.load(settings);

	// label settings
	altitudeWriter.load(settings);
	climbWriter.load(settings);
	speedWriter.load(settings);
	altitudeOffsetDialog.load(settings);
}

void Kystsoft::ColorVario::UserInterface::setAltitudeSamplingInterval(double interval)
{
	altitudeAudio.setSamplingInterval(interval);
	altitudePainter.setSamplingInterval(interval);
	altitudeWriter.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setClimbSamplingInterval(double interval)
{
	climbAudio.setSamplingInterval(interval);
	climbPainter.setSamplingInterval(interval);
	climbWriter.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setSpeedSamplingInterval(double interval)
{
	speedWriter.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setAltitudeAccuracy(double accuracy)
{
	Color color = Color::error();
	if (accuracy < 10)
		color = Color::subText();
	else if (accuracy < 100)
		color = Color::warning();
	if (accuracy < 10)
		altitudePage.altitudeLabel().setTextColor(Color::mainText());
	else
		altitudePage.altitudeLabel().setTextColor(color);
	climbPage.altitudeLabel().setTextColor(color);
	speedPage.altitudeLabel().setTextColor(color);
	altitudeOffsetDialog.altitudeLabel().setTextColor(color);
}

void Kystsoft::ColorVario::UserInterface::setAltitudeOffset(double offset)
{
	altitudeOffsetDialog.setOffset(offset);
}

void Kystsoft::ColorVario::UserInterface::setAltitude(double altitude)
{
	// TODO: Enable when creating new icon
//	setAltitudeAccuracy(5);
//	altitude = 1298;
//	setSpeed(37 / 3.6);

	altitudeAudio.setAltitude(altitude);
	altitudePainter.setAltitude(altitude);
	altitudeWriter.setAltitude(altitude);
	altitudeOffsetDialog.setAltitude(altitude);
}

void Kystsoft::ColorVario::UserInterface::setClimb(double climb)
{
	// TODO: Enable when creating new icon
//	climb = 1.9;

	climbAudio.setClimb(climb);
	climbPainter.setClimb(climb);
	climbWriter.setClimb(climb);
}

void Kystsoft::ColorVario::UserInterface::setSpeed(double speed)
{
	speedWriter.setSpeed(speed);
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
	stage.Add(layer);
	layer.RaiseToTop();

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
	stage.Add(layer);
	layer.RaiseToTop();

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
	layer.SetTouchConsumed(true);
	layer.SetHoverConsumed(true);
	layer.SetVisible(false);
	stage.Add(layer);
	layer.RaiseToTop();

	messageDialog.create(size);
	layer.Add(messageDialog);

	altitudeOffsetDialog.create(size);
	layer.Add(altitudeOffsetDialog);
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

void Kystsoft::ColorVario::UserInterface::onWheelEvent(const Dali::WheelEvent& event)
{
	if (messageDialog.isVisible())
		messageDialog.onWheelEvent(event);
	else if (altitudeOffsetDialog.isVisible())
		altitudeOffsetDialog.onWheelEvent(event);
	else
		pageView.onWheelEvent(event);
}

void Kystsoft::ColorVario::UserInterface::onMenuVisible(bool visible)
{
	if (visible)
		pageView.clearKeyInputFocus();
	else
		pageView.setKeyInputFocus();
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

	// colors and labels
	switch (newPageIndex)
	{
	case 0: // altitude
		altitudePainter.setCanvas(altitudePage);
		climbPainter.setCanvas();
		altitudeWriter.setPaper(altitudePage.altitudeLabel());
		climbWriter.setPaper();
		speedWriter.setPaper();
		break;
	case 1: // climb
		altitudePainter.setCanvas();
		climbPainter.setCanvas(climbPage);
		altitudeWriter.setPaper(climbPage.altitudeLabel());
		climbWriter.setPaper(climbPage.climbLabel());
		speedWriter.setPaper();
		break;
	case 2: // speed
		altitudePainter.setCanvas();
		climbPainter.setCanvas(speedPage);
		altitudeWriter.setPaper(speedPage.altitudeLabel());
		climbWriter.setPaper(speedPage.climbLabel());
		speedWriter.setPaper(speedPage.speedLabel());
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
	quitTimer.Start();
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onQuitTimer()
{
	quitSignl.emit();
	return false;
}

void Kystsoft::ColorVario::UserInterface::onTapDetected(Dali::Actor actor, const Dali::TapGesture& gesture)
{
	if (actor == pageView)
	{
		auto y = gesture.screenPoint.y;
		auto height = Dali::Stage::GetCurrent().GetSize().height;
		if (y < height / 4)
			menu.show();
		else if (y > height * 9 / 16)
			menu.hide();

		pageTapDetectedSignl.emit();
	}
}

void Kystsoft::ColorVario::UserInterface::onVerticalPanDetected(Dali::Actor /*actor*/, const Dali::PanGesture& gesture)
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

void Kystsoft::ColorVario::UserInterface::onLongPressDetected(Dali::Actor actor, const Dali::LongPressGesture& gesture)
{
	if (gesture.state == Dali::Gesture::Started && actor != menu)
		altitudeOffsetDialog.show();
}

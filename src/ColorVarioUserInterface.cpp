#include "ColorVarioUserInterface.h"
#include "AppFunctions.h"

Kystsoft::ColorVario::UserInterface::UserInterface()
	: pages(5)
{
	// connect audio signals
	altitudeAudio.mutedSignal().connect(this, &UserInterface::onAltitudeAudioMuted);
	climbAudio.mutedSignal().connect(this, &UserInterface::onClimbAudioMuted);
}

void Kystsoft::ColorVario::UserInterface::create()
{
	// get stage
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 stageSize = stage.GetSize();

	// create pages
	for (auto& page : pages)
	{
		page = Dali::Toolkit::Control::New();
		page.SetSize(stageSize);
		page.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
		page.SetPosition(0, 0);
		page.SetVisible(false);
		stage.Add(page);
	}

	// create color rings
	createAltitudeRing(stageSize);
	createClimbRing(stageSize);

	// create text labels
	createQuitPage(stageSize);
	createAltitudePageLabels(stageSize);
	createClimbPageLabels(stageSize);
	createSpeedPageLabels(stageSize);

	// make sure all value labels are connected
	altitudeLabel = speedPageAltitudeLabel;
	climbLabel = speedPageClimbLabel;
	speedLabel = speedPageSpeedLabel;

	// create toolbars
	createUpperToolbar(stageSize);
	createLowerToolbar(stageSize);

	// connect stage signals
	stage.TouchSignal().Connect(this, &UserInterface::onTouch);
	stage.WheelEventSignal().Connect(this, &UserInterface::onWheelEvent);

	// set initial page
	setCurrentPage(Page::Climb);
}

void Kystsoft::ColorVario::UserInterface::load(const Settings& settings)
{
	// altitude settings
	altitudeAudio.load(settings);
	altitudeLabel.load(settings);
	altitudeRing.load(settings);

	// climb settings
	climbAudio.load(settings);
	climbLabel.load(settings);
	climbRing.load(settings);

	// speed settings
	speedLabel.load(settings);
}

void Kystsoft::ColorVario::UserInterface::setAltitudeSamplingInterval(double interval)
{
	altitudeAudio.setSamplingInterval(interval);
	altitudeLabel.setSamplingInterval(interval);
	altitudeRing.setSamplingInterval(interval);
}

void Kystsoft::ColorVario::UserInterface::setClimbSamplingInterval(double interval)
{
	climbAudio.setSamplingInterval(interval);
	climbLabel.setSamplingInterval(interval);
	climbRing.setSamplingInterval(interval);
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
	if (upperToolbar.IsVisible() && std::difftime(std::time(nullptr), lastUpperToolbarTouch) > 15)
		upperToolbar.SetVisible(false);
	if (lowerToolbar.IsVisible() && std::difftime(std::time(nullptr), lastLowerToolbarTouch) > 15)
		lowerToolbar.SetVisible(false);

	altitudeAudio.setAltitude(altitude);
	altitudeLabel.setAltitude(altitude);
	altitudeRing.setAltitude(altitude);
}

void Kystsoft::ColorVario::UserInterface::setClimb(double climb)
{
	climbAudio.setClimb(climb);
	climbLabel.setClimb(climb);
	climbRing.setClimb(climb);
}

void Kystsoft::ColorVario::UserInterface::setSpeed(double speed)
{
	speedLabel.setSpeed(speed);
}

void Kystsoft::ColorVario::UserInterface::onDisplayLocked(bool locked)
{
	lockDisplayButton.setChecked(locked);
}

void Kystsoft::ColorVario::UserInterface::onBluetoothEnabled(bool enabled)
{
	enableBluetoothButton.setSelectedImage(appSharedResourcePath() + "BluetoothEnabled.png");
	enableBluetoothButton.setChecked(enabled);
}

void Kystsoft::ColorVario::UserInterface::onBluetoothConnected(bool connected)
{
	if (connected)
		enableBluetoothButton.setSelectedImage(appSharedResourcePath() + "BluetoothConnected.png");
	else
		enableBluetoothButton.setSelectedImage(appSharedResourcePath() + "BluetoothEnabled.png");
	enableBluetoothButton.setChecked(true);
}

void Kystsoft::ColorVario::UserInterface::createAltitudeRing(Dali::Vector2 pageSize)
{
	altitudeRing = AltitudeRing::New();
	altitudeRing.SetSize(pageSize);
	altitudeRing.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	altitudeRing.SetPosition(0, 0);
	altitudePage().Add(altitudeRing);
}

void Kystsoft::ColorVario::UserInterface::createClimbRing(Dali::Vector2 pageSize)
{
	climbRing = ClimbRing::New();
	climbRing.SetSize(pageSize);
	climbRing.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbRing.SetPosition(0, 0);
	climbPage().Add(climbRing);
}

void Kystsoft::ColorVario::UserInterface::createQuitPage(Dali::Vector2 pageSize)
{
	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create quit button
	float size = std::min(pageSize.width, pageSize.height) / 2;
	quitButton = PushButton::New();
	quitButton.setUnselectedImage(resourceDir + "QuitUnselected.png");
	quitButton.setSelectedImage(resourceDir + "QuitSelected.png");
	quitButton.SetSize(size, size);
	quitButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton.SetPosition(pageSize.width / 2, pageSize.height / 2);
	quitButton.ClickedSignal().Connect(this, &UserInterface::onQuitButtonClicked);
	quitPage().Add(quitButton);
}

void Kystsoft::ColorVario::UserInterface::createAltitudePageLabels(Dali::Vector2 pageSize)
{
	altitudePageAltitudeLabel = TextLabel::New("Altitude");
	altitudePageAltitudeLabel.SetSize(pageSize.width, pageSize.height / 2);
	altitudePageAltitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	altitudePageAltitudeLabel.SetPosition(0, pageSize.height / 4);
	altitudePageAltitudeLabel.setPointSize(15);
	altitudePageAltitudeLabel.setHorizontalAlignment("CENTER");
	altitudePageAltitudeLabel.setVerticalAlignment("CENTER");
	altitudePageAltitudeLabel.setTextColor(Dali::Color::RED);
	altitudePage().Add(altitudePageAltitudeLabel);
}

void Kystsoft::ColorVario::UserInterface::createClimbPageLabels(Dali::Vector2 pageSize)
{
	climbPageClimbLabel = TextLabel::New("Climb");
	climbPageClimbLabel.SetSize(pageSize.width, pageSize.height / 4);
	climbPageClimbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbPageClimbLabel.SetPosition(0, pageSize.height / 4);
	climbPageClimbLabel.setPointSize(15);
	climbPageClimbLabel.setHorizontalAlignment("CENTER");
	climbPageClimbLabel.setVerticalAlignment("CENTER");
	climbPageClimbLabel.setTextColor(Dali::Color::WHITE);
	climbPage().Add(climbPageClimbLabel);

	climbPageAltitudeLabel = TextLabel::New("Altitude");
	climbPageAltitudeLabel.SetSize(pageSize.width, pageSize.height / 4);
	climbPageAltitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	climbPageAltitudeLabel.SetPosition(0, pageSize.height / 2);
	climbPageAltitudeLabel.setHorizontalAlignment("CENTER");
	climbPageAltitudeLabel.setVerticalAlignment("CENTER");
	climbPageAltitudeLabel.setTextColor(Dali::Color::RED);
	climbPage().Add(climbPageAltitudeLabel);
}

void Kystsoft::ColorVario::UserInterface::createSpeedPageLabels(Dali::Vector2 pageSize)
{
	// TODO: Figure out a layout that fits with the buttons

	speedPageAltitudeLabel = TextLabel::New("Altitude");
//	speedPageAltitudeLabel.SetSize(pageSize.width, pageSize.height / 7);
	speedPageAltitudeLabel.SetSize(pageSize.width, pageSize.height / 4);
	speedPageAltitudeLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
//	speedPageAltitudeLabel.SetPosition(0, pageSize.height / 4);
	speedPageAltitudeLabel.SetPosition(0, pageSize.height / 8);
	speedPageAltitudeLabel.setHorizontalAlignment("CENTER");
	speedPageAltitudeLabel.setVerticalAlignment("CENTER");
//	speedPageAltitudeLabel.setVerticalAlignment("BOTTOM");
	speedPageAltitudeLabel.setTextColor(Dali::Color::RED);
	speedPage().Add(speedPageAltitudeLabel);

	speedPageClimbLabel = TextLabel::New("Climb");
//	speedPageClimbLabel.SetSize(pageSize.width, pageSize.height * 3 / 14);
	speedPageClimbLabel.SetSize(pageSize.width, pageSize.height / 4);
	speedPageClimbLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
//	speedPageClimbLabel.SetPosition(0, pageSize.height * 11 / 28);
	speedPageClimbLabel.SetPosition(0, pageSize.height * 3 / 8);
	speedPageClimbLabel.setPointSize(15);
	speedPageClimbLabel.setHorizontalAlignment("CENTER");
	speedPageClimbLabel.setVerticalAlignment("CENTER");
	speedPageClimbLabel.setTextColor(Dali::Color::WHITE);
	speedPage().Add(speedPageClimbLabel);

	speedPageSpeedLabel = TextLabel::New("Speed");
//	speedPageSpeedLabel.SetSize(pageSize.width, pageSize.height / 7);
	speedPageSpeedLabel.SetSize(pageSize.width, pageSize.height / 4);
	speedPageSpeedLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
//	speedPageSpeedLabel.SetPosition(0, pageSize.height * 17 / 28);
	speedPageSpeedLabel.SetPosition(0, pageSize.height * 5 / 8);
	speedPageSpeedLabel.setHorizontalAlignment("CENTER");
	speedPageSpeedLabel.setVerticalAlignment("CENTER");
//	speedPageSpeedLabel.setVerticalAlignment("TOP");
	speedPageSpeedLabel.setTextColor(Dali::Color::WHITE);
	speedPage().Add(speedPageSpeedLabel);
}

void Kystsoft::ColorVario::UserInterface::createUpperToolbar(Dali::Vector2 pageSize)
{
	float height = pageSize.height / 8;
	float spacing = pageSize.width / 16;

	// create upper toolbar
	upperToolbar = Dali::Toolkit::Control::New();
	upperToolbar.SetSize(pageSize.width, height);
	upperToolbar.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	upperToolbar.SetPosition(pageSize.width / 2, pageSize.height / 4);
	altitudePage().Add(upperToolbar);
	climbPage().Add(upperToolbar);
	speedPage().Add(upperToolbar);
	lastUpperToolbarTouch = std::time(nullptr);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create mute audio button
	muteAudioButton = PushButton::New();
	muteAudioButton.setCheckable(true);
	muteAudioButton.setChecked(false);
	muteAudioButton.setUnselectedImage(resourceDir + "AudioUnmuted.png");
	muteAudioButton.setSelectedImage(resourceDir + "AudioMuted.png");
	muteAudioButton.SetSize(height, height);
	muteAudioButton.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	muteAudioButton.SetPosition(pageSize.width / 2 - spacing / 2, 0);
	muteAudioButton.ClickedSignal().Connect(this, &UserInterface::onMuteAudioButtonClicked);
	upperToolbar.Add(muteAudioButton);

	// create lock display button
	lockDisplayButton = PushButton::New();
	lockDisplayButton.setCheckable(true);
	lockDisplayButton.setChecked(false);
	lockDisplayButton.setUnselectedImage(resourceDir + "DisplayUnlocked.png");
	lockDisplayButton.setSelectedImage(resourceDir + "DisplayLocked.png");
	lockDisplayButton.SetSize(height, height);
	lockDisplayButton.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	lockDisplayButton.SetPosition(pageSize.width / 2 + spacing / 2, 0);
	lockDisplayButton.ClickedSignal().Connect(this, &UserInterface::onLockDisplayButtonClicked);
	upperToolbar.Add(lockDisplayButton);
}

void Kystsoft::ColorVario::UserInterface::createLowerToolbar(Dali::Vector2 pageSize)
{
	float height = pageSize.height / 8;
	float spacing = pageSize.width / 16;

	// create lower toolbar
	lowerToolbar = Dali::Toolkit::Control::New();
	lowerToolbar.SetSize(pageSize.width, height);
	lowerToolbar.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	lowerToolbar.SetPosition(pageSize.width / 2, pageSize.height * 3 / 4);
	lastLowerToolbarTouch = std::time(nullptr);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create enable bluetooth button
	enableBluetoothButton = PushButton::New();
	enableBluetoothButton.setCheckable(true);
	enableBluetoothButton.setChecked(false);
	enableBluetoothButton.setUnselectedImage(resourceDir + "BluetoothDisabled.png");
	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothEnabled.png");
	enableBluetoothButton.SetSize(height, height);
	enableBluetoothButton.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	enableBluetoothButton.SetPosition(pageSize.width / 2 - spacing / 2, 0);
	enableBluetoothButton.ClickedSignal().Connect(this, &UserInterface::onEnableBluetoothButtonClicked);
	lowerToolbar.Add(enableBluetoothButton);

	// create enable location button
	enableLocationButton = PushButton::New();
	enableLocationButton.setCheckable(true);
	enableLocationButton.setChecked(false);
	enableLocationButton.setUnselectedImage(resourceDir + "LocationDisabled.png");
	enableLocationButton.setSelectedImage(resourceDir + "LocationEnabled.png");
	enableLocationButton.SetSize(height, height);
	enableLocationButton.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	enableLocationButton.SetPosition(pageSize.width / 2 + spacing / 2, 0);
	lowerToolbar.Add(enableLocationButton);
}

void Kystsoft::ColorVario::UserInterface::showPage(int newPage)
{
	// TODO: Add code for showing the error page if an error message is available
	if (newPage < 0)
		newPage = 0;
	else if (newPage > int(Page::Speed))
		newPage = int(Page::Speed);
//	else if (newPage > int(Page::Error))
//		newPage = int(Page::Error);
	if (newPage == curPage)
		return;

	switch (Page(newPage))
	{
	case Page::Quit:
		altitudeAudio.mute();
		climbAudio.mute();
		break;
	case Page::Altitude:
		climbAudio.mute();
		altitudeAudio.setMuted(muteAudioButton.isChecked());
		altitudeLabel = altitudePageAltitudeLabel;
		altitudePage().Add(upperToolbar);
		altitudePage().Add(lowerToolbar);
		break;
	case Page::Climb:
		altitudeAudio.mute();
		climbAudio.setMuted(muteAudioButton.isChecked());
		climbLabel = climbPageClimbLabel;
		altitudeLabel = climbPageAltitudeLabel;
		climbPage().Add(climbRing);
		climbPage().Add(upperToolbar);
		climbPage().Add(lowerToolbar);
		break;
	case Page::Speed:
		altitudeAudio.mute();
		climbAudio.setMuted(muteAudioButton.isChecked());
		altitudeLabel = speedPageAltitudeLabel;
		climbLabel = speedPageClimbLabel;
		speedLabel = speedPageSpeedLabel;
		speedPage().Add(climbRing);
		speedPage().Add(upperToolbar);
		speedPage().Add(lowerToolbar);
		break;
	case Page::Error:
		altitudeAudio.mute();
		climbAudio.mute();
		break;
	}

	page(curPage).SetVisible(false);
	page(newPage).SetVisible(true);
	curPage = newPage;
}

void Kystsoft::ColorVario::UserInterface::onAltitudeAudioMuted(bool muted)
{
	if (curPage == int(Page::Altitude))
		muteAudioButton.setChecked(muted);
}

void Kystsoft::ColorVario::UserInterface::onClimbAudioMuted(bool muted)
{
	if (curPage != int(Page::Altitude))
		muteAudioButton.setChecked(muted);
}

void Kystsoft::ColorVario::UserInterface::onTouch(const Dali::TouchData& touch)
{
	if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
	{
		Dali::Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
		if (touch.GetScreenPosition(0).y < stageSize.height / 4)
		{
			lastUpperToolbarTouch = std::time(nullptr);
			if (!upperToolbar.IsVisible())
				upperToolbar.SetVisible(true);
		}
		else if (touch.GetScreenPosition(0).y > stageSize.height * 3 / 4)
		{
			lastLowerToolbarTouch = std::time(nullptr);
			if (!lowerToolbar.IsVisible())
				lowerToolbar.SetVisible(true);
		}
	}
}

void Kystsoft::ColorVario::UserInterface::onWheelEvent(const Dali::WheelEvent& event)
{
	// TODO: Consider doing this in the controller
	if (event.z > 0)
		showNextPage();
	else if (event.z < 0)
		showPreviousPage();
}

bool Kystsoft::ColorVario::UserInterface::onQuitButtonClicked(Dali::Toolkit::Button)
{
	quitSignl.emit();
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onMuteAudioButtonClicked(Dali::Toolkit::Button)
{
	if (curPage == int(Page::Altitude))
		altitudeAudio.setMuted(muteAudioButton.isChecked());
	else
		climbAudio.setMuted(muteAudioButton.isChecked());
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onLockDisplayButtonClicked(Dali::Toolkit::Button)
{
	lockDisplaySignl.emit(lockDisplayButton.isChecked());
	return true;
}

bool Kystsoft::ColorVario::UserInterface::onEnableBluetoothButtonClicked(Dali::Toolkit::Button)
{
	enableBluetoothSignl.emit(enableBluetoothButton.isChecked());
	return true;
}

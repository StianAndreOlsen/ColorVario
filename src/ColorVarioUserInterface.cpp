#include "ColorVarioUserInterface.h"
#include "AppFunctions.h"

Kystsoft::ColorVario::UserInterface::UserInterface()
	: pages(5)
{
}

void Kystsoft::ColorVario::UserInterface::create()
{
	// get stage
	Dali::Stage stage = Dali::Stage::GetCurrent();
	Dali::Vector2 stageSize = stage.GetSize();

	// create empty pages
	for (auto& page : pages)
	{
		page = Dali::Toolkit::Control::New();
		page.SetSize(stageSize);
		page.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
		page.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
		page.SetPosition(0, 0);
		page.SetVisible(false);
		stage.Add(page);
	}

	// create page contents
	createQuitPageContents(stageSize);
	createAltitudePageContents(stageSize);
	createClimbPageContents(stageSize);
	createSpeedPageContents(stageSize);
	createErrorPageContents(stageSize);

	// create toolbars
	createUpperToolbar(stageSize);
	createLowerToolbar(stageSize);

	// make sure the 'drivers' are connected
	altitudeRing = altitudePage();
	climbRing = climbPage();
	altitudeLabel = climbPageAltitudeLabel;
	climbLabel = climbPageClimbLabel;
	speedLabel = speedPageSpeedLabel;

	// connect stage signals
	stage.TouchSignal().Connect(this, &UserInterface::onTouch);
	stage.WheelEventSignal().Connect(this, &UserInterface::onWheelEvent);

	// set initial page
	setCurrentPage(Page::Climb);
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
	// hide toolbars
	if (upperToolbar.IsVisible() && std::difftime(std::time(nullptr), lastUpperToolbarTouch) > 15)
		upperToolbar.SetVisible(false);
	if (lowerToolbar.IsVisible() && std::difftime(std::time(nullptr), lastLowerToolbarTouch) > 15)
		lowerToolbar.SetVisible(false);

	// synchronize mute audio button
	if (Page(curPage) == Page::Altitude)
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
	setCurrentPage(Page::Error);
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

void Kystsoft::ColorVario::UserInterface::setLocationEnabled(bool enabled)
{
	enableLocationButton.setChecked(enabled);
}

void Kystsoft::ColorVario::UserInterface::createQuitPageContents(Dali::Vector2 pageSize)
{
	float size = std::min(pageSize.width, pageSize.height) / 2;
	std::string resourceDir = appSharedResourcePath();

	quitButton = PushButton::New();
	quitButton.SetSize(size, size);
	quitButton.SetParentOrigin(Dali::ParentOrigin::CENTER);
	quitButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton.SetPosition(0, 0);
	quitButton.setUnselectedImage(resourceDir + "QuitUnselected.png");
	quitButton.setSelectedImage(resourceDir + "QuitSelected.png");
	quitButton.ClickedSignal().Connect(this, &UserInterface::onQuitButtonClicked);
	quitPage().Add(quitButton);
}

void Kystsoft::ColorVario::UserInterface::createAltitudePageContents(Dali::Vector2 pageSize)
{
	float width = pageSize.width;
	float height = pageSize.height / 2;

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

void Kystsoft::ColorVario::UserInterface::createClimbPageContents(Dali::Vector2 pageSize)
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

void Kystsoft::ColorVario::UserInterface::createSpeedPageContents(Dali::Vector2 pageSize)
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

void Kystsoft::ColorVario::UserInterface::createErrorPageContents(Dali::Vector2 pageSize)
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

void Kystsoft::ColorVario::UserInterface::createUpperToolbar(Dali::Vector2 pageSize)
{
	float height = pageSize.height / 8;
	float spacing = pageSize.width / 16;
	float width = 2 * height + spacing;

	// create upper toolbar
	upperToolbar = Dali::Toolkit::Control::New();
	upperToolbar.SetSize(width, height);
	upperToolbar.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	upperToolbar.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	upperToolbar.SetPosition(0, height / 2);
	Dali::Stage::GetCurrent().Add(upperToolbar);
	lastUpperToolbarTouch = std::time(nullptr);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create mute audio button
	muteAudioButton = PushButton::New();
	muteAudioButton.SetSize(height, height);
	muteAudioButton.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	muteAudioButton.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	muteAudioButton.SetPosition(0, 0);
	muteAudioButton.setUnselectedImage(resourceDir + "AudioUnmuted.png");
	muteAudioButton.setSelectedImage(resourceDir + "AudioMuted.png");
	muteAudioButton.setCheckable(true);
	muteAudioButton.setChecked(true);
	muteAudioButton.ClickedSignal().Connect(this, &UserInterface::onMuteAudioButtonClicked);
	upperToolbar.Add(muteAudioButton);

	// create lock display button
	lockDisplayButton = PushButton::New();
	lockDisplayButton.SetSize(height, height);
	lockDisplayButton.SetParentOrigin(Dali::ParentOrigin::TOP_RIGHT);
	lockDisplayButton.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	lockDisplayButton.SetPosition(0, 0);
	lockDisplayButton.setUnselectedImage(resourceDir + "DisplayUnlocked.png");
	lockDisplayButton.setSelectedImage(resourceDir + "DisplayLocked.png");
	lockDisplayButton.setCheckable(true);
	lockDisplayButton.setChecked(false);
	lockDisplayButton.ClickedSignal().Connect(this, &UserInterface::onLockDisplayButtonClicked);
	upperToolbar.Add(lockDisplayButton);
}

void Kystsoft::ColorVario::UserInterface::createLowerToolbar(Dali::Vector2 pageSize)
{
	float height = pageSize.height / 8;
	float spacing = pageSize.width / 16;
	float width = 2 * height + spacing;

	// create lower toolbar
	lowerToolbar = Dali::Toolkit::Control::New();
	lowerToolbar.SetSize(width, height);
	lowerToolbar.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	lowerToolbar.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	lowerToolbar.SetPosition(0, -height / 2);
	Dali::Stage::GetCurrent().Add(lowerToolbar);
	lastLowerToolbarTouch = std::time(nullptr);

	// get resource directory
	std::string resourceDir = appSharedResourcePath();

	// create enable bluetooth button
	enableBluetoothButton = PushButton::New();
	enableBluetoothButton.SetSize(height, height);
	enableBluetoothButton.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
	enableBluetoothButton.SetAnchorPoint(Dali::AnchorPoint::TOP_LEFT);
	enableBluetoothButton.SetPosition(0, 0);
	enableBluetoothButton.setUnselectedImage(resourceDir + "BluetoothDisabled.png");
	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothEnabled.png");
	enableBluetoothButton.setCheckable(true);
	enableBluetoothButton.setChecked(false);
	enableBluetoothButton.ClickedSignal().Connect(this, &UserInterface::onEnableBluetoothButtonClicked);
	lowerToolbar.Add(enableBluetoothButton);

	// create enable location button
	// TODO: set disabled and handle icon change
	enableLocationButton = PushButton::New();
	enableLocationButton.SetSize(height, height);
	enableLocationButton.SetParentOrigin(Dali::ParentOrigin::TOP_RIGHT);
	enableLocationButton.SetAnchorPoint(Dali::AnchorPoint::TOP_RIGHT);
	enableLocationButton.SetPosition(0, 0);
	enableLocationButton.setUnselectedImage(resourceDir + "LocationDisabled.png");
	enableLocationButton.setSelectedImage(resourceDir + "LocationEnabled.png");
	enableLocationButton.setCheckable(true);
	enableLocationButton.setChecked(false);
	lowerToolbar.Add(enableLocationButton);
}

void Kystsoft::ColorVario::UserInterface::showPage(int newPage)
{
	newPage = std::clamp(newPage, 0, pageCount() - 1);
	if (newPage == int(Page::Error) && errorLabel.text().empty())
		return; // stay on the existing page
	if (newPage == curPage)
		return;

	// sound
	if (Page(newPage) == Page::Altitude)
	{
		climbAudio.stop();
		altitudeAudio.start();
	}
	else if (Page(curPage) == Page::Altitude || curPage < 0)
	{
		altitudeAudio.stop();
		climbAudio.start();
	}

	// color and labels
	switch (Page(newPage))
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

	if (curPage >= 0)
		page(curPage).SetVisible(false);
	page(newPage).SetVisible(true);
	curPage = newPage;
}

void Kystsoft::ColorVario::UserInterface::onTouch(const Dali::TouchData& touch)
{
	if (touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::FINISHED)
	{
		float height = Dali::Stage::GetCurrent().GetSize().height;
		if (touch.GetScreenPosition(0).y < height / 4)
		{
			lastUpperToolbarTouch = std::time(nullptr);
			upperToolbar.SetVisible(true);
		}
		else if (touch.GetScreenPosition(0).y > height * 3 / 4)
		{
			lastLowerToolbarTouch = std::time(nullptr);
			lowerToolbar.SetVisible(true);
		}
		else
		{
			upperToolbar.SetVisible(false);
			lowerToolbar.SetVisible(false);
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
		if (curPage == int(Page::Altitude))
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

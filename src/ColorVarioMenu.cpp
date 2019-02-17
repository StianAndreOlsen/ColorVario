#include "ColorVarioMenu.h"
#include "AppFunctions.h"
#include "TextLabel.h"
#include <cmath>

void Kystsoft::ColorVario::Menu::create(const Dali::Vector2& size)
{
	auto radius = size.width;

	// menu background
	Dali::Property::Map background;
	{
		background[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
		background[Dali::Toolkit::GradientVisual::Property::UNITS] = Dali::Toolkit::GradientVisual::Units::USER_SPACE;
		background[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, -radius + size.height / 2);
		background[Dali::Toolkit::GradientVisual::Property::RADIUS] = radius;

		Dali::Property::Array stopOffsets;
		stopOffsets.PushBack(0.99f);
		stopOffsets.PushBack(0.99f);
		background[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;

		Dali::Property::Array stopColors;
		stopColors.PushBack(Color::button());
		stopColors.PushBack(Dali::Color::TRANSPARENT);
		background[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	}

	// empty menu
	control = Dali::Toolkit::Control::New();
	control.SetSize(size);
	control.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	control.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	control.SetPosition(0, 0);
	control.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);

	// connect menu signals
	control.OnStageSignal().Connect(this, &Menu::onStageEntered);
	control.TouchSignal().Connect(this, &Menu::onTouch);
	control.KeyEventSignal().Connect(this, &Menu::onKeyEvent);

	// application label
	auto appLabel = TextLabel::New(appName());
	appLabel.SetResizePolicy(Dali::ResizePolicy::USE_NATURAL_SIZE, Dali::Dimension::ALL_DIMENSIONS);
	appLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	appLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	appLabel.SetPosition(0, 0);
	appLabel.setVerticalAlignment("CENTER");
	appLabel.setHorizontalAlignment("CENTER");
	appLabel.setTextColor(Color::subText());
	appLabel.setPointSize(9);
	control.Add(appLabel);

	// buttons and status icons
	createButtonLayer(size, radius);
	createStatusLayer(size);

	// hide automatically after 15 seconds
	autoHideTimer = Dali::Timer::New(15000);
	autoHideTimer.TickSignal().Connect(this, &Menu::onAutoHide);
	autoHideTimer.Start();
}

void Kystsoft::ColorVario::Menu::setVisible(bool visible)
{
	auto pos = control.GetCurrentPosition();
	auto targetY = visible ? 0.0f : -control.GetTargetSize().height;
	if (pos.y != targetY)
	{
		pos.y = targetY;
		auto animation = Dali::Animation::New(0.25f);
		animation.AnimateTo(Dali::Property(control, Dali::Actor::Property::POSITION), pos);
		animation.Play();
	}
	if (visible)
	{
		control.SetKeyInputFocus();
		autoHideTimer.Start();
	}
	else
	{
		control.ClearKeyInputFocus();
		autoHideTimer.Stop();
	}
	visibleSignl.emit(visible);
}

void Kystsoft::ColorVario::Menu::showOrHide(float vy)
{
	if (vy == 0)
	{
		auto posY = control.GetCurrentPosition().y;
		auto height = control.GetTargetSize().height;
		auto visibleHeight = height + posY;
		setVisible(visibleHeight >= height / 2);
	}
	else
		setVisible(vy > 0);
}

void Kystsoft::ColorVario::Menu::translate(float dy)
{
	auto posY = control.GetCurrentPosition().y;
	auto minY = -control.GetTargetSize().height;
	auto maxY = 0.0f;
	auto targetY = std::clamp(posY + dy, minY, maxY);
	if (posY != targetY)
		control.TranslateBy(Dali::Vector3(0, targetY - posY, 0));
}

void Kystsoft::ColorVario::Menu::createButtonLayer(const Dali::Vector2& menuSize, float menuRadius)
{
	// button layer
	auto layer = Dali::Layer::New();
	layer.SetSize(menuSize);
	layer.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	layer.SetPosition(0, 0);
	control.Add(layer);
	layer.RaiseToTop();

	// button size and position
	auto width = menuSize.width / 6;
	auto height = width;
	auto radius = height / 2; // button radius
	auto margin = radius / 4; // margin towards menu radius
	auto spacing = radius / 5; // spacing between buttons
	auto centerRadius = menuRadius - margin - radius; // button center point radius
	auto angle = 2 * std::asin((radius + spacing / 2) / centerRadius); // angle between buttons

	// resource directory
	auto resourceDir = appSharedResourcePath();

	// enable bluetooth button
	auto x = -centerRadius * std::sin(2 * angle);
	auto y =  centerRadius * std::cos(2 * angle) - menuRadius;
	auto enableBluetoothButton = PushButton::New();
	enableBluetoothButton.SetSize(width, height);
	enableBluetoothButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	enableBluetoothButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	enableBluetoothButton.SetPosition(x, y);
	enableBluetoothButton.setUnselectedImage(resourceDir + "BluetoothDisabled.png");
	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothEnabled.png");
	enableBluetoothButton.setCheckable(true);
	enableBluetoothButton.setChecked(false);
	layer.Add(enableBluetoothButton);

	// TODO: Remove this code when bluetooth is implemented, and adjust the button positions
	enableBluetoothButton.disable();
	enableBluetoothButton.SetVisible(false);
	spacing = radius / 1.5;
	angle = 2 * std::asin((radius + spacing / 2) / centerRadius); // angle between buttons

	// lock display button
//	x = -centerRadius * std::sin(angle);
//	y =  centerRadius * std::cos(angle) - menuRadius;
	x = -centerRadius * std::sin(1.5 * angle);
	y =  centerRadius * std::cos(1.5 * angle) - menuRadius;
	auto lockDisplayButton = PushButton::New();
	lockDisplayButton.SetSize(width, height);
	lockDisplayButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	lockDisplayButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	lockDisplayButton.SetPosition(x, y);
	lockDisplayButton.setUnselectedImage(resourceDir + "DisplayUnlocked.png");
	lockDisplayButton.setSelectedImage(resourceDir + "DisplayLocked.png");
	lockDisplayButton.setCheckable(true);
	lockDisplayButton.setChecked(false);
	layer.Add(lockDisplayButton);

	// mute audio button
//	x = 0;
//	y = centerRadius - menuRadius;
	x = -centerRadius * std::sin(0.5 * angle);
	y =  centerRadius * std::cos(0.5 * angle) - menuRadius;
	auto muteAudioButton = PushButton::New();
	muteAudioButton.SetSize(width, height);
	muteAudioButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	muteAudioButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	muteAudioButton.SetPosition(x, y);
	muteAudioButton.setUnselectedImage(resourceDir + "AudioUnmuted.png");
	muteAudioButton.setSelectedImage(resourceDir + "AudioMuted.png");
	muteAudioButton.setCheckable(true);
	muteAudioButton.setChecked(true);
	layer.Add(muteAudioButton);

	// message button
//	x = centerRadius * std::sin(angle);
//	y = centerRadius * std::cos(angle) - menuRadius;
	x = centerRadius * std::sin(0.5 * angle);
	y = centerRadius * std::cos(0.5 * angle) - menuRadius;
	auto messageButton = PushButton::New();
	messageButton.SetSize(width, height);
	messageButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	messageButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	messageButton.SetPosition(x, y);
	messageButton.setUnselectedImage(resourceDir + "Information.png");
	messageButton.setSelectedImage(resourceDir + "InformationPressed.png");
	layer.Add(messageButton);

	// quit button
//	x = centerRadius * std::sin(2 * angle);
//	y = centerRadius * std::cos(2 * angle) - menuRadius;
	x = centerRadius * std::sin(1.5 * angle);
	y = centerRadius * std::cos(1.5 * angle) - menuRadius;
	auto quitButton = PushButton::New();
	quitButton.SetSize(width, height);
	quitButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	quitButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton.SetPosition(x, y);
	quitButton.setUnselectedImage(resourceDir + "Quit.png");
	quitButton.setSelectedImage(resourceDir + "QuitPressed.png");
	layer.Add(quitButton);
}

void Kystsoft::ColorVario::Menu::createStatusLayer(const Dali::Vector2& menuSize)
{
	// status layer
	auto layer = Dali::Layer::New();
	layer.SetSize(menuSize);
	layer.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	layer.SetPosition(0, 0);
	control.Add(layer);
	layer.RaiseToTop();

	// icon size and position
	auto width = menuSize.width / 6 / 2;
	auto height = width;
	auto margin = height / 8;

	// resource directory
	auto resourceDir = appSharedResourcePath();

	// location icon
	auto locationIcon = Dali::Toolkit::ImageView::New(resourceDir + "Location.png");
	locationIcon.SetSize(width, height);
	locationIcon.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	locationIcon.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	locationIcon.SetPosition(0, margin);
	locationIcon.SetVisible(false);
	layer.Add(locationIcon);
}

bool Kystsoft::ColorVario::Menu::onKeyEvent(Dali::Toolkit::Control /*control*/, const Dali::KeyEvent& event)
{
	if (event.state == Dali::KeyEvent::Up)
	{
		if (Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) ||
		    Dali::IsKey(event, Dali::DALI_KEY_BACK))
		{
			hide();
			return true;
		}
	}
	return false;
}

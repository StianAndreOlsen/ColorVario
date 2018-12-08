#include "ColorVarioMenu.h"
#include "AppFunctions.h"
#include "Color.h"
#include "TextLabel.h"
#include <cmath>

Kystsoft::ColorVario::Menu Kystsoft::ColorVario::Menu::New()
{
	auto size = Dali::Stage::GetCurrent().GetSize();
	auto radius = size.height;
	size.height /= 2;

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
	auto menu = Dali::Toolkit::Control::New();
	menu.SetSize(size);
	menu.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	menu.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	menu.SetPosition(0, 0);
	menu.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);

	// application label
	auto appLabel = TextLabel::New(appName());
	appLabel.SetParentOrigin(Dali::ParentOrigin::CENTER);
	appLabel.SetAnchorPoint(Dali::AnchorPoint::BOTTOM_CENTER);
	appLabel.SetPosition(0, 0);
	appLabel.setVerticalAlignment("CENTER");
	appLabel.setHorizontalAlignment("CENTER");
	appLabel.setTextColor(Color::subText());
	appLabel.setPointSize(8);
	menu.Add(appLabel);

	// buttons and status icons
	createButtonLayer(menu, size, radius);
	createStatusLayer(menu, size);

	return menu;
}

void Kystsoft::ColorVario::Menu::createButtonLayer(Dali::Actor parent, const Dali::Vector2& menuSize, float menuRadius)
{
	// button layer
	auto layer = Dali::Layer::New();
	layer.SetSize(menuSize);
	layer.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	layer.SetPosition(0, 0);
	layer.RaiseToTop();
	parent.Add(layer);

	// button size and position
	auto width = menuSize.width / 6;
	auto height = width;
	auto radius = height / 2; // button radius
	auto margin = radius / 4; // margin towards menu radius
	auto spacing = radius / 5; // spacing between buttons
	auto centerRadius = menuRadius - margin - radius; // button center point radius
	auto angle = 2 * std::asin((radius + spacing / 2) / centerRadius); // angle between buttons

	// button background
	Dali::Property::Map background;
	{
		background[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
		background[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, 0);
		background[Dali::Toolkit::GradientVisual::Property::RADIUS] = 0.5f;
		Dali::Property::Array stopOffsets;
		stopOffsets.PushBack(0.99f);
		stopOffsets.PushBack(0.99f);
		background[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
		Dali::Property::Array stopColors;
		stopColors.PushBack(Color::button().shaded(0.5f));
		stopColors.PushBack(Dali::Color::TRANSPARENT);
		background[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	}

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
	enableBluetoothButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);
	enableBluetoothButton.setUnselectedImage(resourceDir + "BluetoothDisabled.png");
	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothEnabled.png");
//	enableBluetoothButton.setSelectedImage(resourceDir + "BluetoothConnected.png");
	enableBluetoothButton.setCheckable(true);
	enableBluetoothButton.setChecked(false);
	layer.Add(enableBluetoothButton);

	// lock display button
	x = -centerRadius * std::sin(angle);
	y =  centerRadius * std::cos(angle) - menuRadius;
	auto lockDisplayButton = PushButton::New();
	lockDisplayButton.SetSize(width, height);
	lockDisplayButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	lockDisplayButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	lockDisplayButton.SetPosition(x, y);
	lockDisplayButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);
	lockDisplayButton.setUnselectedImage(resourceDir + "DisplayUnlocked.png");
	lockDisplayButton.setSelectedImage(resourceDir + "DisplayLocked.png");
	lockDisplayButton.setCheckable(true);
	lockDisplayButton.setChecked(false);
	layer.Add(lockDisplayButton);

	// mute audio button
	x = 0;
	y = centerRadius - menuRadius;
	auto muteAudioButton = PushButton::New();
	muteAudioButton.SetSize(width, height);
	muteAudioButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	muteAudioButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	muteAudioButton.SetPosition(x, y);
	muteAudioButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);
	muteAudioButton.setUnselectedImage(resourceDir + "AudioUnmuted.png");
	muteAudioButton.setSelectedImage(resourceDir + "AudioMuted.png");
	muteAudioButton.setCheckable(true);
	muteAudioButton.setChecked(true);
	layer.Add(muteAudioButton);

	// information button
	x = centerRadius * std::sin(angle);
	y = centerRadius * std::cos(angle) - menuRadius;
	auto informationButton = PushButton::New();
	informationButton.SetSize(width, height);
	informationButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	informationButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	informationButton.SetPosition(x, y);
	informationButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);
	informationButton.setUnselectedImage(resourceDir + "Information.png");
	informationButton.setSelectedImage(resourceDir + "InformationPressed.png");
//	informationButton.setUnselectedImage(resourceDir + "Warning.png");
//	informationButton.setSelectedImage(resourceDir + "WarningPressed.png");
//	informationButton.setUnselectedImage(resourceDir + "Error.png");
//	informationButton.setSelectedImage(resourceDir + "ErrorPressed.png");
	layer.Add(informationButton);

	// quit button background
	{
		Dali::Property::Array stopColors;
		stopColors.PushBack(Color::error().shaded(0.9f));
		stopColors.PushBack(Dali::Color::TRANSPARENT);
		background[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	}

	// quit button
	x = centerRadius * std::sin(2 * angle);
	y = centerRadius * std::cos(2 * angle) - menuRadius;
	auto quitButton = PushButton::New();
	quitButton.SetSize(width, height);
	quitButton.SetParentOrigin(Dali::ParentOrigin::BOTTOM_CENTER);
	quitButton.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	quitButton.SetPosition(x, y);
	quitButton.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);
	quitButton.setUnselectedImage(resourceDir + "Quit.png");
	quitButton.setSelectedImage(resourceDir + "QuitPressed.png");
	layer.Add(quitButton);
}

void Kystsoft::ColorVario::Menu::createStatusLayer(Dali::Actor parent, const Dali::Vector2& menuSize)
{
	// status layer
	auto layer = Dali::Layer::New();
	layer.SetSize(menuSize);
	layer.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	layer.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	layer.SetPosition(0, 0);
	layer.RaiseToTop();
	parent.Add(layer);

	// icon size and position
	auto width = menuSize.width / 6 * 2 / 3;
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
	layer.Add(locationIcon);
}

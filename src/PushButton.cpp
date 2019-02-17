#include "PushButton.h"

Kystsoft::PushButton Kystsoft::PushButton::New()
{
	PushButton button = Dali::Toolkit::PushButton::New();

	// remove default label (equal to BOTTOM_BTN since Tizen 4.0)
	button.setLabel("");

	// remove default background (also introduced in Tizen 4.0)
	button.SetBackgroundColor(Dali::Color::TRANSPARENT);

	return button;
}

void Kystsoft::PushButton::setEllipticBackground(const Color& color)
{
	Dali::Property::Map background;
	background[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
	background[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, 0);
	background[Dali::Toolkit::GradientVisual::Property::RADIUS] = 0.5f;

	Dali::Property::Array stopOffsets;
	stopOffsets.PushBack(0.99f);
	stopOffsets.PushBack(0.99f);
	background[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;

	Dali::Property::Array stopColors;
	stopColors.PushBack(color);
	stopColors.PushBack(Dali::Color::TRANSPARENT);
	background[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;

	SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, background);
}

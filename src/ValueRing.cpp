#include "ValueRing.h"
#include <cmath>

Kystsoft::ValueRing& Kystsoft::ValueRing::operator=(const Dali::Toolkit::Control& rhs)
{
	if (*this != rhs)
	{
		if (*this)
		{
			auto gradient = colorGradient(Dali::Color::TRANSPARENT);
			SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, gradient);
		}
		Dali::Toolkit::Control::operator=(rhs);
		if (*this)
		{
			auto gradient = colorGradient();
			SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, gradient);
		}
	}
	return *this;
}

Dali::Property::Map Kystsoft::ValueRing::colorGradient(Color color)
{
	// radial gradient
	Dali::Property::Map map;
	map[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
	map[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, 0);
	map[Dali::Toolkit::GradientVisual::Property::RADIUS] = 0.5f;
	map[Dali::Toolkit::GradientVisual::Property::SPREAD_METHOD] = Dali::Toolkit::GradientVisual::SpreadMethod::REPEAT;
	Dali::Property::Array stopOffsets;
	stopOffsets.PushBack(0.0f);
	stopOffsets.PushBack(0.5f + 0.5f / std::sqrt(2.0f));
	stopOffsets.PushBack(0.99f);
	stopOffsets.PushBack(1.00f);
	map[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
	Dali::Property::Array stopColors;
	stopColors.PushBack(Dali::Color::TRANSPARENT);
	stopColors.PushBack(Dali::Color::TRANSPARENT);
	stopColors.PushBack(color);
	stopColors.PushBack(color);
	map[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	return map;
}

void Kystsoft::ValueRing::setValue(double value)
{
	averageValue += value;
	Color newColor = color(averageValue);
	newColor.alphaBlend(Color::window()); // alpha blend the new color on top of the background color since Dali don't use gamma correction
	if (newColor == currentColor)
		return;
	currentColor = newColor;
	if (*this)
	{
		auto gradient = colorGradient();
		SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, gradient);
	}
}

void Kystsoft::ValueRing::load(const Settings& settings, const std::string& section)
{
	color.load(settings, section);
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
}

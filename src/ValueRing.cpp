#include "ValueRing.h"
#include <cmath>

void Kystsoft::ValueRing::setValue(double value)
{
	averageValue += value;
	Color newColor = color(averageValue);
	if (newColor == currentColor)
		return;
	currentColor = newColor;

	// radial gradient
	Dali::Property::Map map;
	map[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::GRADIENT;
	map[Dali::Toolkit::GradientVisual::Property::CENTER] = Dali::Vector2(0, 0);
	map[Dali::Toolkit::GradientVisual::Property::RADIUS] = 0.5f;
	Dali::Property::Array stopOffsets;
	stopOffsets.PushBack(0.5f + 0.5f / std::sqrt(2.0f));
	stopOffsets.PushBack(0.99f);
	stopOffsets.PushBack(0.99f);
	map[Dali::Toolkit::GradientVisual::Property::STOP_OFFSET] = stopOffsets;
	Dali::Property::Array stopColors;
	stopColors.PushBack(Dali::Color::TRANSPARENT);
	stopColors.PushBack(newColor);
	stopColors.PushBack(Dali::Color::TRANSPARENT);
	map[Dali::Toolkit::GradientVisual::Property::STOP_COLOR] = stopColors;
	SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, map);
}

void Kystsoft::ValueRing::load(const Settings& settings, const std::string& section)
{
	color.load(settings, section);
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
}

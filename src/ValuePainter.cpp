#include "ValuePainter.h"
#include <cmath>

Kystsoft::ValuePainter::ValuePainter()
{
	valueLateTimer = Dali::Timer::New(5000); // default period
	valueLateTimer.TickSignal().Connect(this, &ValuePainter::onValueLate);
}

void Kystsoft::ValuePainter::setSamplingInterval(double interval)
{
	averageValue.setSamplingInterval(interval);
	auto milliSec = static_cast<unsigned int>(5 * interval * 1000 + 0.5);
	milliSec = std::max(1000u, milliSec); // at least one second
	auto running = valueLateTimer.IsRunning();
	valueLateTimer.SetInterval(milliSec);
	if (!running)
		valueLateTimer.Stop();
}

void Kystsoft::ValuePainter::setCanvas(Dali::Toolkit::Control canvas)
{
	if (control != canvas)
	{
		paint(Dali::Color::TRANSPARENT);
		control = canvas;
		paint(currentColor);
	}
}

void Kystsoft::ValuePainter::setValue(double value)
{
	averageValue += value;
	Color newColor = color(averageValue);
	if (currentColor != newColor)
		paint(currentColor = newColor);
	valueLateTimer.Start();
}

void Kystsoft::ValuePainter::load(const Settings& settings, const std::string& section)
{
	color.load(settings, section);
	setAveragingInterval(settings.value(section + ".averagingInterval", 1.0));
}

Dali::Property::Map Kystsoft::ValuePainter::colorGradient(Color color)
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

void Kystsoft::ValuePainter::paint(Color color)
{
	if (control)
	{
		auto gradient = colorGradient(color);
		control.SetProperty(Dali::Toolkit::Control::Property::BACKGROUND, gradient);
	}
}

bool Kystsoft::ValuePainter::onValueLate()
{
	paint(currentColor = Dali::Color::TRANSPARENT);
	return false;
}

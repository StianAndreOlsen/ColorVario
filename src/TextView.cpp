#include "TextView.h"
#include "Color.h"
#include <cmath>

void Kystsoft::TextView::create(const Dali::Vector2& size)
{
	scrollView = Dali::Toolkit::ScrollView::New();
	scrollView.SetSize(size);
	scrollView.SetParentOrigin(Dali::ParentOrigin::CENTER);
	scrollView.SetAnchorPoint(Dali::AnchorPoint::CENTER);
	scrollView.SetPosition(0, 0);

	Dali::Toolkit::RulerPtr rulerX = new Dali::Toolkit::DefaultRuler();
	rulerX->Disable();
	scrollView.SetRulerX(rulerX);
	rulerY = new Dali::Toolkit::FixedRuler();
	scrollView.SetRulerY(rulerY);
	scrollView.SetScrollingDirection(Dali::PanGestureDetector::DIRECTION_VERTICAL);

	// default margins
	auto r = Dali::Stage::GetCurrent().GetSize().height / 2;
	auto x = size.width / 2;
	auto y = std::sqrt(r * r - x * x);
	auto margin = r - y;
	topMargin = bottomMargin = margin;

	textLabel = TextLabel::New();
	textLabel.SetResizePolicy(Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::WIDTH);
	textLabel.SetResizePolicy(Dali::ResizePolicy::DIMENSION_DEPENDENCY, Dali::Dimension::HEIGHT);
	textLabel.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	textLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	textLabel.SetPosition(0, topMargin);
	textLabel.setVerticalAlignment("TOP");
	textLabel.setHorizontalAlignment("CENTER");
	textLabel.setTextColor(Color::defaultText());
	textLabel.setPointSize(8); // TODO: 7 or 8?
	textLabel.setMultiLine(true);
	textLabel.enableMarkup();

	scrollView.Add(textLabel);
}

void Kystsoft::TextView::setText(const std::string& text)
{
	textLabel.setText(text);

	auto textWidth = textLabel.GetRelayoutSize(Dali::Dimension::WIDTH);
	auto textHeight = textLabel.GetHeightForWidth(textWidth);
	auto scrollHeight = scrollView.GetTargetSize().height;
	scrollHeight = std::max(scrollHeight, topMargin + textHeight + bottomMargin);
	rulerY->SetDomain(Dali::Toolkit::RulerDomain(0, scrollHeight));
}

void Kystsoft::TextView::onWheelEvent(const Dali::WheelEvent& event)
{
	auto scrollDistance = scrollView.GetTargetSize().height / 3;
	auto scrollPosition = scrollView.GetCurrentScrollPosition();
	if (event.z > 0)
		scrollPosition.y += scrollDistance;
	else if (event.z < 0)
		scrollPosition.y -= scrollDistance;
	scrollView.ScrollTo(scrollPosition);
}

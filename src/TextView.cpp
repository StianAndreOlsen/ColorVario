#include "TextView.h"
#include "Color.h"
#include <cmath>

void Kystsoft::TextView::create(const Dali::Vector2& size)
{
	// save height since both scrollView.GetTargetSize() and
	// scrollView.GetCurrentSize() returns 0 before scrollView is shown
	height = size.height;

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
	// TODO: Search for functions that enables scrolling to continue after a flick
//	scrollView.SetScrollFlickAlphaFunction(Dali::AlphaFunction::EASE_OUT);

	scrollView.ScrollCompletedSignal().Connect(this, &TextView::onScrollCompleted);

	// default margins
	auto r = Dali::Stage::GetCurrent().GetSize().height / 2;
	auto x = size.width / 2;
	auto y = std::sqrt(r * r - x * x);
	auto margin = r - y;
	topMargin = bottomMargin = margin;

	textLabel = TextLabel::New();
	// TODO: Figure out why auto-resizing doesn't work!
	//	When going from a short to a long text the long text gets clipped
	textLabel.SetSize(size.width, size.height * 10); // works with fairly long texts!
//	textLabel.SetResizePolicy(Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::WIDTH);
//	textLabel.SetResizePolicy(Dali::ResizePolicy::DIMENSION_DEPENDENCY, Dali::Dimension::HEIGHT);
	textLabel.SetParentOrigin(Dali::ParentOrigin::TOP_CENTER);
	textLabel.SetAnchorPoint(Dali::AnchorPoint::TOP_CENTER);
	textLabel.SetPosition(0, topMargin);
	textLabel.setVerticalAlignment("TOP");
	textLabel.setHorizontalAlignment("CENTER");
	textLabel.setTextColor(Color::defaultText());
	textLabel.setPointSize(7);
	textLabel.setMultiLine(true);
	textLabel.enableMarkup();
	textLabel.OnRelayoutSignal().Connect(this, &TextView::onTextLabelRelayout);
	scrollView.Add(textLabel);
}

void Kystsoft::TextView::setTopMargin(float margin)
{
	topMargin = margin;
	textLabel.SetPosition(0, topMargin);
	updateRuler();
}

void Kystsoft::TextView::setBottomMargin(float margin)
{
	bottomMargin = margin;
	updateRuler();
}

void Kystsoft::TextView::setText(const std::string& text)
{
	textLabel.setText(text);
	// onTextLabelRelayout will call updateRuler
}

void Kystsoft::TextView::onWheelEvent(const Dali::WheelEvent& event)
{
	auto maxY = rulerY->GetDomain().max - height;
	targetY = std::clamp(targetY, 0.0f, maxY);

	// TODO: Why isn't ScrollView showing an overshoot effect?
	auto distance = height - topMargin - bottomMargin;
	if (event.z > 0)
		targetY += distance;
	else if (event.z < 0)
		targetY -= distance;

	scrollToTarget();
}

void Kystsoft::TextView::scrollToTop()
{
	targetY = 0;
	scrollToTarget();
}

void Kystsoft::TextView::scrollToTarget()
{
	auto position = scrollView.GetCurrentScrollPosition();
	position.y = targetY;
	scrollView.ScrollTo(position);
}

void Kystsoft::TextView::updateRuler()
{
	auto textWidth = textLabel.GetRelayoutSize(Dali::Dimension::WIDTH);
	auto textHeight = textLabel.GetHeightForWidth(textWidth);
	auto scrollHeight = topMargin + textHeight + bottomMargin;
	scrollHeight = std::max(scrollHeight, height);
	rulerY->SetDomain(Dali::Toolkit::RulerDomain(0, scrollHeight));

	scrollToTop();
}

void Kystsoft::TextView::onScrollCompleted(const Dali::Vector2& currentScrollPosition)
{
	targetY = currentScrollPosition.y;
}

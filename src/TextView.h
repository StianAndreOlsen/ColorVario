#ifndef KYSTSOFT_TEXTVIEW_H
#define KYSTSOFT_TEXTVIEW_H

#include "TextLabel.h"

namespace Kystsoft {

class TextView : public Dali::ConnectionTracker
{
public:
	TextView() {}
	TextView(const TextView& other) = delete;
	TextView& operator=(const TextView& rhs) = delete;
	operator Dali::Toolkit::ScrollView() const { return scrollView; }
	void create(const Dali::Vector2& size);
	void setTopMargin(float margin);
	void setBottomMargin(float margin);
	void setText(const std::string& text);
	void onWheelEvent(const Dali::WheelEvent& event);
	void scrollToTop();
	void scrollToTarget();
private:
	void updateRuler();
	void onTextLabelRelayout(Dali::Actor) { updateRuler(); }
	void onScrollCompleted(const Dali::Vector2& currentScrollPosition);
	float height = 0;
	Dali::Toolkit::ScrollView scrollView;
	Dali::Toolkit::RulerPtr rulerY;
	float targetY = 0;
	float topMargin = 0;
	float bottomMargin = 0;
	TextLabel textLabel;
};

} // namespace Kystsoft

#endif // KYSTSOFT_TEXTVIEW_H

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
	void setText(const std::string& text);
//	void setTopMargin(float margin);
//	void setBottomMargin(float margin);
	void onWheelEvent(const Dali::WheelEvent& event);
private:
	Dali::Toolkit::ScrollView scrollView;
	Dali::Toolkit::RulerPtr rulerY;
	float topMargin = 0;
	float bottomMargin = 0;
	TextLabel textLabel;
};

} // namespace Kystsoft

#endif // KYSTSOFT_TEXTVIEW_H

#ifndef KYSTSOFT_TEXTLABEL_H
#define KYSTSOFT_TEXTLABEL_H

#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

class TextLabel : public Dali::Toolkit::TextLabel
{
public:
	TextLabel() {}
	TextLabel(const Dali::Toolkit::TextLabel& other) : Dali::Toolkit::TextLabel(other) {}
	TextLabel& operator=(const Dali::Toolkit::TextLabel& rhs) { Dali::Toolkit::TextLabel::operator=(rhs); return *this; }
	static TextLabel DownCast(BaseHandle handle) { return Dali::Toolkit::TextLabel::DownCast(handle); }
	std::string text() const { return GetProperty<std::string>(Property::TEXT); }
	void setText(const std::string& text) { SetProperty(Property::TEXT, text); }
	float pointSize() const { return GetProperty<float>(Property::POINT_SIZE); }
	void setPointSize(float size) { SetProperty(Property::POINT_SIZE, size); }
	bool multiLine() const { return GetProperty<bool>(Property::MULTI_LINE); }
	void setMultiLine(bool multiLine) { SetProperty(Property::MULTI_LINE, multiLine); }
	std::string horizontalAlignment() const { return GetProperty<std::string>(Property::HORIZONTAL_ALIGNMENT); }
	void setHorizontalAlignment(const std::string& align) { SetProperty(Property::HORIZONTAL_ALIGNMENT, align); }
	std::string verticalAlignment() const { return GetProperty<std::string>(Property::VERTICAL_ALIGNMENT); }
	void setVerticalAlignment(const std::string& align) { SetProperty(Property::VERTICAL_ALIGNMENT, align); }
	Dali::Vector4 textColor() const { return GetProperty<Dali::Vector4>(Property::TEXT_COLOR); }
	void setTextColor(const Dali::Vector4& color) { SetProperty(Property::TEXT_COLOR, color); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_TEXTLABEL_H

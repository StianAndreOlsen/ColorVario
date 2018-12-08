#ifndef KYSTSOFT_TEXTLABEL_H
#define KYSTSOFT_TEXTLABEL_H

#include <dali-toolkit/dali-toolkit.h>

namespace Kystsoft {

// https://developer.tizen.org/development/guides/native-application/user-interface/dali/ui-components/textlabel
// https://developer.tizen.org/development/guides/.net-application/natural-user-interface/ui-components/textlabel
class TextLabel : public Dali::Toolkit::TextLabel
{
public:
	TextLabel() {}
	TextLabel(const Dali::Toolkit::TextLabel& other) : Dali::Toolkit::TextLabel(other) {}
	TextLabel& operator=(const Dali::Toolkit::TextLabel& rhs) { Dali::Toolkit::TextLabel::operator=(rhs); return *this; }
	static TextLabel New() { return Dali::Toolkit::TextLabel::New(); }
	static TextLabel New(const std::string& text) { return Dali::Toolkit::TextLabel::New(text); }
	static TextLabel DownCast(BaseHandle handle) { return Dali::Toolkit::TextLabel::DownCast(handle); }
	auto text() const { return GetProperty<std::string>(Property::TEXT); }
	void setText(const std::string& text) { SetProperty(Property::TEXT, text); }
	auto fontFamily() const { return GetProperty<std::string>(Property::FONT_FAMILY); }
	void setFontFamily(const std::string& fontFamily) { SetProperty(Property::FONT_FAMILY, fontFamily); }
	auto fontStyle() const { return GetProperty<std::string>(Property::FONT_STYLE); }
	void setFontStyle(const std::string& fontStyle) { SetProperty(Property::FONT_STYLE, fontStyle); }
	auto pointSize() const { return GetProperty<float>(Property::POINT_SIZE); }
	void setPointSize(float size) { SetProperty(Property::POINT_SIZE, size); }
	auto isMultiLine() const { return GetProperty<bool>(Property::MULTI_LINE); }
	void setMultiLine(bool multiLine) { SetProperty(Property::MULTI_LINE, multiLine); }
	auto horizontalAlignment() const { return GetProperty<std::string>(Property::HORIZONTAL_ALIGNMENT); }
	void setHorizontalAlignment(const std::string& align) { SetProperty(Property::HORIZONTAL_ALIGNMENT, align); }
	auto verticalAlignment() const { return GetProperty<std::string>(Property::VERTICAL_ALIGNMENT); }
	void setVerticalAlignment(const std::string& align) { SetProperty(Property::VERTICAL_ALIGNMENT, align); }
	auto textColor() const { return GetProperty<Dali::Vector4>(Property::TEXT_COLOR); }
	void setTextColor(const Dali::Vector4& color) { SetProperty(Property::TEXT_COLOR, color); }
	auto isMarkupEnabled() const { return GetProperty<bool>(Property::ENABLE_MARKUP); }
	void enableMarkup(bool enable = true) { SetProperty(Property::ENABLE_MARKUP, enable); }
	void disableMarkup(bool disable = true) { enableMarkup(!disable); }
	auto isAutoScrollEnabled() const { return GetProperty<bool>(Property::ENABLE_AUTO_SCROLL); }
	void enableAutoScroll(bool enable = true) { SetProperty(Property::ENABLE_AUTO_SCROLL, enable); }
	void disableAutoScroll(bool disable = true) { enableAutoScroll(!disable); }
	auto autoScrollSpeed() const { return GetProperty<int>(Property::AUTO_SCROLL_SPEED); }
	void setAutoScrollSpeed(int speed) { SetProperty(Property::AUTO_SCROLL_SPEED, speed); }
	auto autoScrollLoopCount() const { return GetProperty<int>(Property::AUTO_SCROLL_LOOP_COUNT); }
	void setAutoScrollLoopCount(int count) { SetProperty(Property::AUTO_SCROLL_LOOP_COUNT, count); }
	auto autoScrollGap() const { return GetProperty<int>(Property::AUTO_SCROLL_GAP); }
	void setAutoScrollGap(int gap) { SetProperty(Property::AUTO_SCROLL_GAP, gap); }
	auto lineSpacing() const { return GetProperty<float>(Property::LINE_SPACING); }
	void setLineSpacing(float spacing) { SetProperty(Property::LINE_SPACING, spacing); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_TEXTLABEL_H

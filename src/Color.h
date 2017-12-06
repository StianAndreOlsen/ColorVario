#ifndef KYSTSOFT_COLOR_H
#define KYSTSOFT_COLOR_H

#include <dali/dali.h>
#include <algorithm>

namespace Kystsoft {

class Color : public Dali::Vector4
{
public:
	Color(const Dali::Vector4& RGBA = Dali::Color::TRANSPARENT) : Dali::Vector4(RGBA) {}
	Color(const Dali::Vector3& RGB, float A = 1) : Dali::Vector4(RGB) { a = A; }
	Color(float R, float G, float B, float A = 1) : Dali::Vector4(R,G,B,A) {}

	// RGBA color space (https://en.wikipedia.org/wiki/RGBA_color_space)
	float red() const { return r; }
	float green() const { return g; }
	float blue() const { return b; }
	float alpha() const { return a; }

	// HSL color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	float hue() const;
	float saturation() const;
	float lightness() const { return (maximum() + minimum()) / 2; }
	float maximum() const { return std::max({r,g,b}); }
	float minimum() const { return std::min({r,g,b}); }
	float chroma() const { return maximum() - minimum(); }
	static Color fromHSLA(float H, float S, float L, float A = 1);
	Color withHue(float H) const { return fromHSLA(H, saturation(), lightness(), alpha()); }
	Color withSaturation(float S) const { return fromHSLA(hue(), S, lightness(), alpha()); }
	Color withLightness(float L) const { return fromHSLA(hue(), saturation(), L, alpha()); }
};

} // namespace Kystsoft

#endif // KYSTSOFT_COLOR_H

#ifndef KYSTSOFT_COLOR_H
#define KYSTSOFT_COLOR_H

#include <dali/dali.h>

namespace Kystsoft {

struct Color : public Dali::Vector4
{
	Color(const Dali::Vector4& RGBA = Dali::Color::TRANSPARENT) : Dali::Vector4(RGBA) {}
	Color(const Dali::Vector3& RGB, float A = 1) : Dali::Vector4(RGB) { a = A; }
	Color(float R, float G, float B, float A = 1) : Dali::Vector4(R,G,B,A) {}
	Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255)
		: Dali::Vector4(R / 255.f, G / 255.f, B / 255.f, A / 255.f) {}

	// RGBA color model (https://en.wikipedia.org/wiki/RGBA_color_space)
	float red() const { return clamp(r, 0, 1); }
	float green() const { return clamp(g, 0, 1); }
	float blue() const { return clamp(b, 0, 1); }
	float alpha() const { return clamp(a, 0, 1); }

	// color properties (https://en.wikipedia.org/wiki/HSL_and_HSV)
	float maximum() const { return std::max({red(), green(), blue()}); }
	float minimum() const { return std::min({red(), green(), blue()}); }
	float chroma() const { return maximum() - minimum(); }
	static constexpr float hueUndefined = -1;
	float hue() const; // returns hueUndefined if chroma == 0

	// HSL color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	float saturationHSL() const;
	float lightness() const { return (maximum() + minimum()) / 2; }
	static Color fromHSLA(float H, float S, float L, float A = 1);
	// TODO: Remove the three functions below
	Color withHue(float H) const { return fromHSLA(H, saturationHSL(), lightness(), alpha()); }
	Color withSaturation(float S) const { return fromHSLA(hue(), S, lightness(), alpha()); }
	Color withLightness(float L) const { return fromHSLA(hue(), saturationHSL(), L, alpha()); }

	// HSV color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	float saturationHSV() const;
	float value() const { return maximum(); }
	static Color fromHSVA(float H, float S, float V, float A = 1);

	// HCY color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	static float luma(float R, float G, float B) { return 0.299f * R + 0.587f * G + 0.114 * B; }
	float luma() const { return luma(red(), green(), blue()); }
	static Color fromHCYA(float H, float C, float Y, float A = 1);

	// helper functions
	static float clamp(float v, float lo, float hi); // TODO: Use std::clamp instead when available in C++17
	static void HCXtoRGB(float H, float C, float X, float* R, float* G, float* B);
};

} // namespace Kystsoft

#endif // KYSTSOFT_COLOR_H

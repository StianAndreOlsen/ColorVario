#ifndef KYSTSOFT_COLOR_H
#define KYSTSOFT_COLOR_H

#include "algorithm.h"
#include <dali/dali.h>
#include <limits>

namespace Kystsoft {

struct Color : public Dali::Vector4
{
	Color(const Dali::Vector4& RGBA = Dali::Color::TRANSPARENT) : Dali::Vector4(RGBA) {}
	Color(const Dali::Vector3& RGB, float A = 1) : Dali::Vector4(RGB) { a = A; }
	Color(float R, float G, float B, float A = 1) : Dali::Vector4(R,G,B,A) {}
	Color(int R, int G, int B, int A = 255) : Dali::Vector4(R,G,B,A) { *this /= 255; }

	// RGB color model (https://en.wikipedia.org/wiki/RGBA_color_space)
	// R is red
	// G is green
	// B is blue
	// A is alpha
	float red() const { return std::clamp(r, 0.0f, 1.0f); }
	float green() const { return std::clamp(g, 0.0f, 1.0f); }
	float blue() const { return std::clamp(b, 0.0f, 1.0f); }
	float alpha() const { return std::clamp(a, 0.0f, 1.0f); }

	// color properties (https://en.wikipedia.org/wiki/HSL_and_HSV)
	float maximum() const { return std::max({red(), green(), blue()}); }
	float minimum() const { return std::min({red(), green(), blue()}); }
	float chroma() const { return maximum() - minimum(); }
	static constexpr float hueUndefined = std::numeric_limits<float>::max(); // use max instead of infinity since max can be written to and read from a text file
	float hue() const; // returns hueUndefined if chroma == 0

	// HSL color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	// H is hue
	// S is saturationHSL
	// L is lightness
	// A is alpha
	float saturationHSL() const;
	float lightness() const { return (maximum() + minimum()) / 2; }
	static Color fromHSLA(float H, float S, float L, float A = 1);

	// HSV color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	// H is hue
	// S is saturationHSV
	// V is value
	// A is alpha
	float saturationHSV() const;
	float value() const { return maximum(); }
	static Color fromHSVA(float H, float S, float V, float A = 1);

	// HSI color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	// H is hue
	// S is saturationHSI
	// I is intensity
	// A is alpha
	float saturationHSI() const;
	float intensity() const { return (red() + green() + blue()) / 3; }
	static Color fromHSIA(float H, float S, float I, float A = 1);

	// HCY color model (https://en.wikipedia.org/wiki/HSL_and_HSV)
	// H is hue
	// C is chroma
	// Y is luma
	// A is alpha
	static float luma(float R, float G, float B) { return 0.299f * R + 0.587f * G + 0.114f * B; }
	float luma() const { return luma(red(), green(), blue()); }
	static Color fromHCYA(float H, float C, float Y, float A = 1);

	// helper functions
	static float hue6(float H); // converts hue to a value in [0,6)
	static void HCtoRGB(float H, float C, float* R, float* G, float* B);
};

} // namespace Kystsoft

#endif // KYSTSOFT_COLOR_H

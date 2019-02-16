#ifndef KYSTSOFT_COLOR_H
#define KYSTSOFT_COLOR_H

#include "algorithm.h"
#include <dali/dali.h>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace Kystsoft {

struct Color : public Dali::Vector4
{
	Color(const Dali::Vector4& RGBA = Dali::Color::TRANSPARENT) : Dali::Vector4(RGBA) {}
	Color(const Dali::Vector3& RGB, float A = 1) : Dali::Vector4(RGB) { a = A; }
	Color(float R, float G, float B, float A = 1) : Dali::Vector4(R,G,B,A) {}

	// from conversions
	void setR(int R) { r = value(R); }
	void setG(int G) { g = value(G); }
	void setB(int B) { b = value(B); }
	void setA(int A) { a = value(A); }
	static Color fromRGBA(int R, int G, int B, float A = 1) { return Color(value(R), value(G), value(B), A); }
	static Color fromRGBA(int R, int G, int B, int A) { return Color(value(R), value(G), value(B), value(A)); }
	static Color fromRGBA(uint32_t RGB, float A) { return fromRGBA(byte2(RGB), byte3(RGB), byte4(RGB), A); }
	static Color fromRGBA(uint32_t RGB, int A) { return fromRGBA(byte2(RGB), byte3(RGB), byte4(RGB), A); }
	static Color fromRGBA(uint32_t RGBA) { return fromRGBA(byte1(RGBA), byte2(RGBA), byte3(RGBA), byte4(RGBA)); }
	static Color fromARGB(uint32_t ARGB) { return fromRGBA(byte2(ARGB), byte3(ARGB), byte4(ARGB), byte1(ARGB)); }
	static Color fromRGBA(const std::string& R, const std::string& G, const std::string& B, const std::string& A) { return Color(value(R), value(G), value(B), value(A)); }
	static Color fromRGBA(const std::string& R, const std::string& G, const std::string& B, float A = 1) { return Color(value(R), value(G), value(B), A); }
	static Color fromRGBA(const std::string& R, const std::string& G, const std::string& B, int A) { return Color(value(R), value(G), value(B), value(A)); }
	static Color fromRGBA(const std::string& RGBA);
	static Color fromARGB(const std::string& ARGB);

	// to conversions
	uint8_t R() const { return byte(red()); }
	uint8_t G() const { return byte(green()); }
	uint8_t B() const { return byte(blue()); }
	uint8_t A() const { return byte(alpha()); }
	uint32_t RGBA() const { return word(red(), green(), blue(), alpha()); }
	uint32_t ARGB() const { return word(alpha(), red(), green(), blue()); }
	std::string fltStringRGBA(const std::string& delim = "\t") const { return fltString(red(), green(), blue(), alpha(), delim); }
	std::string fltStringARGB(const std::string& delim = "\t") const { return fltString(alpha(), red(), green(), blue(), delim); }
	std::string decStringRGBA(const std::string& delim = "\t") const { return decString(red(), green(), blue(), alpha(), delim); }
	std::string decStringARGB(const std::string& delim = "\t") const { return decString(alpha(), red(), green(), blue(), delim); }
	std::string hexStringRGBA() const { return hexString(RGBA()); } // e.g. opaque green: 0x00FF00FF
	std::string hexStringARGB() const { return hexString(ARGB()); } // e.g. opaque green: 0xFF00FF00

	// conversion helpers
	static float value(int v) { return v / 255.0f; }
	static float value(const std::string& str);
	static uint8_t byte1(uint32_t word) { return uint8_t((word >> 24) & 0xFF); }
	static uint8_t byte2(uint32_t word) { return uint8_t((word >> 16) & 0xFF); }
	static uint8_t byte3(uint32_t word) { return uint8_t((word >>  8) & 0xFF); }
	static uint8_t byte4(uint32_t word) { return uint8_t(word & 0xFF); }
	static uint8_t byte(float v) { return uint8_t(v * 255 + 0.5f); }
	static uint32_t word(float v1, float v2, float v3, float v4);
	static uint32_t word(const std::string& str);
	static std::vector<std::string> components(const std::string& str); // split str in up to 4 sub-strings
	static std::string fltString(float v1, float v2, float v3, float v4, const std::string& delim = "\t");
	static std::string decString(float v1, float v2, float v3, float v4, const std::string& delim = "\t");
	static std::string hexString(uint32_t word);

	// system palette, https://developer.tizen.org/design/wearable/visual-design/colors
	static Color systemBackground() { return Color(0, 0, 0); }
	static Color systemPoint() { return fromRGBA(0x4CCFFF, 1.0f); }
	static Color systemButtonBackground() { return fromRGBA(0x00354A, 0.9f); }
	static Color systemButtonPressEffect() { return fromRGBA(0x0092CC, 0.3f); }
	static Color systemMainText() { return Color(1, 1, 1); }
	static Color systemSubText() { return fromRGBA(0xC7C7C7, 1.0f); }
	static Color systemDefaultText() { return fromRGBA(0xD1D1D1, 1.0f); }

	// application palette
	static Color window() { return systemBackground(); }
	static Color headingText() { return systemPoint(); }
	static Color mainText() { return fromRGBA(0xF9F9F9, 1.0f); } // most standard watch apps use this
	static Color subText() { return systemSubText(); }
	static Color defaultText() { return systemDefaultText(); }
	static Color button() { return systemButtonBackground().alphaBlended(window(), 1); } // Dali doesn't use gamma correction
	static Color buttonPressed() { return systemButtonPressEffect().alphaBlended(button(), 1); } // Dali doesn't use gamma correction
	static Color buttonText() { return systemMainText(); }
	static Color disabled() { return fromRGBA(0x808080, 1.0f); }
	static Color information() { return headingText(); }
	static Color success() { return fromRGBA(0x38E000, 1.0f); } // same as calendar heading
	static Color warning() { return fromRGBA(0xEECC00, 1.0f); } // nicer than messages heading
	static Color error() { return fromRGBA(0xFF370F, 1.0f); } // same as calendar Sundays

	// standard gamma used for gamma correction, https://en.wikipedia.org/wiki/Gamma_correction
	static constexpr float stdGamma = 2.2f;

	// blend two colors a and b, https://stackoverflow.com/questions/726549/algorithm-for-additive-color-mixing-for-rgb-values/29321264#29321264
	void blend(const Color& b, float t, float gamma = stdGamma) { *this = blended(b, t, gamma); }
	Color blended(const Color& b, float t, float gamma = stdGamma) const { return blended(*this, b, t, gamma); }
	static Color blended(const Color& a, const Color& b, float t, float gamma = stdGamma);
	static float blendedColorValue(float a, float b, float t, float gamma = stdGamma);
	static float blendedAlphaValue(float a, float b, float t) { return (1 - t) * a + t * b; }

	// alpha blend src on top of dst, https://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending
	void alphaBlend(const Color& dst, float gamma = stdGamma) { *this = alphaBlended(dst, gamma); }
	Color alphaBlended(const Color& dst, float gamma = stdGamma) const { return alphaBlended(*this, dst, gamma); }
	static Color alphaBlended(const Color& src, const Color& dst, float gamma = stdGamma);
	static float alphaBlendedColorValue(float srcRGB, float srcA, float dstRGB, float dstA, float outA, float gamma = stdGamma);
	static float alphaBlendedAlphaValue(float srcA, float dstA) { return srcA + dstA * (1 - srcA); }

	// tint and shade, https://en.wikipedia.org/wiki/Tints_and_shades
	void tint(float t, float gamma = stdGamma) { *this = tinted(t, gamma); }
	void shade(float t, float gamma = stdGamma) { *this = shaded(t, gamma); }
	Color tinted(float t, float gamma = stdGamma) const { return tinted(*this, t, gamma); }
	Color shaded(float t, float gamma = stdGamma) const { return shaded(*this, t, gamma); }
	static Color tinted(const Color& c, float t, float gamma = stdGamma);
	static Color shaded(const Color& c, float t, float gamma = stdGamma);

	// RGB color model, https://en.wikipedia.org/wiki/RGBA_color_space
	// R is red
	// G is green
	// B is blue
	// A is alpha
	float red() const { return std::clamp(r, 0.0f, 1.0f); }
	float green() const { return std::clamp(g, 0.0f, 1.0f); }
	float blue() const { return std::clamp(b, 0.0f, 1.0f); }
	float alpha() const { return std::clamp(a, 0.0f, 1.0f); }

	// color properties, https://en.wikipedia.org/wiki/HSL_and_HSV
	float maximum() const { return std::max({red(), green(), blue()}); }
	float minimum() const { return std::min({red(), green(), blue()}); }
	float chroma() const { return maximum() - minimum(); }
	static constexpr float hueUndefined = std::numeric_limits<float>::max(); // use max instead of infinity since max can be written to and read from a text file
	float hue() const; // returns hueUndefined if chroma == 0

	// HSL color model, https://en.wikipedia.org/wiki/HSL_and_HSV
	// H is hue
	// S is saturationHSL
	// L is lightness
	// A is alpha
	float saturationHSL() const;
	float lightness() const { return (maximum() + minimum()) / 2; }
	static Color fromHSLA(float H, float S, float L, float A = 1);

	// HSV color model, https://en.wikipedia.org/wiki/HSL_and_HSV
	// H is hue
	// S is saturationHSV
	// V is value
	// A is alpha
	float saturationHSV() const;
	float value() const { return maximum(); }
	static Color fromHSVA(float H, float S, float V, float A = 1);

	// HSI color model, https://en.wikipedia.org/wiki/HSL_and_HSV
	// H is hue
	// S is saturationHSI
	// I is intensity
	// A is alpha
	float saturationHSI() const;
	float intensity() const { return (red() + green() + blue()) / 3; }
	static Color fromHSIA(float H, float S, float I, float A = 1);

	// HCY color model, https://en.wikipedia.org/wiki/HSL_and_HSV
	// H is hue
	// C is chroma
	// Y is luma
	// A is alpha
	static float luma(float R, float G, float B) { return 0.299f * R + 0.587f * G + 0.114f * B; }
	float luma() const { return luma(red(), green(), blue()); }
	static Color fromHCYA(float H, float C, float Y, float A = 1);

	// color model helpers
	static float hue6(float H); // converts hue to a value in [0,6)
	static void HCtoRGB(float H, float C, float* R, float* G, float* B);
};

} // namespace Kystsoft

#endif // KYSTSOFT_COLOR_H

#include "Color.h"
#include <iomanip>
#include <sstream>

Kystsoft::Color Kystsoft::Color::fromRGBA(const std::string& RGBA)
{
	auto v = components(RGBA);
	switch (v.size())
	{
	case 4: return fromRGBA(v[0], v[1], v[2], v[3]);  // R G B A
	case 3: return fromRGBA(v[0], v[1], v[2]);        // R G B
	case 2: return fromRGBA(word(v[0]), value(v[1])); // RGB A
	case 1: return fromRGBA(word(v[0]));              // RGBA
	}
	return Color();
}

Kystsoft::Color Kystsoft::Color::fromARGB(const std::string& ARGB)
{
	auto v = components(ARGB);
	switch (v.size())
	{
	case 4: return fromRGBA(v[1], v[2], v[3], v[0]);  // A R G B
	case 3: return fromRGBA(v[0], v[1], v[2]);        // R G B
	case 2: return fromRGBA(word(v[1]), value(v[0])); // A RGB
	case 1: return fromARGB(word(v[0]));              // ARGB
	}
	return Color();
}

float Kystsoft::Color::value(const std::string& str)
{
	std::istringstream is(str);
	if (str.find('.') != str.npos)
	{
		// float
		float v = 0;
		is >> v;
		return v;
	}
	// integer (both decimal and hexadecimal)
	int v = 0;
	is >> v;
	return value(v);
}

uint32_t Kystsoft::Color::word(float v1, float v2, float v3, float v4)
{
	uint32_t b1 = byte(v1);
	uint32_t b2 = byte(v2);
	uint32_t b3 = byte(v3);
	uint32_t b4 = byte(v4);
	return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

uint32_t Kystsoft::Color::word(const std::string& str)
{
	std::istringstream is(str);
	uint32_t w = 0;
	is >> w;
	return w;
}

std::vector<std::string> Kystsoft::Color::components(const std::string& str)
{
	std::istringstream is(str);
	std::string s;
	std::vector<std::string> v;
	while (is >> s && v.size() < 4)
		v.push_back(s);
	return v;
}

std::string Kystsoft::Color::fltString(float v1, float v2, float v3, float v4, const std::string& delim)
{
	std::ostringstream os;
	os.setf(os.fixed, os.floatfield); // always print decimal separator
	os.precision(3); // more than 255 different floats between 0 and 1
	os << v1 << delim
	   << v2 << delim
	   << v3 << delim
	   << v4;
	return os.str();
}

std::string Kystsoft::Color::decString(float v1, float v2, float v3, float v4, const std::string& delim)
{
	std::ostringstream os;
	// cast to int is required to avoid ASCII characters on output
	os << int(byte(v1)) << delim
	   << int(byte(v2)) << delim
	   << int(byte(v3)) << delim
	   << int(byte(v4));
	return os.str();
}

std::string Kystsoft::Color::hexString(uint32_t word)
{
	std::ostringstream os;
	os.setf(os.uppercase);
	os.setf(os.hex, os.basefield);
	os.setf(os.right, os.adjustfield);
	os.fill('0');
	os << "0x" << std::setw(8) << word; // prepend 0x instead of using showbase since showbase will prepend 0X
	return os.str();
}

Kystsoft::Color Kystsoft::Color::blended(const Color& a, const Color& b, float t, float gamma)
{
	return Color
	(
		blendedColorValue(a.red(),   b.red(),   t, gamma),
		blendedColorValue(a.green(), b.green(), t, gamma),
		blendedColorValue(a.blue(),  b.blue(),  t, gamma),
		blendedAlphaValue(a.alpha(), b.alpha(), t)
	);
}

float Kystsoft::Color::blendedColorValue(float a, float b, float t, float gamma)
{
	return std::pow((1 - t) * std::pow(a, gamma) + t * std::pow(b, gamma), 1 / gamma);
}

Kystsoft::Color Kystsoft::Color::alphaBlended(const Color& src, const Color& dst, float gamma)
{
	float outA = alphaBlendedAlphaValue(src.alpha(), dst.alpha());
	return Color
	(
		alphaBlendedColorValue(src.red(),   src.alpha(), dst.red(),   dst.alpha(), outA, gamma),
		alphaBlendedColorValue(src.green(), src.alpha(), dst.green(), dst.alpha(), outA, gamma),
		alphaBlendedColorValue(src.blue(),  src.alpha(), dst.blue(),  dst.alpha(), outA, gamma),
		outA
	);
}

float Kystsoft::Color::alphaBlendedColorValue(float srcRGB, float srcA, float dstRGB, float dstA, float outA, float gamma)
{
	if (outA == 0)
		return 0;
	return std::pow((std::pow(srcRGB, gamma) * srcA + std::pow(dstRGB, gamma) * dstA * (1 - srcA)) / outA, 1 / gamma);
}

Kystsoft::Color Kystsoft::Color::tinted(const Color& c, float t, float gamma)
{
	Color white(1, 1, 1, c.alpha()); // preserve alpha
	return blended(c, white, t, gamma);
}

Kystsoft::Color Kystsoft::Color::shaded(const Color& c, float t, float gamma)
{
	Color black(0, 0, 0, c.alpha()); // preserve alpha
	return blended(c, black, t, gamma);
}

// https://en.wikipedia.org/wiki/Hue
// https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
// https://stackoverflow.com/questions/39118528/rgb-to-hsl-conversion
// https://stackoverflow.com/questions/23090019/fastest-formula-to-get-hue-from-rgb
float Kystsoft::Color::hue() const
{
	float C = chroma();
	if (C == 0)
		return hueUndefined;
	float M = maximum();
	float R = red();
	float G = green();
	float B = blue();
	float H = 0;
	if (M == R)
		H = (G - B) / C + (G < B ? 6 : 0);
	else if (M == G)
		H = (B - R) / C + 2;
	else // M == B
		H = (R - G) / C + 4;
	return H * 60;
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#Saturation
float Kystsoft::Color::saturationHSL() const
{
	float L = lightness();
	if (L == 1)
		return 0;
	float C = chroma();
	return C / (1 - std::fabs(2 * L - 1));
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSL
Kystsoft::Color Kystsoft::Color::fromHSLA(float H, float S, float L, float A /*= 1*/)
{
	H = hue6(H);
	S = std::clamp(S, 0.0f, 1.0f);
	L = std::clamp(L, 0.0f, 1.0f);
	A = std::clamp(A, 0.0f, 1.0f);
	float C = (1 - std::fabs(2 * L - 1)) * S;
	float R = 0, G = 0, B = 0;
	HCtoRGB(H, C, &R, &G, &B);
	float m = L - C / 2;
	return Color(R + m, G + m, B + m, A);
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#Saturation
float Kystsoft::Color::saturationHSV() const
{
	float V = value();
	if (V == 0)
		return 0;
	float C = chroma();
	return C / V;
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
Kystsoft::Color Kystsoft::Color::fromHSVA(float H, float S, float V, float A /*= 1*/)
{
	H = hue6(H);
	S = std::clamp(S, 0.0f, 1.0f);
	V = std::clamp(V, 0.0f, 1.0f);
	A = std::clamp(A, 0.0f, 1.0f);
	float C = V * S;
	float R = 0, G = 0, B = 0;
	HCtoRGB(H, C, &R, &G, &B);
	float m = V - C;
	return Color(R + m, G + m, B + m, A);
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#Saturation
float Kystsoft::Color::saturationHSI() const
{
	float I = intensity();
	if (I == 0)
		return 0;
	float m = minimum();
	return 1 - m / I;
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
Kystsoft::Color Kystsoft::Color::fromHSIA(float H, float S, float I, float A /*= 1*/)
{
	H = hue6(H);
	S = std::clamp(S, 0.0f, 1.0f);
	I = std::clamp(I, 0.0f, 1.0f);
	A = std::clamp(A, 0.0f, 1.0f);
	float Z = 1 - std::fabs(std::fmod(H, 2) - 1);
	float C = 3 * I * S / (1 + Z);
	float R = 0, G = 0, B = 0;
	HCtoRGB(H, C, &R, &G, &B);
	float m = I * (1 - S);
	return Color(R + m, G + m, B + m, A);
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_luma/chroma/hue
Kystsoft::Color Kystsoft::Color::fromHCYA(float H, float C, float Y, float A /*= 1*/)
{
	H = hue6(H);
	C = std::clamp(C, 0.0f, 1.0f);
	Y = std::clamp(Y, 0.0f, 1.0f);
	A = std::clamp(A, 0.0f, 1.0f);
	float R = 0, G = 0, B = 0;
	HCtoRGB(H, C, &R, &G, &B);
	float m = Y - luma(R,G,B);
	return Color(R + m, G + m, B + m, A);
}

float Kystsoft::Color::hue6(float H)
{
	if (H == hueUndefined)
		return H;
	H = std::fmod(H, 360);
	if (H < 0)
		H += 360;
	return H / 60;
}

void Kystsoft::Color::HCtoRGB(float H, float C, float* R, float* G, float* B)
{
	if (H == hueUndefined)
	{
		*R = 0;
		*G = 0;
		*B = 0;
		return;
	}
	float X = C * (1 - std::fabs(std::fmod(H, 2) - 1));
	if (H <= 1)
	{
		*R = C;
		*G = X;
		*B = 0;
	}
	else if (H <= 2)
	{
		*R = X;
		*G = C;
		*B = 0;
	}
	else if (H <= 3)
	{
		*R = 0;
		*G = C;
		*B = X;
	}
	else if (H <= 4)
	{
		*R = 0;
		*G = X;
		*B = C;
	}
	else if (H <= 5)
	{
		*R = X;
		*G = 0;
		*B = C;
	}
	else // H < 6
	{
		*R = C;
		*G = 0;
		*B = X;
	}
}

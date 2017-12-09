#include "Color.h"
#include <cmath>

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
	S = clamp(S, 0, 1);
	L = clamp(L, 0, 1);
	A = clamp(A, 0, 1);
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
	S = clamp(S, 0, 1);
	V = clamp(V, 0, 1);
	A = clamp(A, 0, 1);
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
	S = clamp(S, 0, 1);
	I = clamp(I, 0, 1);
	A = clamp(A, 0, 1);
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
	C = clamp(C, 0, 1);
	Y = clamp(Y, 0, 1);
	A = clamp(A, 0, 1);
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

float Kystsoft::Color::clamp(float v, float lo, float hi)
{
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
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

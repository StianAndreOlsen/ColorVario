#include "Color.h"
#include <cmath>

// https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
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
		H = std::fmod((G - B) / C, 6);
	else if (M == G)
		H = (B - R) / C + 2;
	else
		H = (R - G) / C + 4;
	return 60 * H;
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
	float C = (1 - std::fabs(2 * L - 1)) * S;
	H /= 60;
	float X = C * (1 - std::fabs(std::fmod(H, 2) - 1));
	float R = 0, G = 0, B = 0;
	HCXtoRGB(H, C, X, &R, &G, &B);
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
	float C = V * S;
	H /= 60;
	float X = C * (1 - std::fabs(std::fmod(H, 2) - 1));
	float R = 0, G = 0, B = 0;
	HCXtoRGB(H, C, X, &R, &G, &B);
	float m = V - C;
	return Color(R + m, G + m, B + m, A);
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_luma/chroma/hue
Kystsoft::Color Kystsoft::Color::fromHCYA(float H, float C, float Y, float A /*= 1*/)
{
	H /= 60;
	float X = C * (1 - std::fabs(std::fmod(H, 2) - 1));
	float R = 0, G = 0, B = 0;
	HCXtoRGB(H, C, X, &R, &G, &B);
	float m = Y - luma(R,G,B);
	return Color(R + m, G + m, B + m, A);
}

float Kystsoft::Color::clamp(float v, float lo, float hi)
{
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
}

void Kystsoft::Color::HCXtoRGB(float H, float C, float X, float* R, float* G, float* B)
{
	if (H < 0)
	{
		*R = 0;
		*G = 0;
		*B = 0;
	}
	else if (H <= 1)
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

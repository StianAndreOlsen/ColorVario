#include "Color.h"
#include <cmath>

// https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
float Kystsoft::Color::hue() const
{
	float M = maximum();
	float C = chroma();
	float H = 0;
	if (C > 0)
	{
		if (M == r)
			H = std::fmod((g - b) / C, 6);
		else if (M == g)
			H = (b - r) / C + 2;
		else
			H = (r - g) / C + 4;
	}
	return 60 * H;
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#Saturation
float Kystsoft::Color::saturation() const
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
	float R = 0;
	float G = 0;
	float B = 0;
	if (H < 1)
	{
		R = C;
		G = X;
		B = 0;
	}
	else if (H < 2)
	{
		R = X;
		G = C;
		B = 0;
	}
	else if (H < 3)
	{
		R = 0;
		G = C;
		B = X;
	}
	else if (H < 4)
	{
		R = 0;
		G = X;
		B = C;
	}
	else if (H < 5)
	{
		R = X;
		G = 0;
		B = C;
	}
	else if (H < 6)
	{
		R = C;
		G = 0;
		B = X;
	}
	float m = L - C / 2;
	return Color(R + m, G + m, B + m, A);
}

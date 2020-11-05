#pragma once

#include <algorithm>
#include "Core/Core.h"

class Color
{
public:
	float array[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float& r = array[0];
	float& g = array[1];
	float& b = array[2];
	float& a = array[3];

public:
	
	// --- Constructors ---
	Color() {}

	Color(float red, float green, float blue, float alpha = 1.0f)
	{
		if (red > 255.0f || green > 255.0f || blue > 255.0f || red < 0.0f || green < 0.0f || blue < 0.0f || alpha > 255.0f || alpha < 0.0f)
			APPCONSOLE_WARN_LOG("Colors can't be negative or bigger than 255!");

		if (red > 1.0f)		red /= 255.0f;
		if (green > 1.0f)	green /= 255.0f;
		if (blue > 1.0f)	blue /= 255.0f;
		if (alpha > 1.0f)	alpha /= 255.0f;

		r = red; g = green; b = blue; a = alpha;
	}

	Color(int r, int g, int b) : Color((float)r, (float)g, (float)b)					{}
	Color(int r, int g, int b, int a) : Color((float)r, (float)g, (float)b, (float)a)	{}

	Color(const Color& col) { *this = col; }
	//Color& operator=(const Color& col) { return *this; }

	// --- Setters ---
	void SetColor(const Color& col)
	{
		*this = col; //  r = col.r; g = col.g; b = col.b; a = col.a; 
	}
	
	void SetColor(float red, float green, float blue, float alpha = 1.0f)
	{
		array[0] = red; array[1] = green; array[2] = blue; array[3] = alpha;
	}

	// --- Operators ---
	float* operator & ()				{ return (float*)this; }

	bool operator== (const Color& col)
	{
		bool ret = true;
		for (int i = 0; ret && i < 4; ++i)
			ret = col.array[i] == array[i];

		return ret;
	}
	bool operator!= (const Color& col)
	{
		bool ret = true;
		for (int i = 0; ret && i < 4; ++i)
			ret = col.array[i] != array[i];

		return ret;
	}

	void operator=(const Color& col)
	{
		for (int i = 0;  i < 4; ++i)
			array[i] = col.array[i];
	}

	float operator[](const uint index) { return array[index]; }

	Color operator +(const Color& col)	{ return Color(ClampValue(col.r + r), ClampValue(col.g + g), ClampValue(col.b + b), ClampValue(col.a + a)); }
	Color operator -(const Color& col)	{ return Color(ClampValue(col.r - r), ClampValue(col.g - g), ClampValue(col.b - b), ClampValue(col.a - a));	}
	Color operator *(const Color& col)	{ return Color(ClampValue(col.r * r), ClampValue(col.g * g), ClampValue(col.b * b), ClampValue(col.a * a)); }
	Color operator /(const Color& col)	{ return Color(ClampValue(col.r / r), ClampValue(col.g / g), ClampValue(col.b / b), ClampValue(col.a / a)); }

};


namespace Colors
{
	extern Color White = Color();
	extern Color Black = Color(0.0f, 0.0f, 0.0f);
	extern Color Red = Color(1.0f, 0.0f, 0.0f);
	extern Color Green = Color(0.0f, 1.0f, 0.0f);
	extern Color Blue = Color(0.0f, 0.0f, 1.0f);
	extern Color Yellow = Color(1.0f, 1.0f, 0.0f);

	extern Color ConsoleRed = Color(1.0f, 0.2f, 0.2f);
	extern Color ConsoleYellow = Color(0.8f, 0.8f, 0.1f);
	extern Color ConsoleBlue = Color(0.3f, 0.3f, 1.0f);
	extern Color ConsoleGreen = Color(0.2f, 1.0f, 0.2f);
}
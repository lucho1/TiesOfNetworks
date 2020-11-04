#pragma once

#include <algorithm>
#include "Core/Core.h"

class Color
{
public:
	
	// --- Constructors ---
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

	Color(float red, float green, float blue, float alpha = 1.0f)
	{
		if (red > 255.0f || green > 255.0f || blue > 255.0f || red < 0.0f || green < 0.0f || blue < 0.0f || alpha > 255.0f || alpha < 0.0f)
			APPCONSOLE_WARN_LOG("Colors can't be negative or bigger than 255!");

		r = red; g = green; b = blue; a = alpha;
		if (red > 1.0f)		r /= 255.0f;
		if (green > 1.0f)	g /= 255.0f;
		if (blue > 1.0f)	b /= 255.0f;
		if (alpha > 1.0f)	a /= 255.0f;
	}

	Color(int r, int g, int b) : Color((float)r, (float)g, (float)b)					{}
	Color(int r, int g, int b, int a) : Color((float)r, (float)g, (float)b, (float)a)	{}

	Color(const Color& col) { *this = col; }
	//Color& operator=(const Color& col) { return *this; }

public:

	// --- Getters ---
	Color GetColor() const { return *this; }
	float* GetColorArray() const { float ret[4] = { r,g,b,a }; return ret; }
	std::vector<float> GetColorVector() const { return {r, g, b, a}; }

	// --- Setters ---
	void SetColor(const Color& col)
	{
		*this = col; //  r = col.r; g = col.g; b = col.b; a = col.a; 
	}
	
	void SetColor(float red, float green, float blue, float alpha = 1.0f)
	{
		r = red; g = green; b = blue; a = alpha;
	}


public:
	
	// --- Operations ---
	void SumColor(const Color& col)
	{
		r += ClampValue(r + col.r);
		g += ClampValue(g + col.g);
		b += ClampValue(b + col.b);
		a += ClampValue(a + col.a);
	}

	void SubstractColor(const Color& col)
	{
		r -= ClampValue(r - col.r);
		g -= ClampValue(g - col.g);
		b -= ClampValue(b - col.b);
		a -= ClampValue(a - col.a);
	}

	void MultiplyColor(const Color& col)
	{
		r *= ClampValue(r * col.r);
		g *= ClampValue(g * col.g);
		b *= ClampValue(b * col.b);
		a *= ClampValue(a * col.a);
	}

	void DivideColor(const Color& col)
	{
		r /= ClampValue(r / col.r);
		g /= ClampValue(g / col.g);
		b /= ClampValue(b / col.b);
		a /= ClampValue(a / col.a);
	}


	// --- Operators ---
	float* operator & ()				{ return (float*)this; }

	bool operator== (const Color& col)	{ return (r == col.r && g == col.g && b == col.b && a == col.a); }
	bool operator!= (const Color& col) { return (r != col.r || g != col.g || b != col.b || a != col.a); }
	float operator[](const uint index) { if (index < 4) return GetColorArray()[index]; else return -1; }

	Color operator +(const Color& col)	{ return Color(ClampValue(col.r + r), ClampValue(col.g + g), ClampValue(col.b + b), ClampValue(col.a + a)); }
	Color operator -(const Color& col)	{ return Color(ClampValue(col.r - r), ClampValue(col.g - g), ClampValue(col.b - b), ClampValue(col.a - a));	}
	Color operator *(const Color& col)	{ return Color(ClampValue(col.r * r), ClampValue(col.g * g), ClampValue(col.b * b), ClampValue(col.a * a)); }
	Color operator /(const Color& col)	{ return Color(ClampValue(col.r / r), ClampValue(col.g / g), ClampValue(col.b / b), ClampValue(col.a / a)); }

public:

	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
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
	extern Color ConsoleYellow = Color(1.0f, 0.5f, 0.0f);
	extern Color ConsoleBlue = Color(0.3f, 0.3f, 1.0f);
	extern Color ConsoleGreen = Color(0.2f, 1.0f, 0.2f);
}
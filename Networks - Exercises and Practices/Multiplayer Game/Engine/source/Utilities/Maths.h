#ifndef _MATHS_H_
#define _MATHS_H_

////////////////////////////////////////////////////////////////////////
// DEFINITIONS
////////////////////////////////////////////////////////////////////////

#ifdef PI
	#undef PI
#endif
#ifdef MIN
	#undef MIN
#endif
#ifdef MAX
	#undef MAX
#endif

#define	PI			3.14159265359f
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))

////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////

inline float DegToRad(float degrees)			{ return float(PI * degrees / 180.0f); }
inline float FractionalPart(float number)		{ return float(number - (int)number); }
inline float Lerp(float a, float b, float t)	{ return float(a + t * (b - a)); }
inline float ClampValue(float val)				{ return float(MAX(0.0f, MIN(val, 1.0f))); }



////////////////////////////////////////////////////////////////////////
// VEC2
////////////////////////////////////////////////////////////////////////

//TODO: Pls construct this and change functions below (and the same with vecs below)
struct vec2
{
	union
	{
		struct { float x; float y; };
		struct { float u; float v; };
		float coords[2] = {};
	};
};

inline vec2 operator+(vec2 a, vec2 b)		{ return vec2{ a.x + b.x, a.y + b.y }; }
inline vec2 operator-(vec2 a, vec2 b)		{ return vec2{ a.x - b.x, a.y - b.y }; }
inline vec2 operator*(vec2 a, vec2 b)		{ return vec2{ a.x * b.x, a.y * b.y }; }
inline vec2 operator*(vec2 a, float b)		{ return vec2{ a.x * b, a.y * b }; }
inline vec2 operator*(float a, vec2 b)		{ return b * a; }
inline vec2 operator/(vec2 a, vec2 b)		{ return vec2{ a.x / b.x, a.y / b.y }; }
inline vec2 operator/(vec2 a, float b)		{ return vec2{ a.x / b, a.y / b }; }
inline vec2 operator/(float a, vec2 b)		{ return vec2{ a / b.x, a / b.y }; }
inline vec2 &operator+=(vec2 &a, vec2 b)	{ a = a + b; return a; }
inline vec2 &operator-=(vec2 &a, vec2 b)	{ a = a - b; return a; }
inline vec2 &operator*=(vec2 &a, float b)	{ a = a * b; return a; }
inline vec2 &operator/=(vec2 &a, float b)	{ a = a / b; return a; }
inline vec2 Lerp(vec2 a, vec2 b, float t)	{ vec2 c = a + t * (b - a); return c; }
inline vec2 Floor(vec2 a)					{ return vec2{ floorf(a.x), floorf(a.y) }; }
inline vec2 Ceil(vec2 a)					{ return vec2{ ceilf(a.x), ceilf(a.y) }; }
inline float Dot(vec2 a, vec2 b)			{ return a.x * b.x + a.y * b.y; }
inline float Length2(vec2 a)				{ return Dot(a, a); }
inline float Length(vec2 a)					{ return sqrtf(Length2(a)); }
inline vec2 Normalize(vec2 a)				{ return a / Length(a); }
inline bool IsZero(vec2 a)					{ return Dot(a, a) < FLT_EPSILON; }

inline vec2 DegreesToVec2(float degrees)	{ return { sinf(DegToRad(degrees)), -cosf(DegToRad(degrees)) }; }



////////////////////////////////////////////////////////////////////////
// VEC4
////////////////////////////////////////////////////////////////////////

struct vec4
{
	union
	{
		struct { float x; float y; float z; float w; };
		struct { float r; float g; float b; float a; };
		float coords[4] = {};
	};
};

inline vec4 operator+(vec4 a, vec4 b)		{ return vec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
inline vec4 operator-(vec4 a, vec4 b)		{ return vec4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
inline vec4 operator*(vec4 a, vec4 b)		{ return vec4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
inline vec4 operator*(vec4 a, float b)		{ return vec4{ a.x * b, a.y * b, a.z * b, a.w * b }; }
inline vec4 operator*(float a, vec4 b)		{ return b * a; }
inline vec4 operator/(vec4 a, vec4 b)		{ return vec4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }
inline vec4 operator/(vec4 a, float b)		{ return vec4{ a.x / b, a.y / b, a.z / b, a.w / b }; }
inline vec4 operator/(float a, vec4 b)		{ return vec4{ a / b.x, a / b.y, a / b.z, a / b.w }; }
inline vec4 &operator*=(vec4 &a, float b)	{ a = a * b; return a; }
inline vec4 &operator/=(vec4 &a, float b)	{ a = a / b; return a; }
inline vec4 Lerp(vec4 a, vec4 b, float t)	{ vec4 c = a + t * (b - a); return c; }
inline float Dot(vec4 a, vec4 b)			{ return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
inline bool IsZero(vec4 a)					{ return Dot(a, a) < FLT_EPSILON; }



////////////////////////////////////////////////////////////////////////
// MAT4
////////////////////////////////////////////////////////////////////////

struct mat4
{
	union
	{
		struct
		{
			vec4 v0;
			vec4 v1;
			vec4 v2;
			vec4 v3;
		};
		float data[4][4];
	};
};

inline mat4 identity()
{
	mat4 matrix = {0};
	matrix.v0.x = 1;
	matrix.v1.y = 1;
	matrix.v2.z = 1;
	matrix.v3.w = 1;
	return matrix;
}

inline mat4 translation(vec2 displacement)
{
	mat4 matrix = identity();
	matrix.v3.x = displacement.x;
	matrix.v3.y = displacement.y;
	return matrix;
}

inline mat4 rotationZ(float angle)
{
	mat4 matrix = identity();
	matrix.v0.x = cosf(angle);
	matrix.v0.y = sinf(angle);
	matrix.v1.x = -sinf(angle);
	matrix.v1.y = cosf(angle);
	return matrix;
}

inline mat4 scaling(vec2 scale)
{
	mat4 matrix = identity();
	matrix.v0.x = scale.x;
	matrix.v1.y = scale.y;
	return matrix;
}

inline vec4 operator*(const mat4 &a, const vec4 &b)
{
	vec4 result;
	result.x = Dot(vec4{ a.v0.x, a.v1.x, a.v2.x, a.v3.x }, b);
	result.y = Dot(vec4{ a.v0.y, a.v1.y, a.v2.y, a.v3.y }, b);
	result.z = Dot(vec4{ a.v0.z, a.v1.z, a.v2.z, a.v3.z }, b);
	result.w = Dot(vec4{ a.v0.w, a.v1.w, a.v2.w, a.v3.w }, b);
	return result;
}

inline mat4 operator*(const mat4 &a, const mat4 &b)
{
	mat4 result = identity();
	result.v0 = a * b.v0;
	result.v1 = a * b.v1;
	result.v2 = a * b.v2;
	result.v3 = a * b.v3;
	return result;
}

inline vec2 vec2_cast(vec4 a)
{
	vec2 result = { a.x, a.y };
	return result;
}

#endif //_MATHS_H_
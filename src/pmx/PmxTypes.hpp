#pragma once
#include <cstdint>

namespace vitriol
{
	enum class PmxStringEncoding : uint8_t
	{
		UTF16LittleEndian = 0,
		UTF8 = 1
	};

	typedef struct Vec2
	{
		float x;
		float y;
	} Vec2;

	typedef struct Vec3
	{
		float x;
		float y;
		float z;
	} Vec3;

	typedef struct Vec4
	{
		float x;
		float y;
		float z;
		float w;
	} Vec4;
};

#pragma once
#include <string>
#include <vector>
#include "PmxTypes.hpp"

namespace vitriol
{
	template <class T> T Limit(T value, T min, T max, T fallback);
	int32_t ReadIndex(std::istream* stream, uint8_t typeSize);
	std::string ReadString(std::istream* stream, PmxStringEncoding sourceEncoding);
}
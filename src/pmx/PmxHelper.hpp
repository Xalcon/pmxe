#pragma once
#include <string>
#include <vector>
#include "PmxTypes.hpp"

namespace vitriol
{
	template <class T> T limit(T value, T min, T max, T fallback);
	int32_t readIndex(std::istream& stream, uint8_t typeSize);
	std::string readString(std::istream& stream, PmxStringEncoding sourceEncoding);

	void writeIndex(std::ostream& stream, int32_t value, uint8_t typeSize);
	void writeString(std::ostream& stream, std::string& string, PmxStringEncoding targetEncoding);
}
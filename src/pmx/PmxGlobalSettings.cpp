#include <cstdio>
#include <cstdint>
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	void PmxGlobalSettings::Parse(std::istream* stream)
	{
		uint8_t size;
		stream->read(reinterpret_cast<char*>(&size), sizeof(size));
		stream->read(reinterpret_cast<char*>(this), sizeof(uint8_t) * size);
		if (size > 8)
			printf("WARN: PMX Global Configuration contains more than 8 configurations! Data will be missing on save! Found %d configs\n", size);
	}
}

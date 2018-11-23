#include <cstdio>
#include <cstdint>
#include "PmxGlobalSettings.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	#define PMX_GLOBAL_SETTINGS_SIZE 8

	void PmxGlobalSettings::Parse(std::istream& stream)
	{
		uint8_t size;
		stream.read(reinterpret_cast<char*>(&size), sizeof(size));
		stream.read(reinterpret_cast<char*>(this), sizeof(uint8_t) * size);
		if (size > PMX_GLOBAL_SETTINGS_SIZE)
			printf("WARN: PMX Global Configuration contains more than %d configurations! Data will be missing on save! Found %d configs\n", PMX_GLOBAL_SETTINGS_SIZE, size);
	}

	void PmxGlobalSettings::Save(std::ostream& stream)
	{
		streamWrite(stream, static_cast<uint8_t>(PMX_GLOBAL_SETTINGS_SIZE)); // write the size as a 1 byte value, fixed to 8
		streamWrite(stream, this, sizeof(PmxGlobalSettings));
	}
}

#pragma once
#include <cstdint>
#include <istream>
#include <vector>

#include "PmxDisplayPane.hpp"
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	enum class PmxDisplayFrameType : uint8_t
	{
		Bone = 0,
		Morph = 1,
	};

	class PmxFrameData
	{
	public:
		PmxDisplayFrameType frameType;
		int32_t index;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings) const;
	};

	class PmxDisplayData
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		uint8_t isSpecialFrame;
		std::vector<PmxFrameData> frames;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings);
	};
}

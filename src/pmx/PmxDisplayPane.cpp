#include "PmxDisplayPane.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
	void PmxFrameData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		stream->read(reinterpret_cast<char*>(&this->frameType), sizeof(this->frameType));
		const int size = this->frameType == PmxDisplayFrameType::Bone
			? settings.boneIndexSize
			: this->frameType == PmxDisplayFrameType::Morph
			? settings.morphIndexSize : throw "Invalid frame type";

		this->index = ReadIndex(stream, size);
	}

	void PmxDisplayData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->displayNameLocal = ReadString(stream, settings.textEncoding);
		this->displayNameUniversal = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->isSpecialFrame), sizeof(this->isSpecialFrame));
		int frameCount;
		stream->read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
		this->frames.reserve(frameCount);
		for (auto i = 0; i < frameCount; i++)
		{
			PmxFrameData frameData;
			frameData.Parse(stream, settings);
			this->frames.emplace_back(frameData);
		}
	}
}

#include "PmxBone.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
	void PmxParentBone::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->parentIndex = ReadIndex(stream, settings.boneIndexSize);
		stream->read(reinterpret_cast<char*>(&this->parentInfluence), sizeof(this->parentInfluence));
	}

	void PmxBoneFixedAxis::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		stream->read(reinterpret_cast<char*>(&this->axisDirection), sizeof(this->axisDirection));
	}

	void PmxBoneLocalCoordinate::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		stream->read(reinterpret_cast<char*>(&this->x), sizeof(this->x) + sizeof(this->z));
	}

	void PmxBoneExternalParent::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->parentIndex = ReadIndex(stream, settings.boneIndexSize);
	}

	void PmxBoneIkAngleLimit::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		stream->read(reinterpret_cast<char*>(&this->min), sizeof(this->min) + sizeof(this->max));
	}

	void PmxBoneIkLinks::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex = ReadIndex(stream, settings.boneIndexSize);
		stream->read(reinterpret_cast<char*>(&this->hasLimits), sizeof(this->hasLimits));
		if (this->hasLimits)
			this->angleLimits.Parse(stream, settings);
	}

	void PmxBoneIk::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->targetIndex = ReadIndex(stream, settings.boneIndexSize);
		int size = sizeof(this->loopCount) + sizeof(this->limitRadians) + sizeof(this->linkCount);
		stream->read(reinterpret_cast<char*>(&this->loopCount), size);
		this->ikLinks.reserve(size);
		for (auto i = 0; i < this->linkCount; i++)
		{
			PmxBoneIkLinks link;
			link.Parse(stream, settings);
			this->ikLinks.emplace_back(link);
		}
	}

	void PmxBoneData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		auto loc = stream->tellg();
		this->boneNameLocal = ReadString(stream, settings.textEncoding);
		this->boneNameUniversal = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->position), sizeof(this->position));
		stream->read(reinterpret_cast<char*>(&this->parentBoneIndex), settings.boneIndexSize);
		stream->read(reinterpret_cast<char*>(&this->layer), sizeof(this->layer));
		stream->read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::IndexedTailPosition))
			stream->read(reinterpret_cast<char*>(&this->tailPositionBoneIndex), settings.boneIndexSize);
		else
			stream->read(reinterpret_cast<char*>(&this->tailPosition), sizeof(this->tailPosition));

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritRotation) || static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritTranslation))
			this->inheritBone.Parse(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::FixedAxis))
			this->fixedAxis.Parse(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::LocalCoordinate))
			this->localCoordinate.Parse(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::ExternalParentDeform))
			this->externalParent.Parse(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InverseKinematics))
			this->ikBoneData.Parse(stream, settings);
	}
}

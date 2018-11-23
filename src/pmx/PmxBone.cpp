#include "PmxBone.hpp"
#include "PmxHelper.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxParentBone::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->parentIndex = readIndex(stream, settings.boneIndexSize);
		stream.read(reinterpret_cast<char*>(&this->parentInfluence), sizeof(this->parentInfluence));
	}

	void PmxParentBone::Save(std::ostream& stream, const PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->parentIndex, settings.boneIndexSize);
		streamWrite(stream, this->parentInfluence);
	}

	void PmxBoneFixedAxis::Parse(std::istream& stream)
	{
		stream.read(reinterpret_cast<char*>(&this->axisDirection), sizeof(this->axisDirection));
	}

	void PmxBoneFixedAxis::Save(std::ostream& stream) const
	{
		streamWrite(stream, this->axisDirection);
	}

	void PmxBoneLocalCoordinate::Parse(std::istream& stream)
	{
		stream.read(reinterpret_cast<char*>(&this->x), sizeof(this->x) + sizeof(this->z));
	}

	void PmxBoneLocalCoordinate::Save(std::ostream& stream) const
	{
		streamWrite(stream, this->x);
		streamWrite(stream, this->z);
	}

	void PmxBoneExternalParent::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->parentIndex = readIndex(stream, settings.boneIndexSize);
	}

	void PmxBoneExternalParent::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->parentIndex, settings.boneIndexSize);
	}

	void PmxBoneIkAngleLimit::Parse(std::istream& stream)
	{
		stream.read(reinterpret_cast<char*>(&this->min), sizeof(this->min) + sizeof(this->max));
	}

	void PmxBoneIkAngleLimit::Save(std::ostream& stream) const
	{
		streamWrite(stream, this->min);
		streamWrite(stream, this->max);
	}

	void PmxBoneIkLinks::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex = readIndex(stream, settings.boneIndexSize);
		stream.read(reinterpret_cast<char*>(&this->hasLimits), sizeof(this->hasLimits));
		if (this->hasLimits)
			this->angleLimits.Parse(stream);
	}

	void PmxBoneIkLinks::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->boneIndex, settings.boneIndexSize);
		streamWrite(stream, this->hasLimits);
		if (this->hasLimits)
			this->angleLimits.Save(stream);
	}

	void PmxBoneIk::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->targetIndex = readIndex(stream, settings.boneIndexSize);
		const int size = sizeof(this->loopCount) + sizeof(this->limitRadians) + sizeof(this->linkCount);
		// batch read
		stream.read(reinterpret_cast<char*>(&this->loopCount), size);
		this->ikLinks.reserve(size);
		for (auto i = 0; i < this->linkCount; i++)
		{
			PmxBoneIkLinks link;
			link.Parse(stream, settings);
			this->ikLinks.emplace_back(link);
		}
	}

	void PmxBoneIk::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->targetIndex, settings.boneIndexSize);
		streamWrite(stream, this->loopCount);
		streamWrite(stream, this->limitRadians);
		streamWrite(stream, this->linkCount);

		for (auto& ikLink : this->ikLinks)
		{
			ikLink.Save(stream, settings);
		}
	}

	void PmxBoneData::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);
		stream.read(reinterpret_cast<char*>(&this->position), sizeof(this->position));
		this->parentBoneIndex = readIndex(stream, settings.boneIndexSize);
		stream.read(reinterpret_cast<char*>(&this->layer), sizeof(this->layer));
		stream.read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::IndexedTailPosition))
			this->tailPositionBoneIndex = readIndex(stream, settings.boneIndexSize);
		else
			stream.read(reinterpret_cast<char*>(&this->tailPosition), sizeof(this->tailPosition));

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritRotation) || static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritTranslation))
			this->inheritBone.Parse(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::FixedAxis))
			this->fixedAxis.Parse(stream);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::LocalCoordinate))
			this->localCoordinate.Parse(stream);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::ExternalParentDeform))
			this->externalParent.Parse(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InverseKinematics))
			this->ikBoneData.Parse(stream, settings);
	}

	void PmxBoneData::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		streamWrite(stream, this->position);
		writeIndex(stream, this->parentBoneIndex, settings.boneIndexSize);
		streamWrite(stream, this->layer);
		streamWrite(stream, this->flags);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::IndexedTailPosition))
			writeIndex(stream, this->tailPositionBoneIndex, settings.boneIndexSize);
		else
			streamWrite(stream, this->tailPosition);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritRotation) || static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritTranslation))
			this->inheritBone.Save(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::FixedAxis))
			this->fixedAxis.Save(stream);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::LocalCoordinate))
			this->localCoordinate.Save(stream);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::ExternalParentDeform))
			this->externalParent.Save(stream, settings);

		if (static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InverseKinematics))
			this->ikBoneData.Save(stream, settings);
	}
}

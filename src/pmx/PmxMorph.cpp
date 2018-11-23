#include "PmxMorph.hpp"
#include "PmxHelper.hpp"
#include "PmxException.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxGroupMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->morphIndex = readIndex(stream, settings.morphIndexSize);
		stream.read(reinterpret_cast<char*>(&this->influence), sizeof(this->influence));
	}

	void PmxGroupMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->morphIndex, settings.morphIndexSize);
		streamWrite(stream, this->influence);
	}

	void PmxVertexMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->vertexIndex = readIndex(stream, settings.vertexIndexSize);
		stream.read(reinterpret_cast<char*>(&this->translation), sizeof(this->translation));
	}

	void PmxVertexMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->vertexIndex, settings.vertexIndexSize);
		streamWrite(stream, this->translation);
	}

	void PmxBoneMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex = readIndex(stream, settings.boneIndexSize);
		stream.read(reinterpret_cast<char*>(&this->translation), sizeof(this->translation));
		stream.read(reinterpret_cast<char*>(&this->rotation), sizeof(this->rotation));
	}

	void PmxBoneMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->boneIndex, settings.boneIndexSize);
		streamWrite(stream, this->translation);
		streamWrite(stream, this->rotation);
	}

	void PmxUVMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->vertexIndex = readIndex(stream, settings.vertexIndexSize);
		stream.read(reinterpret_cast<char*>(&this->data), sizeof(this->data));
	}

	void PmxUVMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->vertexIndex, settings.vertexIndexSize);
		streamWrite(stream, this->data);
	}

	void PmxMaterialMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->materialIndex = readIndex(stream, settings.materialIndexSize);
		stream.read(reinterpret_cast<char*>(&this->blendingMode), sizeof(this->blendingMode));
		stream.read(reinterpret_cast<char*>(&this->diffuse), sizeof(this->diffuse));
		stream.read(reinterpret_cast<char*>(&this->specular), sizeof(this->specular));
		stream.read(reinterpret_cast<char*>(&this->specularStrength), sizeof(this->specularStrength));
		stream.read(reinterpret_cast<char*>(&this->ambient), sizeof(this->ambient));
		stream.read(reinterpret_cast<char*>(&this->edgeColor), sizeof(this->edgeColor));
		stream.read(reinterpret_cast<char*>(&this->edgeSize), sizeof(this->edgeSize));
		stream.read(reinterpret_cast<char*>(&this->textureTint), sizeof(this->textureTint));
		stream.read(reinterpret_cast<char*>(&this->environmentTint), sizeof(this->environmentTint));
		stream.read(reinterpret_cast<char*>(&this->toonTint), sizeof(this->toonTint));
	}

	void PmxMaterialMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->materialIndex, settings.materialIndexSize);
		streamWrite(stream, this->blendingMode);
		streamWrite(stream, this->diffuse);
		streamWrite(stream, this->specular);
		streamWrite(stream, this->specularStrength);
		streamWrite(stream, this->ambient);
		streamWrite(stream, this->edgeColor);
		streamWrite(stream, this->edgeSize);
		streamWrite(stream, this->textureTint);
		streamWrite(stream, this->environmentTint);
		streamWrite(stream, this->toonTint);
	}

	void PmxFlipMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->morphIndex = readIndex(stream, settings.morphIndexSize);
		stream.read(reinterpret_cast<char*>(&this->influence), sizeof(this->influence));
	}

	void PmxFlipMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->morphIndex, settings.morphIndexSize);
		streamWrite(stream, this->influence);
	}

	void PmxImpulseMorph::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->rigidBodyIndex = readIndex(stream, settings.rigidBodyIndexSize);
		stream.read(reinterpret_cast<char*>(&this->localFlag), sizeof(this->localFlag));
		stream.read(reinterpret_cast<char*>(&this->movementSpeed), sizeof(this->movementSpeed));
		stream.read(reinterpret_cast<char*>(&this->rotationTorque), sizeof(this->rotationTorque));
	}

	void PmxImpulseMorph::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->rigidBodyIndex, settings.rigidBodyIndexSize);
		streamWrite(stream, this->localFlag);
		streamWrite(stream, this->movementSpeed);
		streamWrite(stream, this->rotationTorque);
	}

	void PmxMorphData::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);
		stream.read(reinterpret_cast<char*>(&this->panelType), sizeof(this->panelType));
		stream.read(reinterpret_cast<char*>(&this->morphType), sizeof(this->morphType));
		int32_t count;
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
		this->dataList.reserve(count);
		for (auto i = 0; i < count; i++)
		{
			std::unique_ptr<PmxMorphOffsetData> offsetData;
			switch (this->morphType)
			{
			case PmxMorphType::Group:
				offsetData = std::make_unique<PmxGroupMorph>();
				break;
			case PmxMorphType::Vertex:
				offsetData = std::make_unique<PmxVertexMorph>();
				break;
			case PmxMorphType::Bone:
				offsetData = std::make_unique<PmxBoneMorph>();
				break;
			case PmxMorphType::UV:
			case PmxMorphType::UVExt1:
			case PmxMorphType::UVExt2:
			case PmxMorphType::UVExt3:
			case PmxMorphType::UVExt4:
				offsetData = std::make_unique<PmxUVMorph>();
				break;
			case PmxMorphType::Material:
				offsetData = std::make_unique<PmxMaterialMorph>();
				break;
			case PmxMorphType::Flip:
				offsetData = std::make_unique<PmxFlipMorph>();
				break;
			case PmxMorphType::Impulse:
				offsetData = std::make_unique<PmxImpulseMorph>();
				break;
			default:
				throw PMX_EXCEPTION("Unknown Morph type");
			}
			offsetData->Parse(stream, settings);
			this->dataList.emplace_back(std::move(offsetData));
		}
	}

	void PmxMorphData::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		streamWrite(stream, this->panelType);
		streamWrite(stream, this->morphType);
		streamWrite(stream, static_cast<int32_t>(this->dataList.size()));
		for (auto& offsetData : this->dataList)
			offsetData->Save(stream, settings);
	}
}

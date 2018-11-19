#include "PmxMorph.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
	void PmxGroupMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->morphIndex = ReadIndex(stream, settings.morphIndexSize);
		stream->read(reinterpret_cast<char*>(&this->influence), sizeof(this->influence));
	}

	void PmxVertexMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->vertexIndex = ReadIndex(stream, settings.vertexIndexSize);
		stream->read(reinterpret_cast<char*>(&this->translation), sizeof(this->translation));
	}

	void PmxBoneMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex = ReadIndex(stream, settings.boneIndexSize);
		stream->read(reinterpret_cast<char*>(&this->translation), sizeof(this->translation));
		stream->read(reinterpret_cast<char*>(&this->rotation), sizeof(this->rotation));
	}

	void PmxUVMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->vertexIndex = ReadIndex(stream, settings.vertexIndexSize);
		stream->read(reinterpret_cast<char*>(&this->data), sizeof(this->data));
	}

	void PmxMaterialMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->materialIndex = ReadIndex(stream, settings.materialIndexSize);
		stream->read(reinterpret_cast<char*>(&this->blendingMode), sizeof(this->blendingMode));
		stream->read(reinterpret_cast<char*>(&this->diffuse), sizeof(this->diffuse));
		stream->read(reinterpret_cast<char*>(&this->specular), sizeof(this->specular));
		stream->read(reinterpret_cast<char*>(&this->specularStrength), sizeof(this->specularStrength));
		stream->read(reinterpret_cast<char*>(&this->ambient), sizeof(this->ambient));
		stream->read(reinterpret_cast<char*>(&this->edgeColor), sizeof(this->edgeColor));
		stream->read(reinterpret_cast<char*>(&this->edgeSize), sizeof(this->edgeSize));
		stream->read(reinterpret_cast<char*>(&this->textureTint), sizeof(this->textureTint));
		stream->read(reinterpret_cast<char*>(&this->environmentTint), sizeof(this->environmentTint));
		stream->read(reinterpret_cast<char*>(&this->toonTint), sizeof(this->toonTint));
	}

	void PmxFlipMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->morphIndex = ReadIndex(stream, settings.morphIndexSize);
		stream->read(reinterpret_cast<char*>(&this->influence), sizeof(this->influence));
	}

	void PmxImpulseMorph::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->rigidBodyIndex = ReadIndex(stream, settings.rigidBodyIndexSize);
		stream->read(reinterpret_cast<char*>(&this->localFlag), sizeof(this->localFlag));
		stream->read(reinterpret_cast<char*>(&this->movementSpeed), sizeof(this->movementSpeed));
		stream->read(reinterpret_cast<char*>(&this->rotationTorque), sizeof(this->rotationTorque));
	}

	void PmxMorphData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->nameLocal = ReadString(stream, settings.textEncoding);
		this->nameUniversal = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->panelType), sizeof(this->panelType));
		stream->read(reinterpret_cast<char*>(&this->morphType), sizeof(this->morphType));
		int count;
		stream->read(reinterpret_cast<char*>(&count), sizeof(count));
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
				throw "Unknown Morph type";
			}
			offsetData->Parse(stream, settings);
			this->dataList.emplace_back(std::move(offsetData));
		}
	}
}
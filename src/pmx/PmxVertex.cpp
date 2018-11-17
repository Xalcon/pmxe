#include "PmxVertex.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
	void PmxVertexSkinningBDEF1::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex = ReadIndex(stream, settings.boneIndexSize);
	}

	void PmxVertexSkinningBDEF2::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex1 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = ReadIndex(stream, settings.boneIndexSize);
		stream->read(reinterpret_cast<char*>(&this->bone1Weight), sizeof(this->bone1Weight));
	}

	void PmxVertexSkinningBDEF4::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex1 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex3 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex4 = ReadIndex(stream, settings.boneIndexSize);
		// read next 4 floats in a single batch
		stream->read(reinterpret_cast<char*>(&this->bone1Weight), sizeof(this->bone1Weight) * 4);
	}

	void PmxVertexSkinningSDEF::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex1 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = ReadIndex(stream, settings.boneIndexSize);
		// read next bytes in a single batch
		const int size = sizeof(this->bone1Weight) + sizeof(this->c) + sizeof(this->r0) + sizeof(this->r1);
		stream->read(reinterpret_cast<char*>(&this->bone1Weight), size);
	}

	void PmxVertexSkinningQDEF::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->boneIndex1 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex3 = ReadIndex(stream, settings.boneIndexSize);
		this->boneIndex4 = ReadIndex(stream, settings.boneIndexSize);
		// read next 4 floats in a single batch
		stream->read(reinterpret_cast<char*>(&this->bone1Weight), sizeof(this->bone1Weight) * 4);
	}

	void PmxVertex::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		int amountToRead = sizeof(this->position)
			+ sizeof(this->normal)
			+ sizeof(this->uv)
			+ settings.additionalVectors * sizeof(Vec4);
		stream->read(reinterpret_cast<char*>(&this->position), amountToRead);
		stream->read(reinterpret_cast<char*>(&this->skinningType), sizeof(this->skinningType));

		switch (this->skinningType)
		{
		case PmxVertexSkinningType::BDEF1:
			this->skinningInfo = std::make_unique<PmxVertexSkinningBDEF1>();
			break;
		case PmxVertexSkinningType::BDEF2:
			this->skinningInfo = std::make_unique<PmxVertexSkinningBDEF2>();
			break;
		case PmxVertexSkinningType::BDEF4:
			this->skinningInfo = std::make_unique<PmxVertexSkinningBDEF4>();
			break;
		case PmxVertexSkinningType::SDEF:
			this->skinningInfo = std::make_unique<PmxVertexSkinningSDEF>();
			break;
		case PmxVertexSkinningType::QDEF:
			this->skinningInfo = std::make_unique<PmxVertexSkinningQDEF>();
			break;
		default:
			throw "Invalid skinning type";
		}

		this->skinningInfo->Parse(stream, settings);
		stream->read(reinterpret_cast<char*>(&this->edgeScale), sizeof(this->edgeScale));
	}
}

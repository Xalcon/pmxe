#include "PmxVertex.hpp"
#include "PmxHelper.hpp"
#include "PmxException.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxVertexSkinningBDEF1::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex = readIndex(stream, settings.boneIndexSize);
	}

	void PmxVertexSkinningBDEF1::Save(std::ostream& stream, const PmxGlobalSettings settings)
	{
		writeIndex(stream, this->boneIndex, settings.boneIndexSize);
	}

	void PmxVertexSkinningBDEF2::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex1 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = readIndex(stream, settings.boneIndexSize);
		stream.read(reinterpret_cast<char*>(&this->bone1Weight), sizeof(this->bone1Weight));
	}

	void PmxVertexSkinningBDEF2::Save(std::ostream& stream, const PmxGlobalSettings settings)
	{
		writeIndex(stream, this->boneIndex1, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex2, settings.boneIndexSize);
		streamWrite(stream, this->bone1Weight);
	}

	void PmxVertexSkinningBDEF4::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex1 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex3 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex4 = readIndex(stream, settings.boneIndexSize);
		// read next 4 floats in a single batch
		stream.read(reinterpret_cast<char*>(&this->bone1Weight), sizeof(this->bone1Weight) * 4);
	}

	void PmxVertexSkinningBDEF4::Save(std::ostream& stream, const PmxGlobalSettings settings)
	{
		writeIndex(stream, this->boneIndex1, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex2, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex3, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex4, settings.boneIndexSize);
		streamWrite(stream, this->bone1Weight);
		streamWrite(stream, this->bone2Weight);
		streamWrite(stream, this->bone3Weight);
		streamWrite(stream, this->bone4Weight);
	}

	void PmxVertexSkinningSDEF::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex1 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = readIndex(stream, settings.boneIndexSize);
		// read next bytes in a single batch
		const int size = sizeof(this->bone1Weight) + sizeof(this->c) + sizeof(this->r0) + sizeof(this->r1);
		stream.read(reinterpret_cast<char*>(&this->bone1Weight), size);
	}

	void PmxVertexSkinningSDEF::Save(std::ostream& stream, const PmxGlobalSettings settings)
	{
		writeIndex(stream, this->boneIndex1, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex2, settings.boneIndexSize);
		streamWrite(stream, this->bone1Weight);
		streamWrite(stream, this->c);
		streamWrite(stream, this->r0);
		streamWrite(stream, this->r1);
	}

	void PmxVertexSkinningQDEF::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		this->boneIndex1 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex2 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex3 = readIndex(stream, settings.boneIndexSize);
		this->boneIndex4 = readIndex(stream, settings.boneIndexSize);
		// read next 4 floats in a single batch
		stream.read(reinterpret_cast<char*>(&this->bone1Weight), sizeof(this->bone1Weight) * 4);
	}

	void PmxVertexSkinningQDEF::Save(std::ostream& stream, const PmxGlobalSettings settings)
	{
		writeIndex(stream, this->boneIndex1, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex2, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex3, settings.boneIndexSize);
		writeIndex(stream, this->boneIndex4, settings.boneIndexSize);
		streamWrite(stream, this->bone1Weight);
		streamWrite(stream, this->bone2Weight);
		streamWrite(stream, this->bone3Weight);
		streamWrite(stream, this->bone4Weight);
	}

	void PmxVertex::Parse(std::istream& stream, const PmxGlobalSettings settings)
	{
		const int amountToRead = sizeof(this->position)
			+ sizeof(this->normal)
			+ sizeof(this->uv)
			+ settings.additionalVectors * sizeof(Vec4);
		stream.read(reinterpret_cast<char*>(&this->position), amountToRead);
		stream.read(reinterpret_cast<char*>(&this->skinningType), sizeof(this->skinningType));

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
			throw PMX_EXCEPTION("Invalid skinning type");
		}

		this->skinningInfo->Parse(stream, settings);
		stream.read(reinterpret_cast<char*>(&this->edgeScale), sizeof(this->edgeScale));
	}

	void PmxVertex::Save(std::ostream& stream, const PmxGlobalSettings settings) const
	{
		streamWrite(stream, this->position);
		streamWrite(stream, this->normal);
		streamWrite(stream, this->uv);
		for (auto i = 0; i < settings.additionalVectors; i++)
			streamWrite(stream, this->additionalData[i]);
		streamWrite(stream, this->skinningType);
		this->skinningInfo->Save(stream, settings);
		streamWrite(stream, this->edgeScale);
	}
}

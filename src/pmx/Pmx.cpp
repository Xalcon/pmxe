#include "Pmx.hpp"
#include <codecvt> 
#include "../Cxx14Polyfill.hpp"
#include "../Helper.hpp"

// Apparently Microsoft forgot to define a symbol for codecvt.
// Works with /MT only
#include <locale>

#if _MSC_VER >= 1900

std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
	return convert.to_bytes(p, p + utf16_string.size());
}

#else

std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(utf16_string);
}

#endif

namespace Vitriol
{
    int32_t ReadIndex(std::istream* stream, uint8_t typeSize)
    {
        uint32_t tmp;
        stream->read(reinterpret_cast<char*>(&tmp), typeSize);

        // This is wrong for type4 vertices, since vertices have a upper limit of UINT32_MAX
        // but we treat them like the others with an upper limit of INT32_MAX
        // this means our parser can only properly parse models with less than 2 million vertices
        // ¯\_(ツ)_/¯
        uint32_t upperLimit = (2<<(6+8*(typeSize-1)))-1;
        return Limit(tmp, 0u, upperLimit, -1u);
    }

    std::string ReadString(std::istream* stream, PmxStringEncoding sourceEncoding)
    {
        uint32_t size;
        stream->read((char*)&size, sizeof(size));
        if(size == 0) return "";

        if(sourceEncoding == PmxStringEncoding::UTF16_LE)
        {
            if(size % 2 != 0)
                throw "Invalid size!";

            std::vector<char16_t> buffer;
            buffer.resize(size);
            stream->read((char*)buffer.data(), size);

            std::u16string source(buffer.begin(), buffer.end());
            //std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
            //std::string dst = convert.to_bytes(buffer.data());
			std::string dst = utf16_to_utf8(source);
            return dst;
        }
        else if(sourceEncoding == PmxStringEncoding::UTF8)
        {
            std::vector<char> buffer;
            buffer.resize(size);

            stream->read((char*)buffer.data(), size);
            return std::string(buffer.begin(), buffer.end());
        }

        return "INVALID_ENCODING";
    }

    void PmxGlobalSettings::Parse(std::istream* stream)
    {
        uint8_t size;
        stream->read(reinterpret_cast<char*>(&size), sizeof(size));
        stream->read(reinterpret_cast<char*>(this), sizeof(uint8_t) * size);
        if(size > 8)
            printf("WARN: PMX Global Configuration contains more than 8 configurations! Data will be missing on save! Found %d configs\n", size);
    }

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
                            + settings.additionalVectors * sizeof(vec4);
        stream->read(reinterpret_cast<char*>(&this->position), amountToRead);
        stream->read(reinterpret_cast<char*>(&this->skinningType), sizeof(this->skinningType));

        switch(this->skinningType)
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

    void PmxMaterial::Parse(std::istream* stream, PmxGlobalSettings settings)
    {
        this->localName = ReadString(stream, settings.textEncoding);
        this->universalName = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->diffuseColor), sizeof(this->diffuseColor));
		stream->read(reinterpret_cast<char*>(&this->specularColor), sizeof(this->specularColor));
		stream->read(reinterpret_cast<char*>(&this->specularStrength), sizeof(this->specularStrength));
		stream->read(reinterpret_cast<char*>(&this->ambientColor), sizeof(this->ambientColor));
		stream->read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));
		stream->read(reinterpret_cast<char*>(&this->edgeColor), sizeof(this->edgeColor));
		stream->read(reinterpret_cast<char*>(&this->edgeScale), sizeof(this->edgeScale));
		stream->read(reinterpret_cast<char*>(&this->textureIndex), settings.textureIndexSize);
		stream->read(reinterpret_cast<char*>(&this->environmentIndex), settings.textureIndexSize);
		stream->read(reinterpret_cast<char*>(&this->environmentBlendMode), sizeof(this->environmentBlendMode));
		stream->read(reinterpret_cast<char*>(&this->toonReference), sizeof(this->toonReference));

        int toonValueSize = this->toonReference == 1 ? sizeof(uint8_t) : settings.textureIndexSize;
        stream->read(reinterpret_cast<char*>(&this->toonValue), toonValueSize);
		this->note = ReadString(stream, settings.textEncoding);
        stream->read(reinterpret_cast<char*>(&this->surfaceCount), sizeof(this->surfaceCount));
    }

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
        if(this->hasLimits)
            this->angleLimits.Parse(stream, settings);
    }

    void PmxBoneIk::Parse(std::istream* stream, PmxGlobalSettings settings)
    {
        this->targetIndex = ReadIndex(stream, settings.boneIndexSize);
        int size = sizeof(this->loopCount) + sizeof(this->limitRadians) + sizeof(this->linkCount);
        stream->read(reinterpret_cast<char*>(&this->loopCount), size);
        this->ikLinks.reserve(size);
        for(auto i = 0; i < this->linkCount; i++)
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

        if(static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::IndexedTailPosition))
            stream->read(reinterpret_cast<char*>(&this->tailPositionBoneIndex), settings.boneIndexSize);
        else
            stream->read(reinterpret_cast<char*>(&this->tailPosition), sizeof(this->tailPosition));

        if(static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritRotation) || static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InheritTranslation))
            this->inheritBone.Parse(stream, settings);

        if(static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::FixedAxis))
            this->fixedAxis.Parse(stream, settings);

        if(static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::LocalCoordinate))
            this->localCoordinate.Parse(stream, settings);

        if(static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::ExternalParentDeform))
            this->externalParent.Parse(stream, settings);

        if(static_cast<int>(this->flags) & static_cast<int>(PmxBoneFlags::InverseKinematics))
            this->ikBoneData.Parse(stream, settings);
    }

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
		stream->read(reinterpret_cast<char*>(&this->operation), sizeof(this->operation));
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
		auto o = stream->tellg();
        this->morphNameLocal = ReadString(stream, settings.textEncoding);
        this->morphNameUniversal = ReadString(stream, settings.textEncoding);
        stream->read(reinterpret_cast<char*>(&this->panelType), sizeof(this->panelType));
        stream->read(reinterpret_cast<char*>(&this->morphType), sizeof(this->morphType));
        int count;
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->dataList.reserve(count);
        for(auto i = 0; i < count; i++)
        {
            std::unique_ptr<PmxMorphOffsetData> offsetData;
            switch(this->morphType)
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

    void Pmx::Parse(std::istream* stream)
    {
        stream->read(this->magic, sizeof(this->magic));

        if(magic[0] != 'P' || magic[1] != 'M' || magic[2] != 'X' || magic[3] != ' ')
            printf("WARN: Unexpected PMX header: %.*s\n", 4, magic);

        stream->read(reinterpret_cast<char*>(&this->version), sizeof(float));

        this->globalSettings.Parse(stream);
        this->nameLocal = ReadString(stream, this->globalSettings.textEncoding);
        this->nameUniversal = ReadString(stream, this->globalSettings.textEncoding);
        this->commentLocal = ReadString(stream, this->globalSettings.textEncoding);
        this->commentUniversal = ReadString(stream, this->globalSettings.textEncoding);

        uint32_t count;

        // read vertices
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->vertices.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxVertex vertex;
            vertex.Parse(stream, this->globalSettings);
            this->vertices.emplace_back(std::move(vertex));
        }

        // read faces
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->vertexIndices.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            uint32_t index;
            stream->read(reinterpret_cast<char*>(&index), this->globalSettings.vertexIndexSize);
            this->vertexIndices.emplace_back(index);
        }

        // read textures
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->textures.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            std::string texture = ReadString(stream, this->globalSettings.textEncoding);
            this->textures.emplace_back(texture);
        }

        // read materials
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->materials.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxMaterial material;
            material.Parse(stream, this->globalSettings);
            this->materials.emplace_back(material);
        }

        // read bones
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->bones.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxBoneData bone;
            bone.Parse(stream, this->globalSettings);
            this->bones.emplace_back(bone);
        }

        // read morphs
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        this->morphs.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxMorphData morph;
            morph.Parse(stream, this->globalSettings);
            this->morphs.emplace_back(std::move(morph));
        }
    }
}

#include "Pmx.hpp"
#include <codecvt> 
#include "../Cxx14Polyfill.hpp"
#include "../Helper.hpp"

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

        if(sourceEncoding == PmxStringEncoding::UTF16_LE)
        {
            if(size % 2 != 0)
                throw "Invalid size!";

            std::vector<char16_t> buffer;
            buffer.resize(size);
            stream->read((char*)buffer.data(), size);

            std::u16string source(buffer.begin(), buffer.end());
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
            std::string dst = convert.to_bytes(buffer.data());
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
        this->Vertices.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxVertex vertex;
            vertex.Parse(stream, this->globalSettings);
            this->Vertices.emplace_back(std::move(vertex));
        }

        // read faces
        stream->read(reinterpret_cast<char*>(&count), sizeof(count));
        printf("Reserved %d entries for indices\n", count);
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
    }
}

#include "Pmx.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
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
		for (size_t i = 0; i < count; i++)
		{
			PmxMorphData morph;
			morph.Parse(stream, this->globalSettings);
			this->morphs.emplace_back(std::move(morph));
		}

		// read displayPanes
		stream->read(reinterpret_cast<char*>(&count), sizeof(count));
		this->displayPanes.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			PmxDisplayData display;
			display.Parse(stream, this->globalSettings);
			this->displayPanes.emplace_back(std::move(display));
		}

		// read rigid bodies
		stream->read(reinterpret_cast<char*>(&count), sizeof(count));
		this->rigidBodies.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			PmxRigidBodyData rigidBody;
			rigidBody.Parse(stream, this->globalSettings);
			this->rigidBodies.emplace_back(rigidBody);
		}

		// read joints
		stream->read(reinterpret_cast<char*>(&count), sizeof(count));
		this->joints.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			PmxJointData joint;
			joint.Parse(stream, this->globalSettings);
			this->joints.emplace_back(joint);
		}

		if(version > 2.0)
		{
			// read softbody data
			stream->read(reinterpret_cast<char*>(&count), sizeof(count));
			this->softbodyData.reserve(count);
			for (size_t i = 0; i < count; i++)
			{
				PmxSoftBodyData softbody;
				softbody.Parse(stream, this->globalSettings);
				this->softbodyData.emplace_back(softbody);
			}
		}
    }
}

#include "Pmx.hpp"
#include "PmxHelper.hpp"
#include "PmxException.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
    void Pmx::Parse(std::istream& stream)
    {
        stream.read(this->magic, sizeof(this->magic));

        if(magic[0] != 'P' || magic[1] != 'M' || magic[2] != 'X' || magic[3] != ' ')
            printf("WARN: Unexpected PMX header: %.*s\n", 4, magic);

        stream.read(reinterpret_cast<char*>(&this->version), sizeof(float));

        this->globalSettings.Parse(stream);
        this->nameLocal = readString(stream, this->globalSettings.textEncoding);
        this->nameUniversal = readString(stream, this->globalSettings.textEncoding);
        this->commentLocal = readString(stream, this->globalSettings.textEncoding);
        this->commentUniversal = readString(stream, this->globalSettings.textEncoding);

        uint32_t count;

        // read vertices
        stream.read(reinterpret_cast<char*>(&count), sizeof(count));
        this->vertices.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxVertex vertex;
            vertex.Parse(stream, this->globalSettings);
            this->vertices.emplace_back(std::move(vertex));
        }

        // read faces
        stream.read(reinterpret_cast<char*>(&count), sizeof(count));
        this->vertexIndices.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            uint32_t index = 0;
            stream.read(reinterpret_cast<char*>(&index), this->globalSettings.vertexIndexSize);
            this->vertexIndices.emplace_back(index);
        }

        // read textures
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
        this->textures.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
	        auto texture = readString(stream, this->globalSettings.textEncoding);
            this->textures.emplace_back(texture);
        }

        // read materials
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
        this->materials.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxMaterial material;
            material.Parse(stream, this->globalSettings);
            this->materials.emplace_back(material);
        }

        // read bones
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
        this->bones.reserve(count);
        for(size_t i = 0; i < count; i++)
        {
            PmxBoneData bone;
            bone.Parse(stream, this->globalSettings);
            this->bones.emplace_back(bone);
        }

		// read morphs
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
		this->morphs.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			PmxMorphData morph;
			morph.Parse(stream, this->globalSettings);
			this->morphs.emplace_back(std::move(morph));
		}

		// read displayPanes
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
		this->displayPanes.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			PmxDisplayData display;
			display.Parse(stream, this->globalSettings);
			this->displayPanes.emplace_back(std::move(display));
		}

		// read rigid bodies
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
		this->rigidBodies.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			PmxRigidBodyData rigidBody;
			rigidBody.Parse(stream, this->globalSettings);
			this->rigidBodies.emplace_back(rigidBody);
		}

		// read joints
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
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
			stream.read(reinterpret_cast<char*>(&count), sizeof(count));
			this->softbodyData.reserve(count);
			for (size_t i = 0; i < count; i++)
			{
				PmxSoftBodyData softbody;
				softbody.Parse(stream, this->globalSettings);
				this->softbodyData.emplace_back(softbody);
			}
		}
    }

    void Pmx::Save(std::ostream& stream)
    {
        streamWrite(stream, "PMX ", 4);
        streamWrite(stream, this->version);

		this->globalSettings.Save(stream);
		writeString(stream, this->nameLocal, this->globalSettings.textEncoding);
		writeString(stream, this->nameUniversal, this->globalSettings.textEncoding);
		writeString(stream, this->commentLocal, this->globalSettings.textEncoding);
		writeString(stream, this->commentUniversal, this->globalSettings.textEncoding);

		streamWrite(stream, static_cast<int32_t>(this->vertices.size()));
		for (auto& vertex : this->vertices)
			vertex.Save(stream, this->globalSettings);

		streamWrite(stream, static_cast<int32_t>(this->vertexIndices.size()));
		for (auto& index : this->vertexIndices)
			writeIndex(stream, index, this->globalSettings.vertexIndexSize);

		streamWrite(stream, static_cast<int32_t>(this->textures.size()));
		for (auto& texture : this->textures)
			writeString(stream, texture, this->globalSettings.textEncoding);

		streamWrite(stream, static_cast<int32_t>(this->materials.size()));
		for (auto& material : this->materials)
			material.Save(stream, this->globalSettings);

		streamWrite(stream, static_cast<int32_t>(this->bones.size()));
		for (auto& bone : this->bones)
			bone.Save(stream, this->globalSettings);

		streamWrite(stream, static_cast<int32_t>(this->morphs.size()));
		for (auto& morph : this->morphs)
			morph.Save(stream, this->globalSettings);

		streamWrite(stream, static_cast<int32_t>(this->displayPanes.size()));
		for (auto& displayPane : this->displayPanes)
			displayPane.Save(stream, this->globalSettings);

		streamWrite(stream, static_cast<int32_t>(this->rigidBodies.size()));
		for (auto& rigidBody : this->rigidBodies)
			rigidBody.Save(stream, this->globalSettings);

		streamWrite(stream, static_cast<int32_t>(this->joints.size()));
		for (auto& joint : this->joints)
			joint.Save(stream, this->globalSettings);

		if(this->version > 2)
		{
			streamWrite(stream, static_cast<int32_t>(this->softbodyData.size()));
			for (auto& softBody : this->softbodyData)
				softBody.Save(stream, this->globalSettings);
		}
    }
}

#pragma once
#include <vector>
#include <string>
#include "PmxTypes.hpp"
#include "PmxGlobalSettings.hpp"
#include "PmxVertex.hpp"
#include "PmxMaterial.hpp"
#include "PmxBone.hpp"
#include "PmxMorph.hpp"
#include "PmxDisplayPane.hpp"
#include "PmxRigidBody.hpp"
#include "PmxJoints.hpp"
#include "PmxSoftBody.hpp"

namespace vitriol
{

    /*class IPmxParsable
    {
        public:
            virtual ~IPmxParsable() {}
            virtual void Parse(std::istream *stream, PmxGlobalSettings settings) = 0;
    };*/


    class Pmx
    {
        public:
            char magic[4];
            float version;
            PmxGlobalSettings globalSettings;
            std::string nameLocal;
            std::string nameUniversal;
            std::string commentLocal;
            std::string commentUniversal;

            std::vector<PmxVertex> vertices;
            std::vector<uint32_t> vertexIndices;
            std::vector<std::string> textures;
            std::vector<PmxMaterial> materials;
            std::vector<PmxBoneData> bones;
            std::vector<PmxMorphData> morphs;
			std::vector<PmxDisplayData> displayPanes;
			std::vector<PmxRigidBodyData> rigidBodies;
			std::vector<PmxJointData> joints;
			std::vector<PmxSoftBodyData> softbodyData;

            void Parse(std::istream* stream);
            void Save(std::ostream& stream);
    };
}

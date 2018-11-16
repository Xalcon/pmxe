#ifndef __VITRIOL_PMX_H__
#define __VITRIOL_PMX_H__
#include <vector>
#include <memory>
#include <string>
#include <istream>

namespace Vitriol
{
    enum class PmxStringEncoding : uint8_t
    {
        UTF16_LE = 0,
        UTF8 = 1
    };

    typedef struct vec2
    {
        float x;
        float y;
    } vec2;

    typedef struct vec3
    {
        float x;
        float y;
        float z;
    } vec3;

    typedef struct vec4
    {
        float x;
        float y;
        float z;
        float w;
    } vec4;

	class PmxGlobalSettings
    {
        public:
            PmxGlobalSettings() {}

			PmxStringEncoding textEncoding;
			uint8_t additionalVectors;
            
			uint8_t vertexIndexSize;
			uint8_t textureIndexSize;
			uint8_t materialIndexSize;
			uint8_t boneIndexSize;
			uint8_t morphIndexSize;
			uint8_t rigidBodyIndexSize;

			void Parse(std::istream *stream);
    };

    enum class PmxVertexSkinningType : uint8_t
    {
		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF = 3,
		QDEF = 4,
    };

    class PmxVertexSkinning
    {
        public:
            virtual void Parse(std::istream* stream, PmxGlobalSettings settings) = 0;
            virtual ~PmxVertexSkinning() {}
    };

    class PmxVertexSkinningBDEF1 : public PmxVertexSkinning
    {
        public:
            int32_t boneIndex;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxVertexSkinningBDEF2 : public PmxVertexSkinning
    {
        public:
            int32_t boneIndex1;
            int32_t boneIndex2;
            // Bone 2 weight = 1.0 - Bone 1 weight
            float bone1Weight;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxVertexSkinningBDEF4 : public PmxVertexSkinning
    {
        public:
            int32_t boneIndex1;
            int32_t boneIndex2;
            int32_t boneIndex3;
            int32_t boneIndex4;
            // total bone weight not guaranteed to be 1.0
            float bone1Weight;
            float bone2Weight;
            float bone3Weight;
            float bone4Weight;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    // Spherical Deform Blending
    class PmxVertexSkinningSDEF : public PmxVertexSkinning
    {
        public:
            int32_t boneIndex1;
            int32_t boneIndex2;
            float bone1Weight;
            vec3 c;
            vec3 r0;
            vec3 r1;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    // Dual quaternion deform blending, v2.1+
    class PmxVertexSkinningQDEF : public PmxVertexSkinning
    {
        public:
            int32_t boneIndex1;
            int32_t boneIndex2;
            int32_t boneIndex3;
            int32_t boneIndex4;
            // total bone weight not guaranteed to be 1.0
            float bone1Weight;
            float bone2Weight;
            float bone3Weight;
            float bone4Weight;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxVertex
    {
        public:
            vec3 position;
            vec3 normal;
            vec2 uv;
            vec4 additionalData[4];
            PmxVertexSkinningType skinningType;
            std::unique_ptr<PmxVertexSkinning> skinningInfo;
            float edgeScale;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

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

            std::vector<PmxVertex> Vertices;
            std::vector<uint32_t> vertexIndices;
            std::vector<std::string> textures;

            void Parse(std::istream* stream);
            void Save(std::ostream* stream);
    };
    
    std::string ReadString(std::istream* stream, PmxStringEncoding sourceEncoding);
}

#endif
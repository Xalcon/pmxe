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

    enum class PmxMaterialFlags : uint8_t
    {
        // 2.0 features
        NoCull = 1 << 0, // Disables back-face culling
        GroundShadow = 1 << 1, // Projects a shadow onto the geometry
        DrawShadow = 1 << 2, // Renders to the shadow map
        ReceiveShadow = 1 << 3, // Receives a shadow from the shadow map
        HasEdge = 1 << 4, // Has "pencil" outline
        // V2.1 features
        VertexColor = 1 << 5, // Uses additional vec4 for vertex color
        PointDrawing = 1 << 6, // Each of the 3 vertices are points
        LineDrawing = 1 << 7, // The triangle is rendered as lines (wireframe?)
    };

    enum class PmxMaterialEnvironmentBlendMode : uint8_t
    {
        Disabled = 0,
        Multiply = 1,
        Additive = 2,
        // Environment blend mode 3 will use the first additional vec4 to map the environment texture,
        // using just the X and Y values as the texture UV. It is mapped as an additional texture layer.
        // This may conflict with other uses for the first additional vec4.
        AdditionalVec4 = 3
    };

    class PmxMaterial
    {
    public:
	    PmxMaterial() : textureIndex(0), environmentIndex(0), toonValue(0), surfaceCount(0) {};
	    std::string localName; // Handy name for the material (Usually Japanese)
        std::string universalName; // Handy name for the material (Usually English)
        vec4 diffuseColor; // RGBA colour (Alpha will set a semi-transparent material)
        vec3 specularColor; // RGB colour of the reflected light
        float specularStrength; // 	The "size" of the specular highlight
        vec3 ambientColor; // RGB colour of the material shadow (When out of light)
        PmxMaterialFlags flags; // see material flags
        vec4 edgeColor; // RGBA colour of the pencil-outline edge (Alpha for semi-transparent)
        float edgeScale; // Pencil-outline scale (1.0 should be around 1 pixel)
        int32_t textureIndex; // See Index Types, this is from the texture table by default
        int32_t environmentIndex; // Same as texture index, but for environment mapping
        PmxMaterialEnvironmentBlendMode environmentBlendMode; // 0 = disabled, 1 = multiply, 2 = additive, 3 = additional vec4*
        uint8_t toonReference; // 0 = Texture reference, 1 = internal reference
        int32_t toonValue; // Toon value will be a texture index much like the standard texture and environment texture indexes
                           // unless the Toon reference byte is equal to 1, in which case Toon value will be a byte that references
                           // a set of 10 internal toon textures (Most implementations will use "toon01.bmp" to "toon10.bmp" as the
                           // internal textures).
		std::string note; // memo or meta; also called note in PMX Editor; This is used for scripting or additional data
        int32_t surfaceCount; // Surface count will always be a multiple of 3. It is based on the offset of the previous material
                              // through to the size of the current material. If you add up all the surface counts for all materials
                              // you should end up with the total number of surfaces.

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
            std::vector<PmxMaterial> materials;

            void Parse(std::istream* stream);
            void Save(std::ostream* stream);
    };
    
    std::string ReadString(std::istream* stream, PmxStringEncoding sourceEncoding);
}

#endif
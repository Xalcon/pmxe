#pragma once
#include <cstdint>
#include <string>
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
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

	class PmxMaterial// : private IPmxParsable
	{
	public:
		PmxMaterial() : textureIndex(0), environmentIndex(0), toonValue(0), surfaceCount(0) {}
		
		std::string nameLocal; // Handy name for the material (Usually Japanese)
		std::string nameUniversal; // Handy name for the material (Usually English)
		Vec4 diffuseColor; // RGBA colour (Alpha will set a semi-transparent material)
		Vec3 specularColor; // RGB colour of the reflected light
		float specularStrength; // 	The "size" of the specular highlight
		Vec3 ambientColor; // RGB colour of the material shadow (When out of light)
		PmxMaterialFlags flags; // see material flags
		Vec4 edgeColor; // RGBA colour of the pencil-outline edge (Alpha for semi-transparent)
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

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings globalSettings);
	};
}

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

    /*class IPmxParsable
    {
        public:
            virtual ~IPmxParsable() {}
            virtual void Parse(std::istream *stream, PmxGlobalSettings settings) = 0;
    };*/

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

    class PmxVertex// : private IPmxParsable
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

    class PmxMaterial// : private IPmxParsable
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

    enum class PmxBoneFlags : uint16_t
    {
        IndexedTailPosition = 1 << 0, // Is the tail position a vec3 or bone index	
        Rotatable = 1 << 1, // Enables rotation
        Translatable = 1 << 2, // Is the tail position a vec3 or bone index	
        IsVisible = 1 << 3, // ???
        Enabled = 1 << 4, // ???
        InverseKinematics = 1 << 5, // Use inverse kinematics (physics)	
        InheritRotation = 1 << 8, // Rotation inherits from another bone
        InheritTranslation = 1 << 9, // Translation inherits from another bone
        FixedAxis = 1 << 10, // The bone's shaft is fixed in a direction
        LocalCoordinate = 1 << 11, // ???
        PhysicsAfterDeform = 1 << 12, // ???
        ExternalParentDeform = 1 << 13, // ???
    };

    class PmxParentBone
    {
        public:
            int32_t parentIndex;
            float parentInfluence;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneFixedAxis
    {
        public:
            vec3 axisDirection;
        
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneLocalCoordinate
    {
        public:
            vec3 x;
            vec3 z;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneExternalParent
    {
        public:
            int32_t parentIndex;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneIkAngleLimit
    {
        public:
            vec3 min; // minimum angle in radians
            vec3 max; // maximum angle in radians
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneIkLinks
    {
        public:
            int32_t boneIndex;
            uint8_t hasLimits; // When equal to 1, use angle limits
            PmxBoneIkAngleLimit angleLimits; // Used if has limits is 1. See IK Angle Limit
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneIk
    {
        public:
            int32_t targetIndex;
            int32_t loopCount;
            float limitRadians;
            int32_t linkCount; // How many bones this IK links with
            std::vector<PmxBoneIkLinks> ikLinks;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneData// : private IPmxParsable
    {
        public:
            std::string boneNameLocal;
            std::string boneNameUniversal;
            vec3 position; // The local translation of the bone
            int32_t parentBoneIndex;
            int32_t layer; // deformation hierarchy
            PmxBoneFlags flags;
            union { vec3 tailPosition; int32_t tailPositionBoneIndex; };
            PmxParentBone inheritBone;
            PmxBoneFixedAxis fixedAxis;
            PmxBoneLocalCoordinate localCoordinate;
            PmxBoneExternalParent externalParent;
            PmxBoneIk ikBoneData;
            
            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    enum class PmxMorphType : uint8_t
    {
        Group = 0,
        Vertex = 1,
        Bone = 2,
        UV = 3,
        UVExt1 = 4,
        UVExt2 = 5,
        UVExt3 = 6,
        UVExt4 = 7,
        Material = 8,
        // 2.1
        Flip = 9,
        Impulse = 10
    };

    enum class PmxMorphPanelType : uint8_t
    {
        Hidden = 0, // None
        Eyebrows = 1, // Bottom Left
        Eyes = 2, // Top Left
        Mouth = 3, // Top right
        Other = 4 // Bottom right
    };

    class PmxMorphOffsetData
    {
        public:
            virtual ~PmxMorphOffsetData() {}
            virtual void Parse(std::istream* stream, PmxGlobalSettings settings) = 0;
    };

    class PmxGroupMorph : public PmxMorphOffsetData
    {
        public:
            int32_t morphIndex;
            float influence;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxVertexMorph : public PmxMorphOffsetData
    {
        public:
            int32_t vertexIndex;
            vec3 translation;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxBoneMorph : public PmxMorphOffsetData
    {
        public:
            int32_t boneIndex;
            vec3 translation;
            vec4 rotation;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxUVMorph : public PmxMorphOffsetData
    {
        public:
            int32_t vertexIndex;
            vec4 data;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxMaterialMorph : public PmxMorphOffsetData
    {
        public:
            int32_t materialIndex;
            uint8_t operation;
            vec4 diffuse;
            vec3 specular;
            float specularStrength;
            vec3 ambient;
            vec4 edgeColor;
            float edgeSize;
            vec4 textureTint;
            vec4 environmentTint;
            vec4 toonTint;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxFlipMorph : public PmxMorphOffsetData
    {
        public:
            int32_t morphIndex;
            float influence;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxImpulseMorph : public PmxMorphOffsetData
    {
        public:
            int32_t rigidBodyIndex;
            uint32_t localFlag;
            vec3 movementSpeed;
            vec3 rotationTorque;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

    class PmxMorphData// : private IPmxParsable
    {
        public:
            std::string morphNameLocal;
            std::string morphNameUniversal;
            PmxMorphPanelType panelType;
            PmxMorphType morphType;
            std::vector<std::unique_ptr<PmxMorphOffsetData>> dataList;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

	enum class PmxDisplayFrameType : uint8_t
	{
		Bone = 0,
		Morph = 1,
	};

	class PmxFrameData
	{
	public:
		PmxDisplayFrameType frameType;
		int32_t index;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxDisplayData
	{
	public:
		std::string displayNameLocal;
		std::string displayNameUniversal;
		uint8_t isSpecialFrame;
		//int32_t frameCount;
		std::vector<PmxFrameData> frames;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

    enum class PmxRigidBodyShapeType : uint8_t
    {
        Sphere = 0,
        Box = 1,
        Capsule = 2
    };

    enum class PmxPhysicsMode : uint8_t
    {
        FollowBone = 0,
        Physics = 1,
        PhysicsAndBone = 2
    };

    class PmxRigidBodyData
    {
        public:
            std::string nameLocal;
            std::string nameUniversal;
            int32_t boneIndex;
            uint8_t groupId;
            uint16_t nonCollisionMask;
            PmxRigidBodyShapeType shapeType;
            vec3 shapeSize;
            vec3 shapePosition;
            vec3 shapeRotation;
            float mass;
            float moveAttenuation;
            float rotationDampening;
            float repulsion;
            float frictionForce;
            PmxPhysicsMode physicsMode;

            void Parse(std::istream* stream, PmxGlobalSettings settings);
    };

	enum class PmxJointType : uint8_t
	{
		Spring6DOF = 0,
		_6DOF = 1,
		P2P = 2,
		ConeTwist = 3,
		Slider = 4,
		Hinge = 5
	};

	class PmxJointData
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		PmxJointType type;
		int32_t rigidBodyIndexA;
		int32_t rigidBodyIndexB;
		vec3 position;
		vec3 rotation;
		vec3 positionMin;
		vec3 positionMax;
		vec3 rotationMin;
		vec3 rotationMax;
		vec3 positionSpring;
		vec3 rotationSpring;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	enum class PmxSoftBodyShapeType : uint8_t
	{
		TriMesh = 0,
		Rope = 1
	};

	enum class PmxSoftBodyFlags : uint8_t
	{
		BLink = 0,
		ClusterCreation = 1,
		LinkCrossing = 2
	};

	enum class PmxSoftBodyAeroDynamicsModel : int32_t
	{
		VPoint = 0,
		VPointTwoSided = 1,
		VPointOneSided = 2,
		TPointTwoSided = 3,
		TPointOneSided = 4,
	};

	class PmxSoftBodyAnchorRigidBody
	{
	public:
		int32_t rigidBodyIndex;
		int32_t vertexIndex;
		uint8_t nearMode;
		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxSoftBodyVertexPin
	{
	public:
		int32_t vertexIndex;
		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	typedef struct PmxSoftBodyConfig
	{
        float cfgVCF; // Velocities correction factor (Baumgarte)
        float cfgDP; // Damping coefficient
        float cfgDG; // Drag coefficient
        float cfgLF; // Lift coefficient
        float cfgPR; // Pressure coefficient
        float cfgVC; // Volume conversation coefficient
        float cfgDF; // Dynamic friction coefficient
        float cfgMT; // Pose matching coefficient
        float cfgCHR; // Rigid contacts hardness
        float cfgKHR; // Kinetic contacts hardness
        float cfgSHR; // Soft contacts hardness
        float cfgAHR; // Anchors hardness
        float clusterSRHR_CL; // Soft vs rigid hardness
        float clusterSKHR_CL; // Soft vs kinetic hardness
        float clusterSSHR_CL; // Soft vs soft hardness
        float clusterSR_SPLT_CL; // Soft vs rigid impulse split
        float clusterSK_SPLT_CL; // Soft vs kinetic impulse split
        float clusterSS_SPLT_CL; // Soft vs soft impulse split
        int iterationV_IT; // Velocities solver iterations
        int iterationP_IT; // Positions solver iterations
        int iterationD_IT; // Drift solver iterations
        int iterationC_IT; // Cluster solver iterations
        int materialLST; // Linear stiffness coefficient
        int materialAST; // Area / Angular stiffness coefficient
        int materialVST; // Volume stiffness coefficient
	} PmxSoftBodyConfig;

	class PmxSoftBodyData
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		PmxSoftBodyShapeType shapeType;
		int32_t materialIndex;
		uint8_t group;
		uint16_t noCollisionMask;
		PmxSoftBodyFlags flags;
		int32_t bLinkCreateDistance;
		int32_t nClusters;
		float totalMass;
		float collisionMargin;
		PmxSoftBodyAeroDynamicsModel aeroModel;
		PmxSoftBodyConfig config;
		std::vector<PmxSoftBodyAnchorRigidBody> anchorRigidBodies;
		std::vector<PmxSoftBodyVertexPin> vertexPins;

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
            void Save(std::ostream* stream);
    };
    
    std::string ReadString(std::istream* stream, PmxStringEncoding sourceEncoding);
}

#endif
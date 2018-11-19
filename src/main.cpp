#include <cstdio>
#include <iostream>
#include <fstream>
#include "pmx/Pmx.hpp"
#include "pmx/PmxException.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
	
    EMSCRIPTEN_KEEPALIVE int ProcessFile(const char* filePath)
    {
        std::ifstream file;
        file.open(filePath, std::ifstream::in | std::ifstream::binary);

        file.seekg(0, std::ios::end);
        const uint64_t totalFileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        vitriol::Pmx pmx;
		try
		{
			pmx.Parse(&file);
		}
		catch(vitriol::PmxException& e)
		{
			std::cout << "Error in " << e.getFile() << ":" << e.getLine() << " @ " << e.getFunc() << std::endl;
			std::cout << e.what() << std::endl;
			return 1;
		}
		catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
			return 1;
		}
        

        const uint64_t len = file.tellg();
        file.close();

        printf("Read %llu bytes from %s (Total size: %llu bytes)\n", len, filePath, totalFileSize);
		std::cout << "Read" << len << "bytes from " << filePath << "(Total size: " << totalFileSize << "bytes)" << std::endl;
        printf("Magic: '%.*s'\n", 4, pmx.magic);
        printf("Version: %.1f\n", pmx.version);
        printf(" - TextEncoding: %hhu\n", pmx.globalSettings.textEncoding);
        printf(" - AdditionalVec4: %hhu\n", pmx.globalSettings.additionalVectors);
        printf(" - vertexIndexSize: %hhu\n", pmx.globalSettings.vertexIndexSize);
        printf(" - textureIndexSize: %hhu\n", pmx.globalSettings.textureIndexSize);
        printf(" - materialIndexSize: %hhu\n", pmx.globalSettings.materialIndexSize);
        printf(" - boneIndexSize: %hhu\n", pmx.globalSettings.boneIndexSize);
        printf(" - morphIndexSize: %hhu\n", pmx.globalSettings.morphIndexSize);
        printf(" - rigidBodyIndexSize: %hhu\n", pmx.globalSettings.rigidBodyIndexSize);
        printf("LocalName: %s\n", pmx.nameLocal.c_str());
        printf("UniversalName: %s\n", pmx.nameUniversal.c_str());
        printf("LocalComment: %s\n", pmx.commentLocal.c_str());
        printf("UniversalComment: %s\n", pmx.commentUniversal.c_str());
        printf("Vertices: %lu\n", pmx.vertices.size());
        int bwt[5];
        for (auto& vertex : pmx.vertices)
	        bwt[static_cast<int>(vertex.skinningType)]++;
        printf("  BDEF1: %d\n  BDEF2: %d\n  BDEF4: %d\n  SDEF: %d\n  QDEF: %d\n", bwt[0], bwt[1], bwt[2], bwt[3], bwt[4]);
        printf("Polygons: %lu\n", pmx.vertexIndices.size() / 3);
		
        printf("Textures(%lu): \n", pmx.textures.size());
        for(size_t i = 0; i < pmx.textures.size(); i++)
            printf("  %lu: %s\n", i, pmx.textures[i].c_str());

        printf("Materials(%lu): \n", pmx.materials.size());
        for(size_t i = 0; i < pmx.materials.size(); i++)
            printf("  %lu: %s\n", i, pmx.materials[i].nameLocal.c_str());

        printf("Bones(%lu): \n", pmx.bones.size());
        for(size_t i = 0; i < pmx.bones.size(); i++)
            printf("  %lu: %s\n", i, pmx.bones[i].nameLocal.c_str());

		printf("Morphs(%lu): \n", pmx.morphs.size());
		for (size_t i = 0; i < pmx.morphs.size(); i++)
			printf("  %lu: %s\n", i, pmx.morphs[i].nameLocal.c_str());

		printf("DisplayPanes(%lu): \n", pmx.displayPanes.size());
		for (size_t i = 0; i < pmx.displayPanes.size(); i++)
			printf("  %lu: %s\n", i, pmx.displayPanes[i].nameLocal.c_str());

		printf("RigidBodies(%lu): \n", pmx.rigidBodies.size());
		for (size_t i = 0; i < pmx.rigidBodies.size(); i++)
			printf("  %lu: %s\n", i, pmx.rigidBodies[i].nameLocal.c_str());

		printf("Joints(%lu): \n", pmx.joints.size());
		for (size_t i = 0; i < pmx.joints.size(); i++)
			printf("  %lu: %s\n", i, pmx.joints[i].nameLocal.c_str());

		printf("SoftBodies(%lu): \n", pmx.softbodyData.size());
		for (size_t i = 0; i < pmx.softbodyData.size(); i++)
			printf("  %lu: %s\n", i, pmx.softbodyData[i].nameLocal.c_str());
        return 0;
    }
}

#ifdef  _MSC_VER
#include <Windows.h>
void main()
{
	SetConsoleOutputCP(65001);
	ProcessFile(R"(F:\Dev\Emscripten\SAO ver1.2\SAO\Kirito(GGO)\Kirito_x.pmx)");
}
#endif
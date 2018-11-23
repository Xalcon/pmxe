#include <cstdio>
#include <iostream>
#include <fstream>
#include "pmx/Pmx.hpp"
#include "pmx/PmxException.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

void PrintPmxInfo(const vitriol::Pmx& pmx)
{
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
    printf("Vertices: %zu\n", pmx.vertices.size());
    int bwt[5];
    for (auto& vertex : pmx.vertices)
        bwt[static_cast<int>(vertex.skinningType)]++;
    printf("  BDEF1: %d\n  BDEF2: %d\n  BDEF4: %d\n  SDEF: %d\n  QDEF: %d\n", bwt[0], bwt[1], bwt[2], bwt[3], bwt[4]);
    printf("Polygons: %zu\n", pmx.vertexIndices.size() / 3);
    
    printf("Textures(%zu): \n", pmx.textures.size());
    for(size_t i = 0; i < pmx.textures.size(); i++)
        printf("  %zu: %s\n", i, pmx.textures[i].c_str());

    printf("Materials(%zu): \n", pmx.materials.size());
    for(size_t i = 0; i < pmx.materials.size(); i++)
        printf("  %zu: %s\n", i, pmx.materials[i].nameLocal.c_str());

    printf("Bones(%zu): \n", pmx.bones.size());
    for(size_t i = 0; i < pmx.bones.size(); i++)
        printf("  %zu: %s\n", i, pmx.bones[i].nameLocal.c_str());

    printf("Morphs(%zu): \n", pmx.morphs.size());
    for (size_t i = 0; i < pmx.morphs.size(); i++)
        printf("  %zu: %s\n", i, pmx.morphs[i].nameLocal.c_str());

    printf("DisplayPanes(%zu): \n", pmx.displayPanes.size());
    for (size_t i = 0; i < pmx.displayPanes.size(); i++)
        printf("  %zu: %s\n", i, pmx.displayPanes[i].nameLocal.c_str());

    printf("RigidBodies(%zu): \n", pmx.rigidBodies.size());
    for (size_t i = 0; i < pmx.rigidBodies.size(); i++)
        printf("  %zu: %s\n", i, pmx.rigidBodies[i].nameLocal.c_str());

    printf("Joints(%zu): \n", pmx.joints.size());
    for (size_t i = 0; i < pmx.joints.size(); i++)
        printf("  %zu: %s\n", i, pmx.joints[i].nameLocal.c_str());

    printf("SoftBodies(%zu): \n", pmx.softbodyData.size());
    for (size_t i = 0; i < pmx.softbodyData.size(); i++)
        printf("  %zu: %s\n", i, pmx.softbodyData[i].nameLocal.c_str());
}

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
			pmx.Parse(file);
            const std::streamoff len = file.tellg();
            printf("Read %u bytes from %s (Total size: %llu bytes)\n", static_cast<uint32_t>(len), filePath, totalFileSize);
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
        file.close();
		
		std::ofstream outFile;
		outFile.open(std::string(filePath) + ".out", std::ifstream::out | std::ifstream::binary);
		if (outFile.fail())
			throw "Unable to open file";
        pmx.Save(outFile);
		outFile.close();
        
        //PrintPmxInfo(pmx);

        return 0;
    }
}

#ifdef  _MSC_VER
#include <Windows.h>
int main(int argc, const char* argv[])
{
	SetConsoleOutputCP(65001);
	ProcessFile(argv[1]);
	return 0;
}
#endif
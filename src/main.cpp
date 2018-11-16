#include <iostream>
#include <fstream>
#include <emscripten.h>
#include "pmx/Pmx.hpp"
#include "../extern/zip/src/zip.h"

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

typedef unsigned char byte;

typedef struct text_s {
	int32_t    size;
	
	union encoding_u {
		uint8_t *    utf8;  // sizeof( uint8_t ) * size
		int16_t *    utf16; // sizeof( int16_t ) * ( size / 2 )
	};
} text_t;

struct PMX_Header
{
    char magic[4];
    float version;
    byte nGLobals;
    byte* globals;
    text_s modelNameLocal;
    text_s modelNameUniversal;
    text_s commentLocal;
    text_s commentUniversal;
};

int on_extract_entry(const char *filename, void *arg) {
    static int i = 0;
    int n = *(int *)arg;
    printf("Extracted: %s (%d of %d)\n", filename, ++i, n);

    return 1;
}

int getFileSize(const char* filename)
{
    std::ifstream stream;
    stream.open(filename);
    stream.seekg(0, std::ios::end);
    int size = stream.tellg();
    stream.close();
    return size;
}

extern "C" {

    EMSCRIPTEN_KEEPALIVE int Unzip(const char* filePath)
    {
        struct zip_t *zip = zip_open(filePath, 0, 'r');
        int i, n = zip_total_entries(zip);
        printf("Processing %s (found %d entries)\n", filePath, n);
        for (i = 0; i < n; ++i)
        {
            zip_entry_openbyindex(zip, i);
            {
                const char *name = zip_entry_name(zip);
                int isdir = zip_entry_isdir(zip);
                unsigned long long size = zip_entry_size(zip);
                unsigned int crc32 = zip_entry_crc32(zip);

                printf("File: %s - %llu - %d\n", name, size, crc32);
                emscripten_sleep(1);
            }
            zip_entry_close(zip);
        }
        zip_close(zip);
        return i;
    }

    EMSCRIPTEN_KEEPALIVE int ProcessFile(const char* filePath)
    {
        std::ifstream file;
        file.open(filePath, std::ifstream::in | std::ifstream::binary);

        file.seekg(0, std::ios::end);
        const int totalFileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        Vitriol::Pmx pmx;
        pmx.Parse(&file);

        const int len = file.tellg();
        file.close();

        printf("Read %d bytes from %s (Total size: %d bytes)\n", len, filePath, totalFileSize);
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
        printf("Vertices: %lu\n", pmx.Vertices.size());
        int bwt[5];
        for(int i = 0; i < pmx.Vertices.size(); i++)
            bwt[(int)pmx.Vertices[i].skinningType]++;
        printf("  BDEF1: %d\n  BDEF2: %d\n  BDEF4: %d\n  SDEF: %d\n  QDEF: %d\n", bwt[0], bwt[1], bwt[2], bwt[3], bwt[4]);
        printf("Polygons: %lu\n", pmx.vertexIndices.size() / 3);
        printf("Textures: \n");
        for(int i = 0; i < pmx.textures.size(); i++)
            printf("  %s\n", pmx.textures[i].c_str());

        return 0;
    }
}

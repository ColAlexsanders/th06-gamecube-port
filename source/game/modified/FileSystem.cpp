#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <direct.h>
#include <new>
#include <windows.h>
#elif __cplusplus >= 201703L
#include <filesystem>
#else // Assume POSIX
#include <sys/stat.h>
#endif

#include "FileSystem.hpp"
#include "pbg3/Pbg3Archive.hpp"
#include "utils.hpp"

#include "in_dat.h"
#include "md_dat.h"
#include "cm_dat.h"
#include "ed_dat.h"
#include "st_dat.h"
#include "tl_dat.h"
#include "cfg_dat.h"

u32 g_LastFileSize;

// THIS IS FOR TESTING ON DOLPHIN since Dolphin does not have SD gecko compatibility for Gamecube.
//   Dolphin doesn't emulate any FAT-capable GC storage, meaning libfat has nothing to mount.
struct EmbeddedAsset
{
    const char * name;
    const u8 * data; // data symbol from bin2s ran on the given .DAT file
    u32 size;
};

static const EmbeddedAsset kEmbeddedAssets[] = { 
    {"紅魔郷IN.DAT", in_dat, in_dat_size},
    {"紅魔郷MD.DAT", md_dat, md_dat_size},
    {"紅魔郷CM.DAT", cm_dat, cm_dat_size},
    {"紅魔郷ED.DAT", ed_dat, ed_dat_size},
    {"紅魔郷ST.DAT", st_dat, st_dat_size},
    {"紅魔郷TL.DAT", tl_dat, tl_dat_size},
    {"東方紅魔郷.cfg", cfg_dat, cfg_dat_size},
};

FILE *FileSystem::FopenUTF8(const char *filepath, const char *mode)
{
    if (mode[0] == 'r')
    {
        for (size_t i = 0; i < ARRAY_SIZE(kEmbeddedAssets); i++)
        {
            if (std::strcmp(kEmbeddedAssets[i].name, filepath) == 0)
                return fmemopen((void *)kEmbeddedAssets[i].data, kEmbeddedAssets[i].size, mode);
        }
    }
#ifndef _WIN32
    return std::fopen(filepath, mode);
#else
    u32 filepathWLen = MultiByteToWideChar(CP_UTF8, 0, filepath, -1, NULL, 0) * 2;
    u32 modeWLen = MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0) * 2;

    if (filepathWLen == 0 || modeWLen == 0)
    {
        return NULL;
    }

    wchar_t *filepathW = new wchar_t[filepathWLen];
    wchar_t *modeW = new wchar_t[modeWLen];

    MultiByteToWideChar(CP_UTF8, 0, filepath, -1, filepathW, filepathWLen / 2);
    MultiByteToWideChar(CP_UTF8, 0, mode, -1, modeW, modeWLen / 2);

    FILE *f = _wfopen(filepathW, modeW);

    delete[] filepathW;
    delete[] modeW;

    return f;
#endif
}

void FileSystem::CreateDir(const char *path)
{
#ifdef _WIN32
    _mkdir(path);
#elif __cplusplus >= 201703L
    auto p = std::filesystem::path(path);
    std::filesystem::create_directory(p);
#else
    mkdir(path, 0755);
#endif
}

u8 *FileSystem::OpenPath(const char *filepath, int isExternalResource)
{
    u8 *data;
    FILE *file;
    size_t fsize;
    i32 entryIdx;
    const char *entryname;
    i32 pbg3Idx;

    entryIdx = -1;
    if (isExternalResource == 0)
    {
        entryname = std::strrchr(filepath, '\\');
        if (entryname == (char *)0x0)
        {
            entryname = filepath;
        }
        else
        {
            entryname = entryname + 1;
        }
        entryname = std::strrchr(entryname, '/');
        if (entryname == (char *)0x0)
        {
            entryname = filepath;
        }
        else
        {
            entryname = entryname + 1;
        }
        if (g_Pbg3Archives != NULL)
        {
            for (pbg3Idx = 0; pbg3Idx < 0x10; pbg3Idx += 1)
            {
                if (g_Pbg3Archives[pbg3Idx] != NULL)
                {
                    entryIdx = g_Pbg3Archives[pbg3Idx]->FindEntry(entryname);
                    if (entryIdx >= 0)
                    {
                        break;
                    }
                }
            }
        }
        if (entryIdx < 0)
        {
            return NULL;
        }
    }
    if (entryIdx >= 0)
    {
        utils::DebugPrint2("%s Decode ... \n", entryname);
        data = g_Pbg3Archives[pbg3Idx]->ReadDecompressEntry(entryIdx, entryname);
        g_LastFileSize = g_Pbg3Archives[pbg3Idx]->GetEntrySize(entryIdx);
    }
    else
    {
        utils::DebugPrint2("%s Load ... \n", filepath);
        file = FopenUTF8(filepath, "rb");
        if (file == NULL)
        {
            utils::DebugPrint2("error : %s is not found.\n", filepath);
            return NULL;
        }
        else
        {
            std::fseek(file, 0, SEEK_END);
            fsize = std::ftell(file);
            g_LastFileSize = fsize;
            std::fseek(file, 0, SEEK_SET);
            data = (u8 *)std::malloc(fsize);
            std::fread(data, 1, fsize, file);
            std::fclose(file);
        }
    }
    return data;
}

int FileSystem::WriteDataToFile(const char *path, const void *data, size_t size)
{
    FILE *f;

    f = FopenUTF8(path, "wb");
    if (f == NULL)
    {
        return -1;
    }
    else
    {
        if (std::fwrite(data, 1, size, f) != size)
        {
            std::fclose(f);
            return -2;
        }
        else
        {
            std::fclose(f);
            return 0;
        }
    }
}

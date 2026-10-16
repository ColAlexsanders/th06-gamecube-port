#include "AnmManager.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "Rng.hpp"
#include "Supervisor.hpp"
#include "TextHelper.hpp"
#include "ZunMath.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

// SDL2 stubs - these are replaced with custom Surface/Rect implementations
// See Surface.hpp for the replacement structures

static VertexTex1Xyzrhw g_PrimitivesToDrawVertexBuf[4];
static VertexTex1DiffuseXyzrhw g_PrimitivesToDrawNoVertexBuf[4];
static VertexTex1DiffuseXyz g_PrimitivesToDrawUnknown[4];
AnmManager *g_AnmManager;

// Texture format mappings - these are project-specific
// The SDL mappings are replaced with direct format constants

static const PixelFormat g_TextureFormatTypeGfxMapping[6] = {
    static_cast<PixelFormat>(0), PIXEL_RGBA, PIXEL_RGBA, PIXEL_RGB, PIXEL_RGB, PIXEL_RGBA};

static const PixelDataType g_TextureFormatTypeMapping[6] = {static_cast<PixelDataType>(0), // ugh
                                                            PIXEL_UNSIGNED_BYTE,           PIXEL_UNSIGNED_SHORT_5_5_5_1,
                                                            PIXEL_UNSIGNED_SHORT_5_6_5,    PIXEL_UNSIGNED_BYTE,
                                                            PIXEL_UNSIGNED_SHORT_4_4_4_4};

static const u8 g_TextureFormatBytesPerPixel[6] = {0, 4, 2, 2, 3, 2};

// Stub implementations for functions that are NOT defined inline in the header

AnmManager::AnmManager()
{
    // Constructor stub
}

AnmManager::~AnmManager()
{
    // Destructor stub
}

Surface *AnmManager::LoadToSurfaceWithFormat(const char *filename, u32 format, u8 **fileData)
{
    // Stub: Image loading not yet implemented
    // This requires image decoding (PNG/JPG) which needs either:
    // 1. A simple custom PNG decoder
    // 2. Porting an image library to devkitPPC
    // 3. Using pre-converted raw pixel data
    
    (void)filename;
    (void)format;
    (void)fileData;
    return NULL;
}

u8 *AnmManager::ExtractSurfacePixels(Surface *src, u8 pixelDepth)
{
    // Stub: pixel extraction not yet implemented
    (void)src;
    (void)pixelDepth;
    return NULL;
}

void AnmManager::FlipSurface(Surface *surface)
{
    // Stub: surface flipping not yet implemented
    (void)surface;
}

void AnmManager::ApplySurfaceToColorBuffer(Surface *src, const Rect &srcRect, const Rect &dstRect)
{
    // Stub: surface application not yet implemented
    (void)src;
    (void)srcRect;
    (void)dstRect;
}

void AnmManager::ReleaseSurfaces(void)
{
    // Release all surfaces stub
    for (u32 i = 0; i < 32; i++)
    {
        if (this->surfaces[i] != NULL)
        {
            free(this->surfaces[i]->pixels);
            this->surfaces[i]->pixels = NULL;
            delete this->surfaces[i];
            this->surfaces[i] = NULL;
        }
    }
}

void AnmManager::ReleaseSurface(i32 surfaceIdx)
{
    // Release surface stub
    if (surfaceIdx >= 0 && surfaceIdx < 32 && this->surfaces[surfaceIdx] != NULL)
    {
        free(this->surfaces[surfaceIdx]->pixels);
        this->surfaces[surfaceIdx]->pixels = NULL;
        delete this->surfaces[surfaceIdx];
        this->surfaces[surfaceIdx] = NULL;
    }
}

ZunResult AnmManager::LoadSurface(i32 surfaceIdx, const char *path)
{
    // Load surface stub - image loading not yet implemented
    (void)surfaceIdx;
    (void)path;
    return ZUN_SUCCESS;
}

void AnmManager::ReleaseTexture(i32 textureIdx)
{
    // Release texture stub
    (void)textureIdx;
}

ZunResult AnmManager::LoadTexture(i32 textureIdx, const char *textureName, i32 textureFormat, ZunColor colorKey)
{
    // Load texture stub - requires image loading
    (void)textureIdx;
    (void)textureName;
    (void)textureFormat;
    (void)colorKey;
    return ZUN_SUCCESS;
}

ZunResult AnmManager::LoadTextureAlphaChannel(i32 textureIdx, const char *textureName, i32 textureFormat, ZunColor colorKey)
{
    // Load texture alpha channel stub - requires image loading
    (void)textureIdx;
    (void)textureName;
    (void)textureFormat;
    (void)colorKey;
    return ZUN_SUCCESS;
}

void AnmManager::ReleaseAnm(i32 anmIdx)
{
    // Release animation stub
    (void)anmIdx;
}

ZunResult AnmManager::LoadAnm(i32 anmIdx, const char *path, i32 spriteIdxOffset)
{
    // Load animation stub
    (void)anmIdx;
    (void)path;
    (void)spriteIdxOffset;
    return ZUN_SUCCESS;
}

void AnmManager::TakeScreenshotIfRequested()
{
    // Screenshot stub
}

void AnmManager::SetupVertexBuffer()
{
    // Setup vertex buffer stub
}

// --- Missing function stubs for GameCube port ---

ZunResult AnmManager::DrawNoRotation(const AnmVm *vm)
{
    // Draw no rotation stub - sprite rendering deferred to GX backend
    (void)vm;
    return ZUN_SUCCESS;
}

void AnmManager::FlushVertexBuffer()
{
    // Flush vertex buffer stub
}

void AnmManager::CopySurfaceToBackBuffer(i32 surfaceIdx, i32 left, i32 top, i32 x, i32 y)
{
    // Copy surface to back buffer stub
    (void)surfaceIdx;
    (void)left;
    (void)top;
    (void)x;
    (void)y;
}

void AnmManager::UpdateDirtyStates()
{
    // Update dirty states stub
}

i32 AnmManager::ExecuteScript(AnmVm *vm)
{
    // Execute script stub - ANM script execution deferred
    (void)vm;
    return 0;
}

void AnmManager::DrawVmTextFmt(AnmVm *vm, ZunColor textColor, ZunColor shadowColor, const char *fmt, ...)
{
    // Draw VM text format stub - text rendering deferred to bitmap font system
    (void)vm;
    (void)textColor;
    (void)shadowColor;
    (void)fmt;
}

void AnmManager::DrawStringFormat2(AnmVm *vm, ZunColor textColor, ZunColor shadowColor, const char *fmt, ...)
{
    // Draw string format stub - text rendering deferred to bitmap font system
    (void)vm;
    (void)textColor;
    (void)shadowColor;
    (void)fmt;
}

ZunResult AnmManager::SetActiveSprite(AnmVm *vm, u32 spriteIdx)
{
    // Set active sprite stub - sprite loading deferred
    (void)vm;
    (void)spriteIdx;
    return ZUN_SUCCESS;
}

void AnmManager::SetAndExecuteScript(AnmVm *vm, const AnmRawInstr *beginingOfScript)
{
    // Set and execute script stub - ANM script execution deferred
    (void)vm;
    (void)beginingOfScript;
}

ZunResult AnmManager::Draw(const AnmVm *vm)
{
    // Draw stub - sprite rendering deferred to GX backend
    (void)vm;
    return ZUN_SUCCESS;
}

void AnmManager::CopySurfaceRectToBackBuffer(i32 surfaceIdx, i32 rectX, i32 rectY, i32 rectLeft, i32 rectTop, i32 width, i32 height)
{
    // Copy surface rect to back buffer stub
    (void)surfaceIdx;
    (void)rectX;
    (void)rectY;
    (void)rectLeft;
    (void)rectTop;
    (void)width;
    (void)height;
}

ZunResult AnmManager::DrawFacingCamera(const AnmVm *vm)
{
    // Draw facing camera stub - sprite rendering deferred to GX backend
    (void)vm;
    return ZUN_SUCCESS;
}

ZunResult AnmManager::Draw2(const AnmVm *vm)
{
    // Draw2 stub - sprite rendering deferred to GX backend
    (void)vm;
    return ZUN_SUCCESS;
}

ZunResult AnmManager::Draw3(const AnmVm *vm)
{
    // Draw3 stub - sprite rendering deferred to GX backend
    (void)vm;
    return ZUN_SUCCESS;
}

void AnmManager::DrawStringFormat(AnmVm *vm, ZunColor textColor, ZunColor shadowColor, const char *fmt, ...)
{
    // Draw string format stub - text rendering deferred to bitmap font system
    (void)vm;
    (void)textColor;
    (void)shadowColor;
    (void)fmt;
}


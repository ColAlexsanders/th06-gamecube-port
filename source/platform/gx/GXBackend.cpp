#include "GXBackend.hpp"
#include <gccore.h>

GfxInterface *GXBackend::Init()
{
    return new GXBackend();
}

void GXBackend::Exit()                                                         { /* TODO: VIDEO_SetBlack */ }
void GXBackend::SetFogRange(f32 nearPlane, f32 farPlane)                       { /* TODO: GX_SetFog */ }
void GXBackend::SetFogColor(ZunColor color)                                    { /* TODO: GX_SetFog */ }
void GXBackend::ToggleVertexAttribute(u8 attr, bool enable)                    { /* TODO: GX_SetVtxDesc */ }
void GXBackend::SetAttributePointer(VertexAttributeArrays attr,
                                    std::size_t stride, void *ptr)             { /* TODO: GX_SetVtxAttrFmt */ }
void GXBackend::SetColorOp(TextureOpComponent component, ColorOp op)           { /* TODO: GX_SetTevColorOp */ }
void GXBackend::SetTextureFactor(ZunColor factor)                              { /* TODO: GX_SetTevColor */ }
void GXBackend::SetTransformMatrix(TransformMatrix type, const ZunMatrix &mat) { /* TODO: GX_LoadPosMtxImm */ }
void GXBackend::SetTextureFilter()                                             { /* TODO: GX_InitTexObjFilterMode */ }
void GXBackend::GetViewport(u32 *viewport)                                     { /* TODO: store viewport state */ }
void GXBackend::GetDepthRange(f32 *depthRange)                                 { /* TODO: store depth state */ }
void GXBackend::SetViewport(i32 x, i32 y, i32 width, i32 height)              { /* TODO: GX_SetViewport */ }
void GXBackend::SetDepthRange(f32 nearPlane, f32 farPlane)                     { /* TODO: GX_SetViewport z params */ }
void GXBackend::Enable(Capabilities cap)                                       { /* TODO: map caps to GX state */ }
bool GXBackend::HasError()                                                     { return false; }
void GXBackend::SetBlendMode(BlendMode mode)                                   { /* TODO: GX_SetBlendMode */ }
void GXBackend::SetDepthMask(bool enable)                                      { /* TODO: GX_SetZMode */ }
void GXBackend::SetDepthFunc(DepthFunc func)                                   { /* TODO: GX_SetZMode */ }
void GXBackend::SetClearDepth(f32 depth)                                       { /* TODO: GX_SetCopyClear z */ }
void GXBackend::SetClearColor(f32 r, f32 g, f32 b, f32 a)                     { /* TODO: GX_SetCopyClear color */ }
void GXBackend::Clear(u32 clearBits)                                           { /* TODO: GX_ClearBoundingBox */ }
GfxTextureHandle GXBackend::CreateTexture()                                    { return 0; /* TODO: allocate GXTexObj */ }
void GXBackend::BindTexture(GfxTextureHandle handle)                           { /* TODO: GX_LoadTexObj */ }
void GXBackend::DeleteTexture(GfxTextureHandle handle)                         { /* TODO: free TMem */ }
void GXBackend::SetTextureImage(u32 width, u32 height, PixelFormat fmt,
                                PixelDataType type, const void *data)          { /* TODO: GX_InitTexObj */ }
void GXBackend::SetTextureSubImage(i32 xoffset, i32 yoffset,
                                   i32 width, i32 height, const void *data)   { /* TODO: partial texture update */ }
void GXBackend::ReadPixels(i32 x, i32 y,
                           i32 width, i32 height, const void *pixels)         { /* TODO: EFB read */ }
void GXBackend::Draw(PrimitiveType type, i32 start, i32 count)                 { /* TODO: GX_Begin/GX_End */ }
void GXBackend::SwapBuffers()                                                  { /* TODO: GX_DrawDone, VIDEO_Flush */ }

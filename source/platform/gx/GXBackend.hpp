#pragma once
#include "../../game/modified/graphics/GfxInterface.hpp"

struct GXBackend : GfxInterface
{
    static GfxInterface *Init();
    virtual void Exit();
    ~GXBackend() override { Exit(); }

    void SetFogRange(f32 nearPlane, f32 farPlane) override;
    void SetFogColor(ZunColor color) override;
    void ToggleVertexAttribute(u8 attr, bool enable) override;
    void SetAttributePointer(VertexAttributeArrays attr, std::size_t stride, void *ptr) override;
    void SetColorOp(TextureOpComponent component, ColorOp op) override;
    void SetTextureFactor(ZunColor factor) override;
    void SetTransformMatrix(TransformMatrix type, const ZunMatrix &matrix) override;
    void SetTextureFilter() override;
    void GetViewport(u32 *viewport) override;
    void GetDepthRange(f32 *depthRange) override;
    void SetViewport(i32 x, i32 y, i32 width, i32 height) override;
    void SetDepthRange(f32 nearPlane, f32 farPlane) override;
    void Enable(Capabilities cap) override;
    bool HasError() override;
    void SetBlendMode(BlendMode mode) override;
    void SetDepthMask(bool enable) override;
    void SetDepthFunc(DepthFunc func) override;
    void SetClearDepth(f32 depth) override;
    void SetClearColor(f32 r, f32 g, f32 b, f32 a) override;
    void Clear(u32 clearBits) override;
    GfxTextureHandle CreateTexture() override;
    void BindTexture(GfxTextureHandle handle) override;
    void DeleteTexture(GfxTextureHandle handle) override;
    void SetTextureImage(u32 width, u32 height, PixelFormat fmt, PixelDataType type, const void *data) override;
    void SetTextureSubImage(i32 xoffset, i32 yoffset, i32 width, i32 height, const void *data) override;
    void ReadPixels(i32 x, i32 y, i32 width, i32 height, const void *pixels) override;
    void Draw(PrimitiveType type, i32 start, i32 count) override;
    void SwapBuffers() override;
};

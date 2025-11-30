#pragma once

#include <RmlUi/Core.h>
#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <vector>

// ============================================
// RmlUi SDL2 渲染器
// ============================================
// 实现 RmlUi::RenderInterface 用于 SDL2 渲染
// 针对 Nintendo Switch 优化
// ============================================

class RmlUi_Renderer_SDL : public Rml::RenderInterface
{
public:
    RmlUi_Renderer_SDL(SDL_Renderer* renderer, int width, int height);
    virtual ~RmlUi_Renderer_SDL();

    // ========================================
    // RenderInterface 必需方法
    // ========================================
    
    // 渲染几何体
    void RenderGeometry(Rml::Vertex* vertices, int num_vertices,
                       int* indices, int num_indices,
                       Rml::TextureHandle texture,
                       const Rml::Vector2f& translation) override;
    
    // 编译几何体（可选优化）
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices,
                                                int* indices, int num_indices,
                                                Rml::TextureHandle texture) override;
    
    void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry,
                               const Rml::Vector2f& translation) override;
    
    void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;
    
    // 裁剪区域
    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(int x, int y, int width, int height) override;
    
    // 纹理管理
    bool LoadTexture(Rml::TextureHandle& texture_handle,
                    Rml::Vector2i& texture_dimensions,
                    const Rml::String& source) override;
    
    bool GenerateTexture(Rml::TextureHandle& texture_handle,
                        const Rml::byte* source,
                        const Rml::Vector2i& source_dimensions) override;
    
    void ReleaseTexture(Rml::TextureHandle texture_handle) override;
    
    // 变换矩阵
    void SetTransform(const Rml::Matrix4f* transform) override;

    // ========================================
    // 辅助方法
    // ========================================
    
    // 帧管理
    void BeginFrame();
    void EndFrame();
    
    // 获取渲染器
    SDL_Renderer* GetSDLRenderer() const { return m_renderer; }

private:
    SDL_Renderer* m_renderer;
    int m_screen_width;
    int m_screen_height;
    
    // 裁剪状态
    bool m_scissor_enabled;
    SDL_Rect m_scissor_rect;
    
    // 变换矩阵
    Rml::Matrix4f m_transform;
    bool m_transform_enabled;
    
    // 编译几何体结构
    struct CompiledGeometry
    {
        std::vector<Rml::Vertex> vertices;
        std::vector<int> indices;
        Rml::TextureHandle texture;
    };
    std::map<Rml::CompiledGeometryHandle, CompiledGeometry> m_compiled_geometry;
    Rml::CompiledGeometryHandle m_next_geometry_handle;
    
    // 纹理缓存
    struct TextureData
    {
        SDL_Texture* texture;
        int width;
        int height;
    };
    std::map<Rml::TextureHandle, TextureData> m_textures;
    Rml::TextureHandle m_next_texture_handle;
    
    // 辅助函数
    SDL_Color ConvertColor(const Rml::Colourf& color);
    SDL_Color ConvertColor(const Rml::Colourb& color);
    void ApplyTransform(Rml::Vector2f& point);
    SDL_Texture* LoadTextureFromFile(const Rml::String& path, int& width, int& height);
};

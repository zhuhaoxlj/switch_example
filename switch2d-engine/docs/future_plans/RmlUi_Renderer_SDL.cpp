#include "RmlUi_Renderer_SDL.h"
#include <algorithm>
#include <cmath>

RmlUi_Renderer_SDL::RmlUi_Renderer_SDL(SDL_Renderer* renderer, int width, int height)
    : m_renderer(renderer)
    , m_screen_width(width)
    , m_screen_height(height)
    , m_scissor_enabled(false)
    , m_transform_enabled(false)
    , m_next_geometry_handle(1)
    , m_next_texture_handle(1)
{
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

RmlUi_Renderer_SDL::~RmlUi_Renderer_SDL()
{
    // 释放所有纹理
    for (auto& pair : m_textures)
    {
        if (pair.second.texture)
        {
            SDL_DestroyTexture(pair.second.texture);
        }
    }
    m_textures.clear();
}

void RmlUi_Renderer_SDL::RenderGeometry(Rml::Vertex* vertices, int num_vertices,
                                        int* indices, int num_indices,
                                        Rml::TextureHandle texture,
                                        const Rml::Vector2f& translation)
{
    if (num_vertices == 0 || num_indices == 0)
        return;

    // 获取纹理（如果有）
    SDL_Texture* sdl_texture = nullptr;
    if (texture)
    {
        auto it = m_textures.find(texture);
        if (it != m_textures.end())
        {
            sdl_texture = it->second.texture;
        }
    }

    // 应用裁剪
    if (m_scissor_enabled)
    {
        SDL_RenderSetClipRect(m_renderer, &m_scissor_rect);
    }

    // 渲染三角形
    for (int i = 0; i < num_indices; i += 3)
    {
        if (i + 2 >= num_indices)
            break;

        Rml::Vertex& v0 = vertices[indices[i]];
        Rml::Vertex& v1 = vertices[indices[i + 1]];
        Rml::Vertex& v2 = vertices[indices[i + 2]];

        // 应用平移
        Rml::Vector2f p0 = v0.position + translation;
        Rml::Vector2f p1 = v1.position + translation;
        Rml::Vector2f p2 = v2.position + translation;

        // 应用变换矩阵（如果启用）
        if (m_transform_enabled)
        {
            ApplyTransform(p0);
            ApplyTransform(p1);
            ApplyTransform(p2);
        }

        if (sdl_texture)
        {
            // 有纹理：绘制纹理三角形
            // SDL2 没有原生三角形纹理支持，使用简化的矩形渲染
            float min_x = std::min({p0.x, p1.x, p2.x});
            float max_x = std::max({p0.x, p1.x, p2.x});
            float min_y = std::min({p0.y, p1.y, p2.y});
            float max_y = std::max({p0.y, p1.y, p2.y});

            SDL_Rect dst_rect = {
                (int)min_x, (int)min_y,
                (int)(max_x - min_x), (int)(max_y - min_y)
            };

            // 使用顶点颜色调制纹理
            SDL_Color color = ConvertColor(v0.colour);
            SDL_SetTextureColorMod(sdl_texture, color.r, color.g, color.b);
            SDL_SetTextureAlphaMod(sdl_texture, color.a);

            SDL_RenderCopy(m_renderer, sdl_texture, nullptr, &dst_rect);
        }
        else
        {
            // 无纹理：绘制实心三角形
            // SDL2 没有原生三角形填充，使用线条模拟
            SDL_Color color = ConvertColor(v0.colour);
            SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

            // 绘制填充三角形（使用扫描线）
            SDL_Vertex sdl_vertices[3] = {
                {{p0.x, p0.y}, color, {0, 0}},
                {{p1.x, p1.y}, color, {0, 0}},
                {{p2.x, p2.y}, color, {0, 0}}
            };
            
            // 使用 SDL_RenderGeometry（如果支持）
            #if SDL_VERSION_ATLEAST(2, 0, 18)
            SDL_RenderGeometry(m_renderer, nullptr, sdl_vertices, 3, nullptr, 0);
            #else
            // 回退：绘制三角形轮廓和填充
            SDL_RenderDrawLine(m_renderer, p0.x, p0.y, p1.x, p1.y);
            SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
            SDL_RenderDrawLine(m_renderer, p2.x, p2.y, p0.x, p0.y);
            #endif
        }
    }

    // 恢复裁剪
    if (m_scissor_enabled)
    {
        SDL_RenderSetClipRect(m_renderer, nullptr);
    }
}

Rml::CompiledGeometryHandle RmlUi_Renderer_SDL::CompileGeometry(
    Rml::Vertex* vertices, int num_vertices,
    int* indices, int num_indices,
    Rml::TextureHandle texture)
{
    CompiledGeometry geometry;
    geometry.vertices.assign(vertices, vertices + num_vertices);
    geometry.indices.assign(indices, indices + num_indices);
    geometry.texture = texture;

    Rml::CompiledGeometryHandle handle = m_next_geometry_handle++;
    m_compiled_geometry[handle] = std::move(geometry);

    return handle;
}

void RmlUi_Renderer_SDL::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry,
                                                const Rml::Vector2f& translation)
{
    auto it = m_compiled_geometry.find(geometry);
    if (it == m_compiled_geometry.end())
        return;

    CompiledGeometry& geom = it->second;
    RenderGeometry(geom.vertices.data(), geom.vertices.size(),
                  geom.indices.data(), geom.indices.size(),
                  geom.texture, translation);
}

void RmlUi_Renderer_SDL::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
{
    m_compiled_geometry.erase(geometry);
}

void RmlUi_Renderer_SDL::EnableScissorRegion(bool enable)
{
    m_scissor_enabled = enable;
    if (!enable)
    {
        SDL_RenderSetClipRect(m_renderer, nullptr);
    }
}

void RmlUi_Renderer_SDL::SetScissorRegion(int x, int y, int width, int height)
{
    m_scissor_rect.x = x;
    m_scissor_rect.y = y;
    m_scissor_rect.w = width;
    m_scissor_rect.h = height;

    if (m_scissor_enabled)
    {
        SDL_RenderSetClipRect(m_renderer, &m_scissor_rect);
    }
}

bool RmlUi_Renderer_SDL::LoadTexture(Rml::TextureHandle& texture_handle,
                                     Rml::Vector2i& texture_dimensions,
                                     const Rml::String& source)
{
    int width, height;
    SDL_Texture* texture = LoadTextureFromFile(source, width, height);

    if (!texture)
        return false;

    texture_handle = m_next_texture_handle++;
    TextureData data;
    data.texture = texture;
    data.width = width;
    data.height = height;
    m_textures[texture_handle] = data;

    texture_dimensions.x = width;
    texture_dimensions.y = height;

    return true;
}

bool RmlUi_Renderer_SDL::GenerateTexture(Rml::TextureHandle& texture_handle,
                                         const Rml::byte* source,
                                         const Rml::Vector2i& source_dimensions)
{
    // 创建 RGBA8888 纹理
    SDL_Texture* texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        source_dimensions.x,
        source_dimensions.y
    );

    if (!texture)
        return false;

    // 上传像素数据
    SDL_UpdateTexture(texture, nullptr, source, source_dimensions.x * 4);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    texture_handle = m_next_texture_handle++;
    TextureData data;
    data.texture = texture;
    data.width = source_dimensions.x;
    data.height = source_dimensions.y;
    m_textures[texture_handle] = data;

    return true;
}

void RmlUi_Renderer_SDL::ReleaseTexture(Rml::TextureHandle texture_handle)
{
    auto it = m_textures.find(texture_handle);
    if (it != m_textures.end())
    {
        if (it->second.texture)
        {
            SDL_DestroyTexture(it->second.texture);
        }
        m_textures.erase(it);
    }
}

void RmlUi_Renderer_SDL::SetTransform(const Rml::Matrix4f* transform)
{
    if (transform)
    {
        m_transform = *transform;
        m_transform_enabled = true;
    }
    else
    {
        m_transform_enabled = false;
    }
}

void RmlUi_Renderer_SDL::BeginFrame()
{
    // 开始新的渲染帧
    SDL_RenderSetClipRect(m_renderer, nullptr);
}

void RmlUi_Renderer_SDL::EndFrame()
{
    // 结束渲染帧
    SDL_RenderSetClipRect(m_renderer, nullptr);
}

SDL_Color RmlUi_Renderer_SDL::ConvertColor(const Rml::Colourf& color)
{
    return {
        (Uint8)(color.red * 255),
        (Uint8)(color.green * 255),
        (Uint8)(color.blue * 255),
        (Uint8)(color.alpha * 255)
    };
}

SDL_Color RmlUi_Renderer_SDL::ConvertColor(const Rml::Colourb& color)
{
    return {color.red, color.green, color.blue, color.alpha};
}

void RmlUi_Renderer_SDL::ApplyTransform(Rml::Vector2f& point)
{
    if (!m_transform_enabled)
        return;

    // 应用 2D 变换矩阵（假设只使用 2D 变换）
    float x = point.x * m_transform[0][0] + point.y * m_transform[1][0] + m_transform[3][0];
    float y = point.x * m_transform[0][1] + point.y * m_transform[1][1] + m_transform[3][1];
    
    point.x = x;
    point.y = y;
}

SDL_Texture* RmlUi_Renderer_SDL::LoadTextureFromFile(const Rml::String& path, int& width, int& height)
{
    // 尝试加载图像文件
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface)
    {
        // 尝试从 romfs 加载
        Rml::String romfs_path = "romfs:/" + path;
        surface = IMG_Load(romfs_path.c_str());
    }

    if (!surface)
        return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    width = surface->w;
    height = surface->h;
    SDL_FreeSurface(surface);

    if (texture)
    {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    return texture;
}

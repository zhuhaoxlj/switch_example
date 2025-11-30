#include "litehtml_container_sdl.h"
#include "Switch2D/Switch2D.h"
#include <algorithm>
#include <cmath>
#include <SDL_image.h>

litehtml_container_sdl::litehtml_container_sdl(SDL_Renderer* renderer)
    : m_renderer(renderer)
    , m_next_font_id(1)
{
}

litehtml_container_sdl::~litehtml_container_sdl()
{
    // 清理字体
    for (auto& pair : m_fonts)
    {
        if (pair.second)
        {
            TTF_CloseFont(pair.second);
        }
    }
    
    // 清理图像
    for (auto& pair : m_images)
    {
        if (pair.second.texture)
        {
            SDL_DestroyTexture(pair.second.texture);
        }
    }
}

// ========================================
// 字体相关实现
// ========================================

litehtml::uint_ptr litehtml_container_sdl::create_font(const litehtml::font_description& descr, const litehtml::document* doc, litehtml::font_metrics* fm)
{
    // 计算字体大小（pt 转 px）
    int font_size = (int)(descr.size * 96.0 / 72.0); // 假设 96 DPI
    
    // 加载字体（简化：使用固定字体路径）
    TTF_Font* font = TTF_OpenFont(m_font_path.c_str(), font_size);
    if (!font)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 0;
    }
    
    // 获取字体度量
    if (fm)
    {
        fm->height = TTF_FontHeight(font);
        fm->ascent = TTF_FontAscent(font);
        fm->descent = TTF_FontDescent(font);
        fm->x_height = font_size / 2; // 估算
        
        // 测量 'x' 的宽度
        int w = 0, h = 0;
        TTF_SizeUTF8(font, "x", &w, &h);
        fm->x_height = h;
    }
    
    // 保存字体并返回 ID
    litehtml::uint_ptr font_id = m_next_font_id++;
    m_fonts[font_id] = font;
    
    return font_id;
}

void litehtml_container_sdl::delete_font(litehtml::uint_ptr hFont)
{
    auto it = m_fonts.find(hFont);
    if (it != m_fonts.end())
    {
        if (it->second)
        {
            TTF_CloseFont(it->second);
        }
        m_fonts.erase(it);
    }
}

litehtml::pixel_t litehtml_container_sdl::text_width(const char* text, litehtml::uint_ptr hFont)
{
    TTF_Font* font = get_font(hFont);
    if (!font)
        return 0;
    
    int w = 0, h = 0;
    TTF_SizeUTF8(font, text, &w, &h);
    return w;
}

void litehtml_container_sdl::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos)
{
    TTF_Font* font = get_font(hFont);
    if (!font || !text || !text[0])
        return;
    
    SDL_Color sdl_color = web_color_to_sdl(color);
    
    // 渲染文本
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, sdl_color);
    if (surface)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        if (texture)
        {
            SDL_Rect dst = {pos.x, pos.y, surface->w, surface->h};
            SDL_RenderCopy(m_renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

litehtml::pixel_t litehtml_container_sdl::pt_to_px(float pt) const
{
    // 假设 96 DPI
    return (litehtml::pixel_t)(pt * 96.0 / 72.0);
}

litehtml::pixel_t litehtml_container_sdl::get_default_font_size() const
{
    return 16; // 默认 16px
}

const char* litehtml_container_sdl::get_default_font_name() const
{
    return "sans-serif";
}

// ========================================
// 绘制相关实现
// ========================================

void litehtml_container_sdl::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
    // 简化实现：绘制圆点或数字
    SDL_Color color = web_color_to_sdl(marker.color);
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    
    int cx = marker.pos.x + marker.pos.width / 2;
    int cy = marker.pos.y + marker.pos.height / 2;
    int radius = std::min(marker.pos.width, marker.pos.height) / 3;
    
    // 绘制圆点（简化版）
    for (int y = -radius; y <= radius; y++)
    {
        int x = (int)sqrt(radius * radius - y * y);
        SDL_RenderDrawLine(m_renderer, cx - x, cy + y, cx + x, cy + y);
    }
}

void litehtml_container_sdl::draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::web_color& color)
{
    SDL_Color sdl_color = web_color_to_sdl(color);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
    
    const auto& clip_box = layer.clip_box;
    SDL_Rect rect = {clip_box.x, clip_box.y, clip_box.width, clip_box.height};
    SDL_RenderFillRect(m_renderer, &rect);
}

void litehtml_container_sdl::draw_linear_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::background_layer::linear_gradient& gradient)
{
    // 简化实现：使用纯色填充
    if (!gradient.color_points.empty())
    {
        draw_solid_fill(hdc, layer, gradient.color_points[0].color);
    }
}

void litehtml_container_sdl::draw_radial_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::background_layer::radial_gradient& gradient)
{
    // 简化实现：使用纯色填充
    if (!gradient.color_points.empty())
    {
        draw_solid_fill(hdc, layer, gradient.color_points[0].color);
    }
}

void litehtml_container_sdl::draw_conic_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::background_layer::conic_gradient& gradient)
{
    // 简化实现：使用纯色填充
    if (!gradient.color_points.empty())
    {
        draw_solid_fill(hdc, layer, gradient.color_points[0].color);
    }
}

void litehtml_container_sdl::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
    // 绘制边框
    auto draw_border = [this](const litehtml::border& b, int x1, int y1, int x2, int y2)
    {
        if (b.width > 0 && b.style != litehtml::border_style_none)
        {
            SDL_Color color = web_color_to_sdl(b.color);
            SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
            
            for (int i = 0; i < b.width; i++)
            {
                SDL_RenderDrawLine(m_renderer, x1, y1 + i, x2, y2 + i);
            }
        }
    };
    
    // 上边框
    draw_border(borders.top, draw_pos.x, draw_pos.y, 
                draw_pos.x + draw_pos.width, draw_pos.y);
    
    // 右边框
    draw_border(borders.right, draw_pos.x + draw_pos.width, draw_pos.y,
                draw_pos.x + draw_pos.width, draw_pos.y + draw_pos.height);
    
    // 下边框
    draw_border(borders.bottom, draw_pos.x, draw_pos.y + draw_pos.height,
                draw_pos.x + draw_pos.width, draw_pos.y + draw_pos.height);
    
    // 左边框
    draw_border(borders.left, draw_pos.x, draw_pos.y,
                draw_pos.x, draw_pos.y + draw_pos.height);
}

// ========================================
// 图像相关实现
// ========================================

void litehtml_container_sdl::load_image(const char* src, const char* baseurl, bool redraw_on_ready)
{
    if (!src || !m_renderer)
        return;
    
    std::string url = src;
    
    // 检查是否已加载
    if (m_images.find(url) != m_images.end())
        return;
    
    // 构建完整 URL
    std::string full_url = url;
    if (url.find("http") != 0 && baseurl && baseurl[0])
    {
        std::string base = baseurl;
        if (url[0] == '/')
            full_url = base + url;
        else
            full_url = base + "/" + url;
    }
    
    printf("[Image] Loading: %s\n", full_url.c_str());
    
    // 创建图片信息
    image_info info;
    info.texture = nullptr;
    info.width = 100;
    info.height = 100;
    
    // 尝试从网络加载图片
    if (full_url.find("http") == 0)
    {
        try
        {
            Switch2D::NetworkManager* net = Switch2D::Engine::getInstance().getNetwork();
            if (net && net->isInitialized())
            {
                printf("[Image] Downloading: %s\n", full_url.c_str());
                Switch2D::HTTPResponse resp = net->get(full_url);
                
                if (resp.isSuccess() && !resp.body.empty())
                {
                    printf("[Image] Downloaded %zu bytes\n", resp.body.size());
                    
                    // 使用 SDL_image 从内存加载图片
                    SDL_RWops* rw = SDL_RWFromConstMem(
                        reinterpret_cast<const void*>(resp.body.data()),
                        static_cast<int>(resp.body.size())
                    );
                    
                    if (rw)
                    {
                        // IMG_Load_RW 会自动检测图片格式（PNG, JPEG, GIF, BMP等）
                        SDL_Surface* surface = IMG_Load_RW(rw, 1); // 1 = 自动关闭 RWops
                        
                        if (surface)
                        {
                            // 创建纹理
                            info.texture = SDL_CreateTextureFromSurface(m_renderer, surface);
                            
                            if (info.texture)
                            {
                                info.width = surface->w;
                                info.height = surface->h;
                                printf("[Image] ✓ Loaded successfully: %dx%d\n", 
                                      info.width, info.height);
                            }
                            else
                            {
                                printf("[Image] ✗ Failed to create texture: %s\n", 
                                      SDL_GetError());
                            }
                            
                            SDL_FreeSurface(surface);
                        }
                        else
                        {
                            printf("[Image] ✗ IMG_Load_RW failed: %s\n", IMG_GetError());
                            SDL_RWclose(rw); // 手动关闭因为加载失败
                        }
                    }
                    else
                    {
                        printf("[Image] ✗ Failed to create RWops\n");
                    }
                }
                else
                {
                    printf("[Image] ✗ Download failed: %s\n", resp.error.c_str());
                }
            }
            else
            {
                printf("[Image] ✗ Network not available\n");
            }
        }
        catch (const std::exception& e)
        {
            printf("[Image] ✗ Exception: %s\n", e.what());
        }
        catch (...)
        {
            printf("[Image] ✗ Unknown exception\n");
        }
    }
    
    // 缓存图片信息（即使加载失败也缓存，避免重复尝试）
    m_images[url] = info;
}

void litehtml_container_sdl::get_image_size(const char* src, const char* baseurl, litehtml::size& sz)
{
    if (!src)
    {
        sz.width = 100;
        sz.height = 100;
        return;
    }
    
    std::string url = src;
    auto it = m_images.find(url);
    
    if (it != m_images.end())
    {
        sz.width = it->second.width;
        sz.height = it->second.height;
    }
    else
    {
        // 默认尺寸
        sz.width = 100;
        sz.height = 100;
    }
}

void litehtml_container_sdl::draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const std::string& url, const std::string& base_url)
{
    const auto& clip_box = layer.clip_box;
    SDL_Rect rect = {(int)clip_box.x, (int)clip_box.y, (int)clip_box.width, (int)clip_box.height};
    
    auto it = m_images.find(url);
    
    if (it != m_images.end() && it->second.texture)
    {
        // 绘制真实图片
        SDL_RenderCopy(m_renderer, it->second.texture, nullptr, &rect);
    }
    else
    {
        // 绘制带边框的占位符
        // 浅灰色背景
        SDL_SetRenderDrawColor(m_renderer, 240, 240, 240, 255);
        SDL_RenderFillRect(m_renderer, &rect);
        
        // 深灰色边框
        SDL_SetRenderDrawColor(m_renderer, 180, 180, 180, 255);
        SDL_RenderDrawRect(m_renderer, &rect);
        
        // 绘制对角线表示图片占位符
        SDL_RenderDrawLine(m_renderer, rect.x, rect.y, 
                          rect.x + rect.w, rect.y + rect.h);
        SDL_RenderDrawLine(m_renderer, rect.x + rect.w, rect.y, 
                          rect.x, rect.y + rect.h);
    }
}

// ========================================
// 链接和事件实现
// ========================================

void litehtml_container_sdl::set_caption(const char* caption)
{
    m_caption = caption ? caption : "";
}

void litehtml_container_sdl::set_base_url(const char* base_url)
{
    m_base_url = base_url ? base_url : "";
}

void litehtml_container_sdl::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el)
{
    // 链接处理（暂不实现）
}

void litehtml_container_sdl::on_anchor_click(const char* url, const litehtml::element::ptr& el)
{
    printf("Link clicked: %s\n", url);
}

void litehtml_container_sdl::on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event)
{
    // 鼠标事件处理（暂不实现）
}

void litehtml_container_sdl::set_cursor(const char* cursor)
{
    // 光标设置（Switch 不需要）
}

// ========================================
// 其他必需接口实现
// ========================================

void litehtml_container_sdl::transform_text(litehtml::string& text, litehtml::text_transform tt)
{
    switch (tt)
    {
    case litehtml::text_transform_capitalize:
        if (!text.empty())
        {
            text[0] = toupper(text[0]);
        }
        break;
    case litehtml::text_transform_uppercase:
        std::transform(text.begin(), text.end(), text.begin(), ::toupper);
        break;
    case litehtml::text_transform_lowercase:
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        break;
    default:
        break;
    }
}

void litehtml_container_sdl::import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl)
{
    // CSS 导入（暂不实现）
}

void litehtml_container_sdl::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius)
{
    clip_info clip;
    clip.rect = {pos.x, pos.y, pos.width, pos.height};
    clip.active = true;
    m_clips.push_back(clip);
    apply_clip();
}

void litehtml_container_sdl::del_clip()
{
    if (!m_clips.empty())
    {
        m_clips.pop_back();
    }
    
    if (m_clips.empty())
    {
        SDL_RenderSetClipRect(m_renderer, nullptr);
    }
    else
    {
        apply_clip();
    }
}

void litehtml_container_sdl::get_viewport(litehtml::position& viewport) const
{
    int w, h;
    SDL_GetRendererOutputSize(m_renderer, &w, &h);
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = w;
    viewport.height = h;
}

litehtml::element::ptr litehtml_container_sdl::create_element(const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc)
{
    // 创建自定义元素（暂不实现）
    return nullptr;
}

void litehtml_container_sdl::get_media_features(litehtml::media_features& media) const
{
    int w, h;
    SDL_GetRendererOutputSize(m_renderer, &w, &h);
    
    media.type = litehtml::media_type_screen;
    media.width = w;
    media.height = h;
    media.device_width = w;
    media.device_height = h;
    media.color = 8; // 8 bits per color
    media.monochrome = 0;
    media.color_index = 256;
    media.resolution = 96; // 96 DPI
}

void litehtml_container_sdl::get_language(litehtml::string& language, litehtml::string& culture) const
{
    language = "en";
    culture = "";
}

// ========================================
// 辅助函数实现
// ========================================

SDL_Color litehtml_container_sdl::web_color_to_sdl(const litehtml::web_color& color)
{
    SDL_Color result;
    result.r = color.red;
    result.g = color.green;
    result.b = color.blue;
    result.a = color.alpha;
    return result;
}

TTF_Font* litehtml_container_sdl::get_font(litehtml::uint_ptr hFont)
{
    auto it = m_fonts.find(hFont);
    if (it != m_fonts.end())
    {
        return it->second;
    }
    return nullptr;
}

void litehtml_container_sdl::apply_clip()
{
    if (!m_clips.empty())
    {
        const auto& clip = m_clips.back();
        if (clip.active)
        {
            SDL_RenderSetClipRect(m_renderer, &clip.rect);
        }
    }
}

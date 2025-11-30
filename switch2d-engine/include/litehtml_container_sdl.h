#pragma once

#include "litehtml.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <string>

// ============================================
// SDL Container for litehtml
// ============================================
// 这是一个简化的 litehtml::document_container 实现
// 基于 SDL2 和 SDL_ttf，适用于 Nintendo Switch
// ============================================

class litehtml_container_sdl : public litehtml::document_container
{
public:
    litehtml_container_sdl(SDL_Renderer* renderer);
    virtual ~litehtml_container_sdl();
    
    // 设置字体路径
    void set_font_path(const char* path) { m_font_path = path; }
    
    // ========================================
    // 字体相关
    // ========================================
    litehtml::uint_ptr create_font(const litehtml::font_description& descr, const litehtml::document* doc, litehtml::font_metrics* fm) override;
    void delete_font(litehtml::uint_ptr hFont) override;
    litehtml::pixel_t text_width(const char* text, litehtml::uint_ptr hFont) override;
    void draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;
    litehtml::pixel_t pt_to_px(float pt) const override;
    litehtml::pixel_t get_default_font_size() const override;
    const char* get_default_font_name() const override;
    
    // ========================================
    // 绘制相关
    // ========================================
    void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
    void draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::web_color& color) override;
    void draw_linear_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::background_layer::linear_gradient& gradient) override;
    void draw_radial_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::background_layer::radial_gradient& gradient) override;
    void draw_conic_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const litehtml::background_layer::conic_gradient& gradient) override;
    void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;
    
    // ========================================
    // 图像相关
    // ========================================
    void load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
    void get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
    void draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const std::string& url, const std::string& base_url) override;
    
    // ========================================
    // 链接和事件
    // ========================================
    void set_caption(const char* caption) override;
    void set_base_url(const char* base_url) override;
    void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;
    void on_anchor_click(const char* url, const litehtml::element::ptr& el) override;
    void on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) override;
    void set_cursor(const char* cursor) override;
    
    // ========================================
    // 其他必需接口
    // ========================================
    void transform_text(litehtml::string& text, litehtml::text_transform tt) override;
    void import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) override;
    void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
    void del_clip() override;
    void get_viewport(litehtml::position& viewport) const override;
    litehtml::element::ptr create_element(const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc) override;
    void get_media_features(litehtml::media_features& media) const override;
    void get_language(litehtml::string& language, litehtml::string& culture) const override;
    
private:
    SDL_Renderer* m_renderer;
    std::string m_font_path;
    std::string m_base_url;
    std::string m_caption;
    
    // 字体缓存
    std::map<litehtml::uint_ptr, TTF_Font*> m_fonts;
    litehtml::uint_ptr m_next_font_id;
    
    // 图像缓存
    struct image_info
    {
        SDL_Texture* texture;
        int width;
        int height;
    };
    std::map<std::string, image_info> m_images;
    
    // 裁剪区域栈
    struct clip_info
    {
        SDL_Rect rect;
        bool active;
    };
    std::vector<clip_info> m_clips;
    
    // 辅助函数
    SDL_Color web_color_to_sdl(const litehtml::web_color& color);
    TTF_Font* get_font(litehtml::uint_ptr hFont);
    void apply_clip();
};

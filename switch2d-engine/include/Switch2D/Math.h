#pragma once
#include <cmath>
#include <algorithm>

namespace Switch2D {

// ============================================
// Vector2 - 2D向量
// ============================================
struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}
    
    Vector2 operator+(const Vector2& v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2& v) const { return {x - v.x, y - v.y}; }
    Vector2 operator*(float s) const { return {x * s, y * s}; }
    Vector2 operator/(float s) const { return {x / s, y / s}; }
    
    Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
    Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    Vector2& operator*=(float s) { x *= s; y *= s; return *this; }
    
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
    
    Vector2 normalized() const {
        float len = length();
        return len > 0 ? *this / len : Vector2(0, 0);
    }
    
    float dot(const Vector2& v) const { return x * v.x + y * v.y; }
    
    static Vector2 Zero() { return {0, 0}; }
    static Vector2 One() { return {1, 1}; }
    static Vector2 Up() { return {0, -1}; }
    static Vector2 Down() { return {0, 1}; }
    static Vector2 Left() { return {-1, 0}; }
    static Vector2 Right() { return {1, 0}; }
};

// ============================================
// Rectangle - 矩形
// ============================================
struct Rectangle {
    float x, y, width, height;
    
    Rectangle() : x(0), y(0), width(0), height(0) {}
    Rectangle(float _x, float _y, float w, float h) : x(_x), y(_y), width(w), height(h) {}
    
    float left() const { return x; }
    float right() const { return x + width; }
    float top() const { return y; }
    float bottom() const { return y + height; }
    
    Vector2 center() const { return {x + width / 2, y + height / 2}; }
    
    bool contains(const Vector2& point) const {
        return point.x >= x && point.x <= x + width &&
               point.y >= y && point.y <= y + height;
    }
    
    bool intersects(const Rectangle& other) const {
        return !(right() < other.left() || left() > other.right() ||
                 bottom() < other.top() || top() > other.bottom());
    }
};

// ============================================
// Color - 颜色
// ============================================
struct Color {
    uint8_t r, g, b, a;
    
    Color() : r(255), g(255), b(255), a(255) {}
    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) 
        : r(_r), g(_g), b(_b), a(_a) {}
    
    static Color White() { return {255, 255, 255, 255}; }
    static Color Black() { return {0, 0, 0, 255}; }
    static Color Red() { return {255, 0, 0, 255}; }
    static Color Green() { return {0, 255, 0, 255}; }
    static Color Blue() { return {0, 0, 255, 255}; }
    static Color Yellow() { return {255, 255, 0, 255}; }
    static Color Cyan() { return {0, 255, 255, 255}; }
    static Color Magenta() { return {255, 0, 255, 255}; }
    static Color Transparent() { return {0, 0, 0, 0}; }
};

// ============================================
// 数学工具函数
// ============================================
namespace Math {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;
    
    inline float Clamp(float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }
    
    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    inline Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
        return {Lerp(a.x, b.x, t), Lerp(a.y, b.y, t)};
    }
}

} // namespace Switch2D

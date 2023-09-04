#include <math.h>

struct vec2d
{
    float x = 0;
    float y = 0;

    vec2d operator+(const vec2d &rhs)
    {
        return {x + rhs.x, y + rhs.y};
    }

    vec2d operator-(const vec2d &rhs)
    {
        return {x - rhs.x, y - rhs.y};
    }

    void operator+=(const vec2d &rhs)
    {
        x += rhs.x;
        y += rhs.y;
    }

    void operator-=(const vec2d &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
    }

    vec2d operator*(const float &rhs)
    {
        return {x * rhs, y * rhs};
    }

    vec2d operator/(const float &rhs)
    {
        return {x / rhs, y / rhs};
    }

    void operator*=(const float &rhs)
    {
        x *= rhs;
        y *= rhs;
    }

    void operator/=(const float &rhs)
    {
        x /= rhs;
        y /= rhs;
    }

    float GetLengthSqared()
    {
        return x * x + y * y;
    }

    float GetLength()
    {
        float l2 = GetLengthSqared();
        float out = sqrtf(l2);
        return out;
    }

    void Normalize()
    {
        float l = GetLength();
        if (l > 0.0f)
        {
            float reverseL = 1.0f / l;
            x *= reverseL;
            y *= reverseL;
        }
    }

    float DotProduct(const vec2d &rhs)
    {
        return x * rhs.x + y * rhs.y;
    }

    float CrossProduct(const vec2d &rhs)
    {
        return x * rhs.y - y * rhs.x;
    }
};


struct camera
{
    vec2d origin;
    vec2d center;
    float zoom;

    float smooth = 0.95f; // how smooth is the movement?
    vec2d *target = nullptr;

    int nHalfScrW;
    int nHalfScrH;

    void Construct(vec2d *target, int nScreenWidth, int nScreenHeight, float zoom = 1.0f, float smooth = 0.95f)
    {
        this->target = target;
        this->center = *target;
        nHalfScrW = nScreenWidth * 0.5f;
        nHalfScrH = nScreenHeight * 0.5f;
        this->origin = {center.x - nHalfScrW / zoom, center.y - nHalfScrH / zoom};
        this->zoom = zoom;
        this->smooth = smooth;
    }

    void Update(float zoom)
    {
        this->zoom = zoom;
        if (this->zoom < 0.01f)
            this->zoom = 0.01f;
        vec2d diff = *target - center;
        center += diff * smooth;
        this->origin = {center.x - nHalfScrW / this->zoom, center.y - nHalfScrH / this->zoom};
    }

    vec2d Project(vec2d p)
    {
        vec2d p_projected = p;
        p_projected -= origin;
        p_projected *= zoom;
        return p_projected;
    }
};
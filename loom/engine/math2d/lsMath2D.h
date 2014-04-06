#pragma once

class lua_State;

namespace lsMath2D
{

    class LineSegment
    {
    public:
        LineSegment();
        
        LineSegment(const float& x1, const float& y1, const float& x2, const float& y2);
        
        LineSegment(const LineSegment& original);
        
        void setP1(const float x1, const float y1);
        
        void setP2(const float x2, const float y2);
        
        void rotateBy(const float thetaRadians);
        
        float getX1() const { return _x1; }
        float getY1() const { return _y1; }
        float getX2() const { return _x2; }
        float getY2() const { return _y2; }
        float getDX() const { return _dx; }
        float getDY() const { return _dy; }

    private:
        float _x1;
        float _y1;
        float _x2;
        float _y2;
        float _dx;
        float _dy;
        
        void _updateDirection();
    };

    struct RaycastResult
    {
        bool hit;
        int hitIndex;
        float hitX;
        float hitY;
        float t;

        RaycastResult() : hit(false), hitIndex(-1), hitX(0.0f), hitY(0.0f), t(0.0f) {}

        void setHit(const bool h) { hit = h; }
        bool getHit() const { return hit; }

        void setHitIndex(const int hi) { hitIndex = hi; }
        int getHitIndex() const { return hitIndex; }

        void setHitX(const float hx) { hitX = hx; }
        float getHitX() const { return hitX; }

        void setHitY(const float hy) { hitY = hy; }
        float getHitY() const { return hitY; }

        void setT(const float newT) { t = newT; }
        float getT() const { return t; }

        static void opAssignment(RaycastResult* a, RaycastResult* b) 
        {
            *a = *b;
        }
    };

    class Math2D
    {
    public:
        static float dotProduct(const float& x1, const float& y1, const float& x2, const float& y2);

        static void rotateVector(float& inOutX, float& inOutY, const float& thetaRadians);

        static float intersectRayWithSegment(const LineSegment& ray, const LineSegment& seg);

        static int raycastToSegments(lua_State* L);
    };

}
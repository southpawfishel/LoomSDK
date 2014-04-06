#include "loom/engine/math2d/lsMath2D.h"

#include "loom/script/loomscript.h"
#include "loom/script/runtime/lsRuntime.h"

using namespace LS;
using namespace lsMath2D;

extern "C"
{
#include "math.h"
#include "float.h"
}

LineSegment::LineSegment()
: _x1(0.0f)
, _y1(0.0f)
, _x2(0.0f)
, _y2(0.0f)
, _dx(0.0f)
, _dy(0.0f)
{
}
    
LineSegment::LineSegment(const float& x1, const float& y1, const float& x2, const float& y2)
: _x1(x1)
, _y1(y1)
, _x2(x2)
, _y2(y2)
, _dx(0.0f)
, _dy(0.0f)
{
    _updateDirection();
}
        
LineSegment::LineSegment(const LineSegment& original)
: _x1(original._x1)
, _y1(original._y1)
, _x2(original._x2)
, _y2(original._y2)
, _dx(original._dx)
, _dy(original._dy)
{
}
    
void LineSegment::setP1(const float x1, const float y1)
{
    _x1 = x1;
    _y1 = y1;
    _updateDirection();
}

void LineSegment::setP2(const float x2, const float y2)
{
    _x2 = x2;
    _y2 = y2;
    _updateDirection();
}

void LineSegment::rotateBy(const float thetaRadians)
{
    Math2D::rotateVector(_dx, _dy, thetaRadians);
    _x2 = _x1 + _dx;
    _y2 = _y1 + _dy;
}

void LineSegment::_updateDirection()
{
    _dx = _x2 - _x1;
    _dy = _y2 - _y1;
}

float Math2D::dotProduct(const float& x1, const float& y1, const float& x2, const float& y2)
{
    return (x1 * x2) + (y1 * y2);
}

void Math2D::rotateVector(float& inOutX, float& inOutY, const float& thetaRadians)
{
    const float sinTheta = sin(thetaRadians);
    const float cosTheta = cos(thetaRadians);
    const float tempX = inOutX;
    const float tempY = inOutY;
    inOutX = (tempX * cosTheta) + (tempY * sinTheta);
    inOutY = (tempX * sinTheta) + (tempY * cosTheta);
}

float Math2D::intersectRayWithSegment(const LineSegment& ray, const LineSegment& seg)
{
    const float t2 = (ray.getDX() * (seg.getY1() - ray.getY1()) + ray.getDY() * (ray.getX1() - seg.getX1())) /
                     (seg.getDX() * ray.getDY() - seg.getDY() * ray.getDX());
    const float t1 = (seg.getX1() + (seg.getDX() * t2) - ray.getX1()) / ray.getDX();
    
    if (t1 >= 0.0f && t2 >= 0.0f && t2 <= 1.0f)
    {
        return t1;
    }
    
    return -1.0f;
}

int Math2D::raycastToSegments(lua_State* L)
{
	float tempT = -1.0f;

	const int RAY_STACK_POS = 1;
	const int OUTSTRUCT_STACK_POS = 2;
	const int VECTOR_STACK_POS = 3;

    LineSegment* ray = (LineSegment*)lualoom_getnativepointer(L, RAY_STACK_POS);
    RaycastResult* result = (RaycastResult*)lualoom_getnativepointer(L, OUTSTRUCT_STACK_POS);
    result->t = FLT_MAX;

    // Loop over each segment in the vector and cast ray against it.
    if ((lua_gettop(L) == VECTOR_STACK_POS) && !lua_isnil(L, VECTOR_STACK_POS))
    {
        // Get the length of the vector on the top of the stack
        const int numSegments = lsr_vector_get_length(L, VECTOR_STACK_POS);

        // Load up the vector table into the stack position where the vector sits
        lua_rawgeti(L, VECTOR_STACK_POS, LSINDEXVECTOR);

        // The actual work: loop over every segment in the vector and run the raycast against it
        for (int i = 0; i < numSegments; ++i)
        {
            lua_rawgeti(L, VECTOR_STACK_POS+1, i);
		    LineSegment* seg = (LineSegment*)lualoom_getnativepointer(L, -1);

		    tempT = intersectRayWithSegment(*ray, *seg);
		    if (tempT != -1.0f && tempT < result->t)
		    {
		    	result->hit = true;
		    	result->hitIndex = i;
		    	result->t = tempT;
		    }

            lua_pop(L, 1);
        }
    }

    if (result->hit)
    {
	    result->setHitX(ray->getX1() + (ray->getDX() * result->t));
	    result->setHitY(ray->getY1() + (ray->getDY() * result->t));
    }

    return 0;
}

static int registerLoomMath2D(lua_State *L)
{
    beginPackage(L, "math2d")

        .beginClass<LineSegment>("LineSegment")
        .addConstructor<void (*)(const float, const float, const float, const float)>()
        .addMethod("setP1", &LineSegment::setP1)
        .addMethod("setP2", &LineSegment::setP2)
        .addMethod("rotateBy", &LineSegment::rotateBy)
        .addVarAccessor("x1", &LineSegment::getX1)
        .addVarAccessor("y1", &LineSegment::getY1)
        .addVarAccessor("x2", &LineSegment::getX2)
        .addVarAccessor("y2", &LineSegment::getY2)
        .addVarAccessor("dx", &LineSegment::getDX)
        .addVarAccessor("dy", &LineSegment::getDY)
        .endClass()

        .beginClass<RaycastResult>("RaycastResult")
        .addConstructor<void (*)(void)>()
        .addVarAccessor("hit", &RaycastResult::getHit, &RaycastResult::setHit)
        .addVarAccessor("hitIndex", &RaycastResult::getHitIndex, &RaycastResult::setHitIndex)
        .addVarAccessor("hitX", &RaycastResult::getHitX, &RaycastResult::setHitX)
        .addVarAccessor("hitY", &RaycastResult::getHitY, &RaycastResult::setHitY)
        .addVarAccessor("t", &RaycastResult::getT, &RaycastResult::setT)
        .endClass()

        .beginClass<Math2D>("Math2D")
        .addStaticMethod("dotProduct", &Math2D::dotProduct)
        .addStaticMethod("rotateVector", &Math2D::rotateVector)
        .addStaticMethod("intersectRayWithSegment", &Math2D::intersectRayWithSegment)
        .addStaticLuaFunction("raycastToSegments", &Math2D::raycastToSegments)
        .endClass()

    .endPackage();

    return 0;
}


void installLoomMath2D()
{
    LOOM_DECLARE_MANAGEDNATIVETYPE(LineSegment, registerLoomMath2D);
    LOOM_DECLARE_MANAGEDNATIVETYPE(RaycastResult, registerLoomMath2D);
    LOOM_DECLARE_NATIVETYPE(Math2D, registerLoomMath2D);
}

void installPackageMath2D()
{
    installLoomMath2D();
}
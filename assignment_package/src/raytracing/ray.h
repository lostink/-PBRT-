#pragma once
#include <globals.h>

class Medium;

class Ray
{
public:
    Ray(const glm::vec4 &o, const glm::vec4 &d);
    Ray(const Ray &r);
    Ray(const Point3f &o, const Vector3f &d, float tMax = INFINITY, float t = 0.f, const Medium* medium = nullptr);
    //Return a copy of this ray that has been transformed
    //by the input transformation matrix.
    Ray GetTransformedCopy(const Matrix4x4 &T) const;

    Point3f origin;
    Vector3f direction;
    float time;
    mutable float tMax;
    const Medium *medium;
};

#include "bounds.h"

bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    //TODO
    float t0 = 0.f, t1 = 999999999999999.f;
    for(int i = 0; i < 3; ++i)
    {
        float invRayDir = 1 / r.direction[i];
        float tNear = (min[i] - r.origin[i]) * invRayDir;
        float tFar = (max[i] - r.origin[i]) * invRayDir;
        if(tNear > tFar)
            std::swap(tNear, tFar);
        tFar *= 1 + 2 * ((3 * RayEpsilon) / (1 - 3 * RayEpsilon));
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if(t0 > t1)
            return false;
    }
    *t = t0;
    return true;
}

Bounds3f Bounds3f::Apply(const Transform &tr)
{
    //TODO
    Bounds3f ret, retnow;
    glm::vec4 temp;

    temp = glm::vec4(tr.T() * glm::vec4(min.x, min.y, min.z, 1));//000
    ret = Point3f(temp[0], temp[1], temp[2]);

    temp = glm::vec4(tr.T() * glm::vec4(max.x, min.y, min.z, 1));//100
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    temp = glm::vec4(tr.T() * glm::vec4(min.x, max.y, min.z, 1));//010
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    temp = glm::vec4(tr.T() * glm::vec4(min.x, min.y, max.z, 1));//001
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    temp = glm::vec4(tr.T() * glm::vec4(min.x, max.y, max.z, 1));//011
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    temp = glm::vec4(tr.T() * glm::vec4(max.x, max.y, min.z, 1));//110
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    temp = glm::vec4(tr.T() * glm::vec4(max.x, min.y, max.z, 1));//101
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    temp = glm::vec4(tr.T() * glm::vec4(max.x, max.y, max.z, 1));//111
    retnow = Point3f(temp[0], temp[1], temp[2]);
    ret = Union(ret, retnow);

    return ret;
}

float Bounds3f::SurfaceArea() const
{
    //TODO
    Point3f d(max - min);
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2)
{
    return Bounds3f(Point3f(std::min(b1.min.x, b2.min.x),
                            std::min(b1.min.y, b2.min.y),
                            std::min(b1.min.z, b2.min.z)),
                    Point3f(std::max(b1.max.x, b2.max.x),
                            std::max(b1.max.y, b2.max.y),
                            std::max(b1.max.z, b2.max.z)));
}

Bounds3f Union(const Bounds3f& b1, const Point3f& p)
{
    return Bounds3f(Point3f(std::min(b1.min.x, p.x),
                            std::min(b1.min.y, p.y),
                            std::min(b1.min.z, p.z)),
                    Point3f(std::max(b1.max.x, p.x),
                            std::max(b1.max.y, p.y),
                            std::max(b1.max.z, p.z)));
}

Bounds3f Union(const Bounds3f& b1, const glm::vec4& p)
{
    return Union(b1, Point3f(p));
}

bool Bounds3f::IntersectP(const Ray &ray, const Vector3f &invDir,
                       const int dirIsNeg[3]) const {
    const Bounds3f &bounds = *this;
    // Check for ray intersection against $x$ and $y$ slabs
    Float tMin = (bounds[dirIsNeg[0]].x - ray.origin.x) * invDir.x;
    Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.origin.x) * invDir.x;
    Float tyMin = (bounds[dirIsNeg[1]].y - ray.origin.y) * invDir.y;
    Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.origin.y) * invDir.y;

    // Update _tMax_ and _tyMax_ to ensure robust bounds intersection
    tMax *= 1 + 2 * gamma(3);
    tyMax *= 1 + 2 * gamma(3);
    if (tMin > tyMax || tyMin > tMax) return false;
    if (tyMin > tMin) tMin = tyMin;
    if (tyMax < tMax) tMax = tyMax;

    // Check for ray intersection against $z$ slab
    Float tzMin = (bounds[dirIsNeg[2]].z - ray.origin.z) * invDir.z;
    Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.origin.z) * invDir.z;

    // Update _tzMax_ to ensure robust bounds intersection
    tzMax *= 1 + 2 * gamma(3);
    if (tMin > tzMax || tzMin > tMax) return false;
    if (tzMin > tMin) tMin = tzMin;
    if (tzMax < tMax) tMax = tzMax;
    return (tMax > 0);
}

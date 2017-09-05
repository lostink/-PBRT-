#include <raytracing/ray.h>


Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(Point3f(o), Vector3f(d), INFINITY, -1.0f, medium)
{}

Ray::Ray(const Ray &r):
    Ray(r.origin, r.direction, r.tMax, r.time, r.medium)
{}

Ray::Ray(const Point3f &o, const Vector3f &d, float tMax, float t, const Medium* medium)
    :origin(o), direction(d), time(t), tMax(tMax), medium(medium)
{}


Ray Ray::GetTransformedCopy(const Matrix4x4 &T) const
{

    glm::vec4 o = glm::vec4(origin, 1);
    glm::vec4 d = glm::vec4(direction, 0);

    o = T * o;
    d = T * d;

    return Ray(o, d);
}

#pragma once
#include <globals.h>
#include <medium.h>
#include <scene/materials/bsdf.h>
#include <raytracing/ray.h>
#include <scene/geometry/primitive.h>
#include <QList>
#include <raytracing/parentintersection.h>


class Material;
class BSDF;
class Primitive;
class Medium;



class Intersection : public ParentIntersection
{
public:
    Intersection() : ParentIntersection(),
        uv(Point2f(0.f)), objectHit(nullptr),
        bsdf(nullptr), tangent(Vector3f(0.f)), bitangent(Vector3f(0.f)){}
    Intersection(const Point3f &p, const Vector3f &wo, Float time,
                const MediumInterface &mediumInterface)
        : ParentIntersection(p, time, wo, mediumInterface),
          uv(Point2f(0.f)), objectHit(nullptr),
          bsdf(nullptr), tangent(Vector3f(0.f)), bitangent(Vector3f(0.f)){}

    // Ask _objectHit_ to produce a BSDF
    // based on other data stored in this
    // Intersection, e.g. the surface normal
    // and UV coordinates
    bool ProduceBSDF();

    // Returns the light emitted from this Intersection
    // along _wo_, provided that this Intersection is
    // on the surface of an AreaLight. If not, then
    // zero light is returned.
    Color3f Le(const Vector3f& wo) const;

    // Instantiate a Ray that originates from this Intersection and
    // travels in direction d.


    Point2f uv;             // The UV coordinates computed at the intersection

    Primitive const * objectHit;     // The object that the ray intersected, or nullptr if the ray hit nothing.
    std::shared_ptr<BSDF> bsdf;// The Bidirection Scattering Distribution Function found at the intersection.

    Vector3f tangent, bitangent; // World-space vectors that form an orthonormal basis with the surface normal.
};





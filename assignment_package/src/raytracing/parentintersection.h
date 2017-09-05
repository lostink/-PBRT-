#ifndef PARENTINTERSECTION_H
#define PARENTINTERSECTION_H

#include <globals.h>
#include <medium.h>

class ParentIntersection
{
public:
    // Interaction Public Methods
    ParentIntersection() : t(0) {}
    ParentIntersection(const Point3f &p,
                Float time, const Vector3f &wo,
                const MediumInterface &mediumInterface)
        : point(p),
          t(time),
          pError(Vector3f(0.f)),
          wo(glm::normalize(wo)),
          normalGeometric(Vector3f(0.f)),
          mediumInterface(mediumInterface) {}
    bool IsSurfaceInteraction() const { return normalGeometric != Normal3f(); }
    Ray SpawnRay(const Vector3f &d) const {
        Vector3f originOffset = normalGeometric * RayEpsilon;
        originOffset = (glm::dot(d, normalGeometric) > 0) ? originOffset : -originOffset;
        Point3f o(this->point + originOffset);
        return Ray(o, d, INFINITY, t, GetMedium(d));
    }
    Ray SpawnRayTo(const ParentIntersection &it) const {
        Vector3f originOffset = normalGeometric * RayEpsilon;
        Vector3f d = it.point - this->point;
        originOffset = (glm::dot(d, normalGeometric) > 0) ? originOffset : -originOffset;
        Point3f o(this->point + originOffset);
        Vector3f originOffset2 = it.normalGeometric * RayEpsilon;
        Vector3f d2 = this->point - it.point;
        originOffset2 = (glm::dot(d2, it.normalGeometric) > 0) ? originOffset2 : -originOffset2;
        Point3f o2(it.point + originOffset2);
        Vector3f d3 = o2 - o;
        return Ray(o, d3, 1 - ShadowEpsilon, t, GetMedium(d3));
    }
    ParentIntersection(const Point3f &p, Float time,
                const MediumInterface &mediumInterface)
        : point(p), t(time), mediumInterface(mediumInterface) {}
    bool IsMediumInteraction() const { return !IsSurfaceInteraction(); }
    const Medium *GetMedium(const Vector3f &w) const {
        return glm::dot(w, normalGeometric) > 0 ? mediumInterface.outside : mediumInterface.inside;
    }
    const Medium *GetMedium() const {
        if(mediumInterface.inside == mediumInterface.outside)
            return mediumInterface.inside;
    }

    // Interaction Public Data
    Point3f point;
    Float t;
    Vector3f pError;
    Vector3f wo;
    Normal3f normalGeometric;
    MediumInterface mediumInterface;
};

#endif // PARENTINTERSECTION_H

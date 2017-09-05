#include "pointlight.h"

Color3f PointLight::Sample_Li(const ParentIntersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf, ParentIntersection *p1) const
{
    *wi = -glm::normalize(shape->transform.position() - ref.point);
    *pdf = 1.f;
    return I / glm::distance2(pLight, ref.point);
}
Color3f PointLight::L(const ParentIntersection &isect, const Vector3f &w) const{
    return I;
}
float PointLight::Pdf_Li(const ParentIntersection &ref, const Vector3f &wi) const{
    return 1.f;
}

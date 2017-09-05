#include "spotlight.h"

float SpotLight::Falloff(const Vector3f &w) const
{

    Vector3f wl = glm::normalize(glm::vec3(transform.invT() * glm::vec4(w, 0.0f)));
    float cosTheta = wl.z;
    if(cosTheta < cosTotalWidth) return 0.f;
    if(cosTheta > cosFalloffStart) return 1.0f;

    //Compute falloff inside spotlight cone
    float delta = (cosTheta - cosTotalWidth) /
            (cosFalloffStart - cosTotalWidth);
    return (delta * delta) * (delta * delta);
}

Color3f SpotLight::Sample_Li(const ParentIntersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf, ParentIntersection *p1) const
{
    *wi = -glm::normalize(shape->transform.position() - ref.point);
    *pdf = 1.f;
    return I * Falloff(*wi) / glm::distance2(pLight, ref.point);
}
Color3f SpotLight::L(const ParentIntersection &isect, const Vector3f &w) const{
    return I;
}
float SpotLight::Pdf_Li(const ParentIntersection &ref, const Vector3f &wi) const{
    return 1.f;
}

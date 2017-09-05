#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const ParentIntersection &isect, const Vector3f &w) const
{
    //TODO
    if(twoSided)
        return emittedLight;
    else
        return glm::dot(isect.normalGeometric, w) > 0.f? emittedLight : glm::vec3(0.0f);
}

Color3f DiffuseAreaLight::Sample_Li(const ParentIntersection &ref, const Point2f &xi,
                                         Vector3f *wi, Float *pdf, ParentIntersection *p1) const
{
    ParentIntersection pShape = shape->Sample(ref, xi, pdf);
    *p1 = pShape;
    if((*pdf == 0) || ((abs(glm::length(ref.point - pShape.point))) < RayEpsilon))
        return Color3f(0.0f);
    *wi = glm::normalize(ref.point - pShape.point);
    return L(pShape, *wi);
}

float DiffuseAreaLight::Pdf_Li(const ParentIntersection &ref, const Vector3f &wi) const
{
    return shape->Pdf(ref, wi);
}

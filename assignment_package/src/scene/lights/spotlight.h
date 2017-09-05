#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include <scene/lights/light.h>

class SpotLight : public AreaLight
{
public:
    SpotLight(const Transform &LightToWorld, const Color3f &I,
              const std::shared_ptr<Shape>& shape,
              float totalWidth, float falloffStart)
        :AreaLight(LightToWorld), pLight(transform.T() * glm::vec4(0,0,0,1)), I(I),
          shape(shape),
          cosTotalWidth(std::cos(totalWidth * Pi / 180)),
          cosFalloffStart(std::cos(falloffStart * Pi / 180))
        {}

    float Falloff(const Vector3f &w) const;

    virtual Color3f Sample_Li(const ParentIntersection &ref, const Point2f &xi,
                      Vector3f *wi, Float *pdf, ParentIntersection* p1) const;
    virtual Color3f L(const ParentIntersection &isect, const Vector3f &w) const;
    virtual float Pdf_Li(const ParentIntersection &ref, const Vector3f &wi) const;

    Color3f Power() const
    {
        return I * 2.0f * Pi * (1.0f - .5f * (cosFalloffStart + cosTotalWidth));
    }
    std::shared_ptr<Shape> shape;
private:
    const Point3f pLight;
    const Color3f I;
    const float cosTotalWidth, cosFalloffStart;
};

#endif // SPOTLIGHT_H

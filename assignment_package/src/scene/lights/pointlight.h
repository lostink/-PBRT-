#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <scene/lights/light.h>


class PointLight : public AreaLight
{
public:
    PointLight(const Transform &LightToWorld, const Color3f &I,
               const std::shared_ptr<Shape>& shape)
        :AreaLight(LightToWorld),shape(shape),pLight(Point3f(LightToWorld.T() * glm::vec4(0,0,0,1))), I(I){}

    virtual Color3f Sample_Li(const ParentIntersection &ref, const Point2f &xi,
                      Vector3f *wi, Float *pdf, ParentIntersection* p1) const;

    Color3f Power() const {return 4 * Pi * I;}
    std::shared_ptr<Shape> shape;
    virtual Color3f L(const ParentIntersection &isect, const Vector3f &w) const;
    virtual float Pdf_Li(const ParentIntersection &ref, const Vector3f &wi) const;
private:
    const Point3f pLight;
    const Color3f I;
};

#endif // POINTLIGHT_H

#ifndef DISTANTLIGHT_H
#define DISTANTLIGHT_H

#include <scene/lights/light.h>
#include <scene/scene.h>

class DistantLight : public Light
{
public:
    DistantLight(const Transform &LightToWorld, const Color3f &L, const Vector3f &wLight)
        :Light(LightToWorld), L(L), wLight(glm::normalize(transform.T() * glm::vec4(wLight,1.0f))){}

    void Preprocess(const Scene &scene)
    {
        //scene.WorldBound().BoundingSphere(&worldCenter, &worldRadius);
    }

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                      Vector3f *wi, Float *pdf) const;

    Color3f Power() const {return L * Pi * worldRadius * worldRadius;}

private:
    const Color3f L;
    const Vector3f wLight;
    Point3f worldCenter;
    float worldRadius;
};

#endif // DISTANTLIGHT_H

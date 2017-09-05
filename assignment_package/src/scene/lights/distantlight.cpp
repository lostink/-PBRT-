#include "distantlight.h"

Color3f DistantLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                  Vector3f *wi, Float *pdf) const
{
    *wi = wLight;
    *pdf = 1.f;
    return L;
}


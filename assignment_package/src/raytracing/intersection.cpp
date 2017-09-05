#include <raytracing/intersection.h>

bool Intersection::ProduceBSDF()
{
    return objectHit->ProduceBSDF(this);
}

Color3f Intersection::Le(const Vector3f &wo) const
{
    const AreaLight* light = objectHit->GetAreaLight();
    return light ? light->L(*this, wo) : Color3f(0.f);
}

/*
SurfaceInteraction::SurfaceInteraction(
    const Point3f &p, const Vector3f &pError, const Point2f &uv,
    const Vector3f &wo, const Vector3f &dpdu, const Vector3f &dpdv,
    const Normal3f &dndu, const Normal3f &dndv, Float time, const Shape *shape)
    : Intersection(p, Normal3f(glm::normalize(glm::cross(dpdu, dpdv))), pError, wo, time,
                  nullptr),
      uv(uv),
      dpdu(dpdu),
      dpdv(dpdv),
      dndu(dndu),
      dndv(dndv),
      shape(shape) {
    // Initialize shading geometry from true geometry
    shading.n = n;
    shading.dpdu = dpdu;
    shading.dpdv = dpdv;
    shading.dndu = dndu;
    shading.dndv = dndv;

    // Adjust normal based on orientation and handedness
    if (shape &&
        (shape->reverseOrientation ^ shape->transformSwapsHandedness)) {
        n *= -1;
        shading.n *= -1;
    }
}

void SurfaceInteraction::SetShadingGeometry(const Vector3f &dpdus,
                                            const Vector3f &dpdvs,
                                            const Normal3f &dndus,
                                            const Normal3f &dndvs,
                                            bool orientationIsAuthoritative) {
    // Compute _shading.n_ for _SurfaceInteraction_
    shading.n = glm::normalize((Normal3f)glm::cross(dpdus, dpdvs));
    if (shape && (shape->reverseOrientation ^ shape->transformSwapsHandedness))
        shading.n = -shading.n;
    if (orientationIsAuthoritative)
        n = Faceforward(n, shading.n);
    else
        shading.n = Faceforward(shading.n, n);

    // Initialize _shading_ partial derivative values
    shading.dpdu = dpdus;
    shading.dpdv = dpdvs;
    shading.dndu = dndus;
    shading.dndv = dndvs;
}
*/

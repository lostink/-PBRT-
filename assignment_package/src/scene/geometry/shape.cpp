#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const ParentIntersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    *pdf = 0.0f;
    Intersection result = Sample(xi, pdf);
    Vector3f wi = ref.point - result.point;

    //For sphere. If sampled back face we need to sample front cap
    if (glm::dot(wi,result.normalGeometric) < 0){
        Point3f refInLocal = glm::vec3(transform.invT() * glm::vec4(ref.point,1));
        float RefToSphere = glm::length(refInLocal - Point3f(0,0,0));
        float angle = (1.0f - glm::acos(1.0f / RefToSphere) / Pi) * 180.0f;
        Intersection result = Sample(xi, &angle);
        *pdf = angle;
        wi = glm::normalize(ref.point - result.point);
    }
    //Sphere part end.

    if((AbsDot(result.normalGeometric, -wi) * Area()) == 0)
        *pdf = 0.0f;
    else
        *pdf = glm::length2(ref.point - result.point) /
            (AbsDot(result.normalGeometric, -glm::normalize(wi)) * Area());
    return result;
}

float Shape::Pdf(const ParentIntersection &ref, const Vector3f &wi) const
{
    Ray ray = ref.SpawnRay(wi);
    Intersection isectLight;
    if(!Intersect(ray, &isectLight))
        return 0.0f;
    float pdf = glm::length(ref.point - isectLight.point) * glm::length(ref.point - isectLight.point) /
            (AbsDot(isectLight.normalGeometric, -glm::normalize(wi)) * Area());
    return pdf;
}

#include "microfacetbrdf.h"

Color3f MicrofacetBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO!
    float cosTheta0 = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
    Vector3f wh = wi + wo;

    //Handle degenerate cases for microfacet reflection
    if(cosThetaI == 0 || cosTheta0 == 0)
        return Color3f(0.);
    if(wh.x == 0 && wh.y == 0 && wh.z == 0)
        return Color3f(0.);

    wh = glm::normalize(wh);
    Color3f F = fresnel->Evaluate(glm::dot(wi,wh));
    return R * distribution->D(wh) * distribution->G(wo,wi) * F/
            (4 * cosThetaI * cosTheta0);
}

Color3f MicrofacetBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    //TODO
    //Sample microfacet orientation wh and reflected direction wi
    Vector3f wh = distribution->Sample_wh(wo, xi);
    *wi = -wo - glm::dot(-wo,wh) * 2.0f * wh;
    if(!SameHemisphere(wo, *wi))
        return Color3f(0.f);

    //Compute PDF of wi for microfacet reflection
    *pdf = distribution->Pdf(wo, wh) / (4 * glm::dot(wo, wh));

    return f(wo, *wi);
}

float MicrofacetBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    if(!SameHemisphere(wo, wi))
        return 0;
    Vector3f wh = glm::normalize(wo + wi);
    return distribution->Pdf(wo, wh) / (4 * glm::dot(wo, wh));
}

#include "fresnel.h"

//----------------Fresnel Dielectric----------------
float FrDielectric(float cosThetaI, float etaI, float etaT)
{
    cosThetaI = glm::clamp(cosThetaI, -1.0f, 1.0f);
    bool entering = cosThetaI > 0.f;
    //Potentially swap indices of refraction
    if(!entering)
    {
        std::swap(etaI, etaT);
        cosThetaI = std::abs(cosThetaI);
    }
    //Compute cosThetaT using Snell's law
    float sinThetaI = std::sqrt(std::max(0.0f, 1 - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;
    if(sinThetaT >= 1)
        return 1.0f;
    float cosThetaT = std::sqrt(std::max(0.0f, 1 - sinThetaT * sinThetaT));
    float Rparl =   ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                    ((etaT * cosThetaI) + (etaI * cosThetaT));
    float Rperp =   ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                    ((etaI * cosThetaI) + (etaT * cosThetaT));
    return (Rparl * Rparl + Rperp * Rperp) / 2;
}

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO   
    float Fr = FrDielectric(cosThetaI, etaI, etaT);
    return Color3f(Fr);
}

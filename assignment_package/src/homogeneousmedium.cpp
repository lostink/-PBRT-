#include "homogeneousmedium.h"

Color3f HomogeneousMedium::Sample(const Ray &ray, Sampler &sampler, MediumInteraction *mi) const
{
    //Sample a channel and distance along the ray
    int channel = std::min((int)(sampler.Get1D() * 3), 2);
    float dist = -std::log(1 - sampler.Get1D()) / sigma_t[channel];
    float t = std::min(dist * glm::length(ray.direction), ray.tMax);
    bool sampledMedium = t < ray.tMax;
    std::shared_ptr<PhaseFunction> temp = std::make_shared<HenyeyGreenstein>(g);
    if(sampledMedium)
        *mi = MediumInteraction(ray.origin+ray.direction*t, -ray.direction, ray.time, this,temp);

    //Compute the transmittance and sampling density
    Color3f Tr = glm::exp(std::min(t, 1000000.0f) * glm::length(ray.direction) * -sigma_t);

    //Return weighting factor for scattering from homogeneous medium
    Color3f density = sampledMedium ? (sigma_t * Tr) : Tr;
    float pdf = 0;
    for(int i = 0; i < 3; ++i)
        pdf += density[i];
    pdf *= 1.0f / 3.0f;
    return sampledMedium ? (Tr * sigma_s / pdf) : (Tr / pdf);
}

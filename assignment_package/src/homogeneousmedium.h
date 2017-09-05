#ifndef HOMOGENEOUSMEDIUM_H
#define HOMOGENEOUSMEDIUM_H
#include <medium.h>
#include <raytracing/intersection.h>
#include <raytracing/mediuminteraction.h>


class HomogeneousMedium : public Medium
{
public:
    HomogeneousMedium(const Color3f &sigma_a, const Color3f sigma_s, float g)
        : sigma_a(sigma_a), sigma_s(sigma_s), sigma_t(sigma_s + sigma_a), g(g) {}

    Color3f Tr(const Ray &ray, Sampler &sampler) const
    {
        return glm::exp(-sigma_t * std::min(ray.tMax * glm::length(ray.direction), 10000.0f));
    }

    Color3f Sample(const Ray &ray, Sampler &sampler, MediumInteraction *mi) const;

private:
    const Color3f sigma_a, sigma_s, sigma_t;
    const float g;
};

#endif // HOMOGENEOUSMEDIUM_H

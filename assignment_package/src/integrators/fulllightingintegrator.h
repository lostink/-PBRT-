#pragma once
#include "integrator.h"

class FullLightingIntegrator : public Integrator
{
public:
    FullLightingIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin using multiple importance sampling
    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f accum) const;
};

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf);
float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf);

Color3f UniformSampleOneLight(const ParentIntersection &it, const Scene &scene, Sampler &sampler, bool handleMedia);
Color3f EstimateDirect(const ParentIntersection &it, const Point2f &uScattering, const std::shared_ptr<Light> light, const Point2f &uLight, const Scene &scene, Sampler &sampler, bool handleMedia);

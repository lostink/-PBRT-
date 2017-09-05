#ifndef SPPMINTEGRATOR_H
#define SPPMINTEGRATOR_H

#include <integrators/integrator.h>

//SPPMPixel structure:
//1. stores the current estimated average radiance visible over the extent of a pixel
//2. stores parameters related to the photon density estimation for the pixel
//3. stores the geometric and reflection information for a visible point in the pixel after the camera pass
struct SPPMPixel
{
    float radius = 0.f;
};

class SPPMIntegrator : public Integrator
{
public:
    SPPMIntegrator();

    void render(const Scene &scene);

private:
    std::shared_ptr<const Camera> camera;
    const float initialSearchRadius;
    const int nIterations;
};

#endif // SPPMINTEGRATOR_H

#ifndef MEDIUM_H
#define MEDIUM_H
#include <globals.h>
#include <raytracing/ray.h>
#include <samplers/sampler.h>
class MediumInteraction;
class Ray;
class PhaseFunction {
  public:
    // PhaseFunction Interface
    virtual ~PhaseFunction();
    virtual Float p(const Vector3f &wo, const Vector3f &wi) const = 0;
    virtual Float Sample_p(const Vector3f &wo, Vector3f *wi,
                           const Point2f &u) const = 0;
    virtual std::string ToString() const = 0;
};
// Medium Declarations
class Medium {
  public:
    // Medium Interface
    virtual ~Medium() {}
    virtual Color3f Tr(const Ray &ray, Sampler &sampler) const = 0;
    virtual Color3f Sample(const Ray &ray, Sampler &sampler,
                            MediumInteraction *mi) const = 0;
};

// HenyeyGreenstein Declarations
class HenyeyGreenstein : public PhaseFunction {
  public:
    // HenyeyGreenstein Public Methods
    HenyeyGreenstein(Float g) : g(g) {}
    Float p(const Vector3f &wo, const Vector3f &wi) const;
    Float Sample_p(const Vector3f &wo, Vector3f *wi,
                   const Point2f &sample) const;
  private:
    const Float g;
};
#endif // MEDIUM_H

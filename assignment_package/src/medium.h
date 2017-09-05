#ifndef MEDIUM_H
#define MEDIUM_H
//#include <globals.h>
#include <raytracing/ray.h>
#include <samplers/sampler.h>

class MediumInteraction;


class Medium
{
public:
    virtual ~Medium() {}
    virtual Color3f Tr(const Ray &ray, Sampler &sampler) const = 0;
    virtual Color3f Sample(const Ray &ray, Sampler &sampler,
                            MediumInteraction *mi) const = 0;
};



struct MediumInterface {
    MediumInterface() : inside(nullptr), outside(nullptr) {}
    // MediumInterface Public Methods
    MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
    MediumInterface(const Medium *inside, const Medium *outside)
        : inside(inside), outside(outside) {}
    bool IsMediumTransition() const { return inside != outside; }
    const Medium *inside, *outside;
};





#endif // MEDIUM_H

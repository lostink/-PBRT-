#ifndef MEDIUMINTERACTION_H
#define MEDIUMINTERACTION_H
#include <raytracing/intersection.h>
#include <phasefunction.h>

class Medium;

class MediumInteraction : public Intersection
{
public:
    MediumInteraction() : phase(nullptr) {}
    MediumInteraction(const Point3f &p, const Vector3f &wo, Float time,
                      const Medium *medium, std::shared_ptr<PhaseFunction> phase)
        : Intersection(p, wo, time, medium), phase(phase) {}
    bool IsValid() const { return phase != nullptr; }

    // MediumInteraction Public Data
    //const PhaseFunction *phase;
    std::shared_ptr<PhaseFunction> phase;
};



#endif // MEDIUMINTERACTION_H

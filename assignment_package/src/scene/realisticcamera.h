#ifndef REALISTICCAMERA_H
#define REALISTICCAMERA_H
#include "camera.h"

class realisticCamera: public Camera
{
public:
    realisticCamera();
    realisticCamera(unsigned int w, unsigned int h);
    realisticCamera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp);
    realisticCamera(const Camera &c);
    Ray RayCastreal(const Point2f &pt) const;

private:
    bool simpleWeighting;
    struct LensElementInterface{
        float curvatureRadius;
        float thickness;
        float eta;
        float apertureRadius;
    };
    std::vector<LensElementInterface> elementInterfaces;
    float LensRearZ() const{
        return elementInterfaces.back().thickness;
    }
    float LensFrontZ() const{
        float zSum = 0;
        for (const LensElementInterface &element : elementInterfaces)
            zSum += element.thickness;
        return zSum;
    }
    float RearElementRadius() const{
        return elementInterfaces.back().apertureRadius;
    }
    bool TraceLensesFromFilm(const Ray &rCamera,Ray *rOut) const;
    bool IntersectSphericalElement(Float radius, Float zCenter,
                                                    const Ray &ray, Float *t,
                                                    Normal3f *n) const;
};
bool Quadratic(const Float &A,const Float &B, const Float &C, Float *t0, Float *t1) ;
#endif // REALISTICCAMERA_H

/*
#include "projectionlight.h"

Transform Scale(float x, float y, float z)
{
    Matrix4x4 m(x, 0, 0, 0,
                0, y, 0, 0,
                0, 0, z, 0,
                0, 0, 0, 1);
    Matrix4x4 minv(1/x,  0,   0, 0,
                   0,  1/y,   0, 0,
                   0,    0, 1/z, 0,
                   0,    0,   0, 1);
    return Transform(m, minv);
}

Transform Perspective(float fov, float n, float f)
{
    //Perform projection divide for perspective projection
    Matrix4x4 persp(1, 0,       0,         0,
                    0, 1,       0,         0,
                    0, 0, f/(f-n), -f*n/(f-n),
                    0, 0,       1,         0);
    //Scale canonical perspective view to specified field of view
    float invTanAng = 1/std::tan(fov * Pi / 180 / 2);
    return Scale(invTanAng, invTanAng, 1) * Transform(persp);

}

ProjectionLight::ProjectionLight(const Transform &LightToWorld, const Color3f &I, const std::string &textname, float fov)
    :Light(LightToworld), pLigh(LightToWorld(Point3f(0,0,0))), I(I)
{
    //Create ProjectionLight MIP map
    Point2i resolution;
    std::shared_ptr<QImage> texels = ReadImage(textname,&resolution);
    if(texels)
        projectionMap.reset(new QImage(resolution, texels.get()));

    //Initialize ProjectionLight projrction matrix
    float aspect = ProjectionMap ? (float(resolution.x) / float(resolution.y)) : 1;
    if(aspect > 1)
        screenBounds = Bounds2f(Point2f(-aspect, -1), Point2f(aspect, 1));
    else
        screenBounds = Bounds2f(Point2f(-1, -1/aspect), Point2f(1, 1/aspect));
    tnear = 1e-3f;
    tfar = 1e30f;
    lightProjection = Perspective(fov, tnear, tfar);

    //Compute cosine of cone surrounding projection directions
    float opposite = std::tan(fov * Pi / 180 / 2.f);
    float tanDiag = opposite * std::sqrt(1 + 1/(aspect * aspect));
    cosTotalWidth = std::cos(std::atan(tanDiag));
}

Color3f ProjectionLight::Projection(const Vector3f &w) const
{
    Vector3f wl = WorldToLight(w);
    //Discard directions behind projection light
    if(wl.z < tnear) return 0;
    //Project point onto projection plane and compute light
    Point3f p = lightProjection(Point3f(wl.x, wl.y, wl.z));
    if(!Inside(Point2f(p.x, p.y), screenBounds)) return 0.f;
    if(!projectionMap) return 1;
    Point2f st = Point2f(screenBounds.Offset(Point2f(p.x, p.y)));
    //return Spectrum(projectionMap->Lookup(st), SpectrumType::Illuminant);
}

Color3f ProjectionLight::Power() const
{

    return (projectionMap ?
                Spectrum(projectionMap->Lookup(Point(.5f, .5f), .5f),
                         SpectrumType::Illuminant) : Color3f(1.f)) *
            I * 2 * Pi * (1.f - cosTotalWidth);

}
*/

#ifndef PROJECTIONLIGHT_H
#define PROJECTIONLIGHT_H

#include <scene/lights/light.h>
#include <QImage>

class ProjectionLight : public Light
{
public:
    ProjectionLight(const Transform &LightToWorld, const Color3f &I, const std::string &textname, float fov);

    Color3f Projection(const Vector3f &w) const;

    Color3f Power() const;

private:
    std::shared_ptr<QImage> projectionMap;
    const Point3f pLight;
    const Color3f I;
    Transform lightProjection;
    float tnear, tfar;
    Bounds2f screenBounds;
    float cosTotalWidth;
};

#endif // PROJECTIONLIGHT_H

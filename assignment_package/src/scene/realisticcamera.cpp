#include "realisticcamera.h"



realisticCamera::realisticCamera():
    Camera(400, 400),
    simpleWeighting(0)
{
    look = Vector3f(0,0,-1);
    up = Vector3f(0,1,0);
    right = Vector3f(1,0,0);
}

realisticCamera::realisticCamera(unsigned int w, unsigned int h):
    Camera(w, h, Vector3f(0,0,10), Vector3f(0,0,0), Vector3f(0,1,0)),
    simpleWeighting(0)
{
}

realisticCamera::realisticCamera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp):
    Camera(w,h,e,r,worldUp),
    simpleWeighting(0)
{
    RecomputeAttributes();
}

realisticCamera::realisticCamera(const Camera &c):
    Camera(c),
    simpleWeighting(0)
{
    LensElementInterface test = {-166.7765f,
                                 3.09606f,1.713f,
                                 10.648f};
    elementInterfaces.push_back(test);
}

Ray realisticCamera::RayCastreal(const Point2f &pt) const{
    Ray film = Camera::Raycast(pt);
    Ray result(Point3f(0.f),Vector3f(0.f));
    if (TraceLensesFromFilm(film,&result))
    {
        return result;
    }
    else
    {
        result.origin = Point3f(0,0,0);
        result.direction = Vector3f(200,0,0);
        return result;
    }
}
bool realisticCamera::TraceLensesFromFilm(const Ray &rCamera,Ray *rOut) const{
    Float elementZ = 0;
    // Transform _rCamera_ from camera to lens system space
    static const Transform CameraToLens = Transform(Vector3f(0, 0, 0),
                                                    Vector3f(0, 0, 0),
                                                    Vector3f(1, 1, -1));
    Ray rLens = rCamera.GetTransformedCopy(CameraToLens.T());
    for (int i = elementInterfaces.size() - 1; i >= 0; --i) {
        const LensElementInterface &element = elementInterfaces[i];
        // Update ray from film accounting for interaction with _element_
        elementZ -= element.thickness;

        // Compute intersection of ray with lens element
        Float t;
        Normal3f n;
        bool isStop = (element.curvatureRadius == 0);
        if (isStop)
            t = (elementZ - rLens.origin.z) / rLens.direction.z;
        else {
            Float radius = element.curvatureRadius;
            Float zCenter = elementZ + element.curvatureRadius;
            if (!IntersectSphericalElement(radius, zCenter, rLens, &t, &n))
                return false;
        }
        // Test intersection point against element aperture
        Point3f pHit = rLens.origin;
        Float r2 = pHit.x * pHit.x + pHit.y * pHit.y;
        if (r2 > element.apertureRadius * element.apertureRadius) return false;
        rLens.origin = pHit;
        // Update ray path for element interface interaction
        if (!isStop) {
            Vector3f w;
            Float etaI = element.eta;
            Float etaT = (i > 0 && elementInterfaces[i - 1].eta != 0)
                             ? elementInterfaces[i - 1].eta
                             : 1;
            if (!Refract(glm::normalize(-rLens.direction), n, etaI / etaT, &w)) return false;
            rLens.direction = w;
        }
    }
    // Transform _rLens_ from lens system space back to camera space
    if (rOut != nullptr) {
        static const Transform LensToCamera = Transform(Vector3f(0, 0, 0),
                                                        Vector3f(0, 0, 0),
                                                        Vector3f(1, 1, -1));
        *rOut = rLens.GetTransformedCopy(LensToCamera.T());
    }
    return true;
}
bool realisticCamera::IntersectSphericalElement(Float radius, Float zCenter,
                                                const Ray &ray, Float *t,
                                                Normal3f *n) const{
    // Compute _t0_ and _t1_ for ray--element intersection
    Point3f o = ray.origin - Vector3f(0, 0, zCenter);
    Float A = ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y + ray.direction.z * ray.direction.z;
    Float B = 2 * (ray.direction.x * o.x + ray.direction.y * o.y + ray.direction.z * o.z);
    Float C = o.x * o.x + o.y * o.y + o.z * o.z - radius * radius;
    Float t0, t1;
    if (!Quadratic(A, B, C, &t0, &t1)) return false;

    // Select intersection $t$ based on ray direction and element curvature
    bool useCloserT = (ray.direction.z > 0) ^ (radius < 0);
    *t = useCloserT ? std::min(t0, t1) : std::max(t0, t1);
    if (*t < 0) return false;

    // Compute surface normal of element at ray intersection point
    *n = Normal3f(Vector3f(o + *t * ray.direction));
    *n = Faceforward(glm::normalize(*n), -ray.direction);
    return true;
}
bool Quadratic(const Float &A,
               const Float &B,
               const Float &C,
               Float *t0, Float *t1){
    // Find quadratic discriminant
    double discrim = B * B - 4. * A * C;
    if (discrim < 0.) return false;
    double rootDiscrim = std::sqrt(discrim);

    Float floatRootDiscrim(rootDiscrim);

    // Compute quadratic _t_ values
    Float q;
    if ((float)B < 0)
        q = -.5 * (B - floatRootDiscrim);
    else
        q = -.5 * (B + floatRootDiscrim);
    *t0 = q / A;
    *t1 = C / q;
    if ((float)*t0 > (float)*t1) std::swap(*t0, *t1);
    return true;
}

#include "directlightingintegrator.h"

Color3f DirectLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f accum) const
{
    //TODO
    if (depth < 0)
        return Color3f(0.0f);
    Intersection isx;
    Color3f L = Color3f(0.0f);
    bool flag = scene.Intersect(ray,&isx);
    if (flag){
        if (isx.ProduceBSDF()){
            std::shared_ptr<BSDF> bsdf = isx.bsdf;
            const Normal3f &n = bsdf->normal;
            Vector3f wo = -ray.direction;
            L += isx.Le(wo);

            //Generate wi
            Vector3f wi;
            float pdf;
            const Point2f xi = sampler->Get2D();
            int numL = scene.lights.size();
            float u = numL * sampler->Get1D();
            ParentIntersection p1;
            Color3f li = scene.lights[int(floor(u))]->Sample_Li(isx, xi, &wi, &pdf, &p1);
            //Compute shadow
            Ray ray2light = isx.SpawnRay(-glm::normalize(wi));
            Intersection test;
            scene.Intersect(ray2light, &test);
            if((test.objectHit == nullptr)
                    ||(test.objectHit->areaLight!=scene.lights[int(floor(u))]))
                return L;
            //shadow ends

            Color3f f = bsdf->f(wo,-glm::normalize(wi));
            pdf /= numL;
            if (pdf <=0) return L;
            L += f * li * AbsDot(glm::normalize(wi),n) / pdf;
            return L;
        }
        else
        {
            Vector3f wo = - ray.direction;
            return isx.Le(wo);
        }
    }else
    {
        return Color3f(0.f);
    }
}

#include "naiveintegrator.h"

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f accum) const
{
    //TODO
    if (depth < 0)
        return Color3f(0.0f);
    Intersection isx;
    Color3f L = Color3f(0.0f);
    bool flag = scene.Intersect(ray,&isx);
    if (flag)
    {
        if (isx.ProduceBSDF()){
            std::shared_ptr<BSDF> bsdf = isx.bsdf;
            const Normal3f &n = bsdf->normal;
            Vector3f wo = -ray.direction;
            L += isx.Le(wo);

            Vector3f wi;
            float pdf;
            const Point2f xi = sampler->Get2D();
            BxDFType sampledType;
            Color3f f = bsdf->Sample_f(wo,&wi,xi,&pdf,BSDF_ALL,&sampledType);

            if (pdf <=0) return L;
            Ray r = isx.SpawnRay(wi);
            Color3f li = Li(r,scene,sampler,depth-1,Color3f (1.0));
            L += f * li * AbsDot(wi,n) / pdf;
            return L;
        }
        else
        {
            Vector3f wo = - ray.direction;
            return isx.Le(wo);
        }
    }
    else
    {
        return Color3f(0.f);
    }
}

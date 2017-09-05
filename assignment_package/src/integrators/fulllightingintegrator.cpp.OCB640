#include "fulllightingintegrator.h"
#include <raytracing/MediumInteraction.h>
#include <raytracing/parentintersection.h>

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,Color3f Russian) const
{
    bool specularBounce = false;
    Ray GoingRay = ray;
    Color3f L(0.0f),beta(1.0f);//beta is accumulated ray color, L is throughput color
    while(depth > 0)
    {
        Intersection isect;
        bool foundIntersection = scene.Intersect(GoingRay,&isect);
        MediumInteraction mi;
        if (GoingRay.medium) beta *= GoingRay.medium->Sample(GoingRay, *sampler.get(), &mi);
        if (IsBlack(beta)) break;
        if (mi.IsValid()) {
            L += beta * UniformSampleOneLight(mi, scene, *sampler.get(), true);
            Vector3f wo = -GoingRay.direction, wi;
            mi.phase->Sample_p(wo, &wi, sampler->Get2D());
            GoingRay = mi.SpawnRay(wi);
        }else{
            if (depth == recursionLimit || specularBounce)
            {
                if (foundIntersection)
                    L+=beta * isect.Le(-GoingRay.direction);
                else
                    for (int i = 0;i < scene.lights.size();++i)
                        L+= beta * scene.lights[i]->Le(GoingRay);
            }
            if (!foundIntersection || depth<=0)
                break;
            if (!isect.ProduceBSDF())
            {
                depth --;
                GoingRay = isect.SpawnRay(GoingRay.direction);
                continue;
            }
            Vector3f wi,wo = - GoingRay.direction;
            const Normal3f &n = glm::normalize(isect.bsdf->normal);
            L += beta * UniformSampleOneLight(isect, scene, *sampler.get(), true);////////////////
            float pdf;
            BxDFType flags;
            Color3f f = isect.bsdf->Sample_f(wo,&wi,sampler->Get2D(),&pdf,BSDF_ALL,&flags);
            if ((f == Color3f(0.f)) || (pdf == 0.f))
                break;

            //From course notes: if indirectlight hits light, break
            Intersection indi;
            if (scene.Intersect((isect.SpawnRay(wi)),&indi) && indi.objectHit->areaLight!=nullptr)
                break;
            specularBounce = (flags & BSDF_SPECULAR) != 0;
            GoingRay = isect.SpawnRay(wi);

            beta *= f * AbsDot(glm::normalize(wi),n) / pdf;
        }
        //if (recursionLimit - depth == 0) return f;
        depth--;
        if (recursionLimit - depth > 3){
            float q = glm::max(glm::max(beta[0],beta[1]),beta[2]);
            if (sampler->Get1D() < q)
                break;
            beta /= 1-q;
        }
    }
    return L;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    if (nf * fPdf + ng * gPdf == 0.f) return 0;
    return nf * fPdf / (nf * fPdf + ng * gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    float f = nf * fPdf;
    float g = ng * gPdf;
    if (f * f + g * g == 0) return 0;
    return (f * f) / (f * f + g * g);
}
Color3f UniformSampleOneLight(const ParentIntersection &it, const Scene &scene,
                              Sampler &sampler,
                              bool handleMedia)
{
    int nLights = int(scene.lights.size());
    if (nLights == 0) return Color3f(0.f);
    int lightNum;
    Float lightPdf;
    lightNum = std::min((int)(sampler.Get1D() * nLights), nLights - 1);
    lightPdf = Float(1) / nLights;
    const std::shared_ptr<Light> &light = scene.lights[lightNum];
    Point2f uLight = sampler.Get2D();
    Point2f uScattering = sampler.Get2D();
    return EstimateDirect(it, uScattering, light, uLight,
                          scene, sampler, handleMedia) / lightPdf;
}
Color3f EstimateDirect(const ParentIntersection &it, const Point2f &uScattering,
                       const std::shared_ptr<Light> light, const Point2f &uLight,
                       const Scene &scene, Sampler &sampler,
                       bool handleMedia){
    Color3f Ld(0.f);
    // Sample light source with multiple importance sampling
    Vector3f wi;
    Float lightPdf = 0, scatteringPdf = 0;
    //VisibilityTester visibility;
    ParentIntersection p1;
    Color3f Li = light->Sample_Li(it, uLight, &wi, &lightPdf, &p1);
    if (lightPdf > 0 && !IsBlack(Li)) {
        // Compute BSDF or phase function's value for light sample
        Color3f f;
        if (it.IsSurfaceInteraction()) {
            // Evaluate BSDF for light sampling strategy
            const Intersection &isect = (const Intersection &)it;
            f = isect.bsdf->f(it.wo, wi, BSDF_ALL) *
                AbsDot(wi, isect.normalGeometric);
            scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, BSDF_ALL);
        } else {
            // Evaluate phase function for light sampling strategy
            const MediumInteraction &mi = (const MediumInteraction &)it;
            Float p = mi.phase->p(mi.wo, wi);
            f = Color3f(p);
            scatteringPdf = p;
        }
        if (!IsBlack(f)) {
            // Compute effect of visibility for light source sample
            if (handleMedia) {
                Ray ray(it.SpawnRayTo(p1));
                Color3f Tr(1.f);
                int times = 0;
                while (true) {
                    times++;
                    Intersection isect;
                    bool hitSurface = scene.Intersect(ray, &isect);
                    // Handle opaque surface along ray's path
                    if (hitSurface && isect.objectHit->GetMaterial() != nullptr)
                        break;

                    if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);
                    if (!hitSurface) break;
                    ray = isect.SpawnRayTo(p1);
                    if (times > 1000) break;
                }
                Li *= Tr;
                Ray shadowTest = it.SpawnRay(glm::normalize(-wi));
                Intersection ShadowCheck;
                scene.Intersect(shadowTest,&ShadowCheck);
                Color3f B = Color3f(1.0f);
                if(((ShadowCheck.objectHit == nullptr)
                        ||(ShadowCheck.objectHit->areaLight != light)))
                    B = Color3f(0.f);
                Li*=B;
            } else {
                Ray shadowTest = it.SpawnRay(glm::normalize(-wi));
                Intersection ShadowCheck;
                scene.Intersect(shadowTest,&ShadowCheck);
                Color3f B = Color3f(1.0f);
                if(((ShadowCheck.objectHit == nullptr)
                        ||(ShadowCheck.objectHit->areaLight != light)))
                    B = Color3f(0.f);
                Li*=B;
            }

            // Add light's contribution to reflected radiance
            if (!IsBlack(Li)) {
                    Float weight =
                        PowerHeuristic(1, lightPdf, 1, scatteringPdf);
                    Ld += f * Li * weight / lightPdf;
            }
        }
    }

    // Sample BSDF with multiple importance sampling
    {
        Color3f f;
        bool sampledSpecular = false;
        if (it.IsSurfaceInteraction()) {
            // Sample scattered direction for surface interactions
            BxDFType sampledType;
            const Intersection &isect = (const Intersection &)it;
            f = isect.bsdf->Sample_f(it.wo, &wi, uScattering, &scatteringPdf,
                                     BSDF_ALL, &sampledType);
            f *= AbsDot(wi, isect.normalGeometric);
            sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;
        } else {
            // Sample scattered direction for medium interactions
            const MediumInteraction &mi = (const MediumInteraction &)it;
            Float p = mi.phase->Sample_p(mi.wo, &wi, uScattering);
            f = Color3f(p);
            scatteringPdf = p;
        }
        if (!IsBlack(f) && scatteringPdf > 0) {
            // Account for light contributions along sampled direction _wi_
            Float weight = 1;
            if (!sampledSpecular) {
                lightPdf = light->Pdf_Li(it, wi);
                if (lightPdf == 0) return Ld;
                weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
            }

            // Find intersection and compute transmittance
            Intersection lightIsect;
            Ray ray = it.SpawnRay(wi);
            Color3f Tr(1.f);
            bool foundSurfaceInteraction =
                handleMedia ? scene.IntersectTr(ray, sampler, &lightIsect, &Tr)
                            : scene.Intersect(ray, &lightIsect);

            // Add light contribution from material sampling
            Color3f Li(0.f);
            if (foundSurfaceInteraction) {
                if (lightIsect.objectHit->areaLight == light)
                    Li = lightIsect.Le(-wi);
            } else
                Li = light->Le(ray);
            if (!IsBlack(Li)) Ld += f * Li * Tr * weight / scatteringPdf;
        }
    }
    return Ld;
}

#ifndef SPECTRUM_H
#define SPECTRUM_H

#define Infinity 9999999999999999999999999.0f

#include <globals.h>


class Spectrum;
enum class SpectrumType{Reflectance, Illuminat};

static const int nCIESamples = 471;
extern const float CIE_X[nCIESamples];
extern const float CIE_Y[nCIESamples];
extern const float CIE_Z[nCIESamples];
extern const float CIE_lambda[nCIESamples];
static const float CIE_Y_integral = 106.856895;

inline float Lerp(float t, float s1, float s2)
{
    float result = s1*(1-t)+s2*t;
    return result;
}

template <typename Predicate> int FindInterval(int size,const Predicate &pred)
{
    int first = 0,len = size;
    while(len > 0){
        int half = len >> 1,middle = first + half;
        if (pred(middle)){
            first = middle+1;
            len -= half +1;
        }else
            len = half;
    }
    return glm::clamp(first - 1, 0 ,size -2);
}

float InterpolateSpectrumSamples(const float *lambda, const float *vals, int n, float l)
{
    if(1 <= lambda[0]) return vals[0];
    if(1 >= lambda[n-1]) return vals[n-1];
    int offset = FindInterval(n, [&](int index) {return lambda[index] <= 1;});
    float t = (1-lambda[offset])/(lambda[offset+1] - lambda[offset]);
    return Lerp(t, vals[offset], vals[offset + 1]);
}

//---------------------------CoefficientSpectrum Class---------------------------
template <int nSpectrumSamples> class CoefficientSpectrum
{
public:
    CoefficientSpectrum(float v = 0.f)
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            c[i] = v;
    }

    // ================ Operator functions ================
    CoefficientSpectrum &operator+=(const CoefficientSpectrum &s2)
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            c[i] += s2.c[i];
        return *this;
    }
    CoefficientSpectrum operator+(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum ret = *this;
        for(int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] += s2.c[i];
        return ret;
    }
    CoefficientSpectrum &operator-=(const CoefficientSpectrum &s2)
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            c[i] -= s2.c[i];
        return *this;
    }
    CoefficientSpectrum operator-(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum ret = *this;
        for(int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] -= s2.c[i];
        return ret;
    }
    CoefficientSpectrum &operator*=(float s2)
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            c[i] *= s2;
        return *this;
    }
    CoefficientSpectrum operator*(float s2) const
    {
        CoefficientSpectrum ret = *this;
        for(int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] *= s2;
        return ret;
    }
    CoefficientSpectrum &operator/=(float s2)
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            c[i] /= s2;
        return *this;
    }
    CoefficientSpectrum operator/(float s2) const
    {
        CoefficientSpectrum ret = *this;
        for(int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] /= s2;
        return ret;
    }
    float &operator[](int i) {return c[i];}

    // ================ Other functions ================
    bool IsBlack() const
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            if(c[i] != 0.)return false;
        return true;
    }
    friend CoefficientSpectrum Sqrt(const CoefficientSpectrum &s)
    {
        CoefficientSpectrum ret;
        for(int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = std::sqrt(s.c[i]);
        return ret;
    }
    CoefficientSpectrum Clamp(float low = 0, float high = Infinity) const
    {
        CoefficientSpectrum ret;
        for(int i = 0; i < nSpectrumSamples; ++i)
            ret.c[i] = glm::clamp(c[i], low, high);
        return ret;
    }
    bool HasNaNs() const
    {
        for(int i = 0; i < nSpectrumSamples; ++i)
            if(std::isnan(c[i])) return true;
        return false;
    }

    static const int nSamples = nSpectrumSamples;

protected:
    float c[nSpectrumSamples];

};

//---------------------------Spectrum Class---------------------------
class Spectrum : public CoefficientSpectrum<3>
{
public:
    Spectrum(float v = 0.f) : CoefficientSpectrum<3>(v) {}
    Spectrum(const CoefficientSpectrum<3> &v) : CoefficientSpectrum<3>(v) {}

    static Spectrum FromRGB(const Color3f rgb, SpectrumType type = SpectrumType::Reflectance)
    {
        Spectrum s;
        s.c[0] = rgb[0];
        s.c[1] = rgb[1];
        s.c[2] = rgb[2];
        return s;
    }
    void ToRGB(Color3f &rgb) const
    {
        rgb[0] = c[0];
        rgb[1] = c[1];
        rgb[2] = c[2];
    }
    Spectrum FromXYZ(float *xyz)
    {
        Spectrum s;
        s.c[0] = xyz[0];
        s.c[1] = xyz[1];
        s.c[2] = xyz[2];
        return s;
    }
    void ToXYZ(float *xyz) const
    {
        xyz[0] = c[0];
        xyz[1] = c[1];
        xyz[2] = c[2];
    }

    const Spectrum &ToSpectrum() const {return *this;}
    Spectrum FromSampled(const float *lambda, const float *v, int n)
    {
        float xyz[3] = {0,0,0};
        for(int i = 0; i < nCIESamples; ++i)
        {
            float val = InterpolateSpectrumSamples(lambda, v, n, CIE_lambda[i]);
            xyz[0] += val * CIE_X[i];
            xyz[1] += val * CIE_Y[i];
            xyz[2] += val * CIE_Z[i];
        }
        float scale = float(CIE_lambda[nCIESamples-1] - CIE_lambda[0])/float(CIE_Y_integral*nCIESamples);
        xyz[0] *= scale;
        xyz[1] *= scale;
        xyz[2] *= scale;
        return FromXYZ(xyz);
    }

};

#endif // SPECTRUM_H

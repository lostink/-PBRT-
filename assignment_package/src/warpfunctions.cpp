#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>
#include <globals.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    //TODO
    float r = std::sqrt(sample[0]);
    float angle = TwoPi * sample[1];
    glm::vec3 result(r * cos(angle), r * sin(angle), 0);
    return result;
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    //TODO
    float r, angle;
    float a = 2 * sample[0] - 1;
    float b = 2 * sample[1] - 1;

    if(a > ((-1)*b))
    {
        if(a > b)
        {
            r = a;
            angle = PiOver4 * (b / a);
        }
        else
        {
            r = b;
            angle = PiOver4 * (2.0f - (a / b));
        }
        }
        else
        {
        if(a < b)
        {
            r = (-1) * a;
            angle = PiOver4 * (4 + b / a);
        }
        else
        {
            r = (-1) * b;
            if(!fequal(b, 0.0f))
                angle = PiOver4 * (6.0f - (a / b));
            else
                angle = 0;
        }
    }

    glm::vec3 result(r * cos(angle), r * sin(angle), 0);
    return result;
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    //TODO
    return InvPi;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    //TODO
    float phi = TwoPi * sample[1];
    float theta = acos(1 - sample[0]*2);
    glm::vec3 result(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
    return result;
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    //TODO
    return Inv4Pi;
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    //TODO
    float phi = TwoPi * sample[1];
    float theta = acos(1-((180-thetaMin)/180) * sample[0] * 2);
    glm::vec3 result(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
    return result;
    return Point3f();
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    //TODO
    float angle = Pi *(180-thetaMin)/180;
    return 1/(TwoPi * (1 - cos(angle)));
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    //TODO
    float phi = TwoPi * sample[1];
    float theta = acos(sample[0]);
    glm::vec3 result(sin(theta)*cos(phi), sin(theta)*sin(phi),cos(theta));
    return result;
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    //TODO
    return Inv2Pi;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    //TODO
    float r, angle;
    float a = 2 * sample[0] - 1;
    float b = 2 * sample[1] - 1;

    if(a > ((-1)*b))
    {
        if(a > b)
        {
            r = a;
            angle = PiOver4 * (b / a);
        }
        else
        {
            r = b;
            angle = PiOver4 * (2.0f - (a / b));
        }
    }
    else
    {
        if(a < b)
        {
            r = (-1) * a;
            angle = PiOver4 * (4 + b / a);
        }
        else
        {
            r = (-1) * b;
            if(!fequal(b, 0.0f))
                angle = PiOver4 * (6.0f - (a / b));
            else
                angle = 0;
        }
    }

    float disk_x = r * cos(angle);
    float disk_y = r * sin(angle);

    glm::vec3 result(disk_x, disk_y, std::sqrt(1-disk_x*disk_x-disk_y*disk_y));
    return result;
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    //TODO
    return (sample[2]*InvPi);
}

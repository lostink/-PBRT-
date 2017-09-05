#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

Bounds3f Triangle::WorldBound() const
{
    //TODO
    Point3f min = points[0], max=points[0];
    for(int i = 0; i < 3; ++i)
    {
        min[i] = std::min(std::min(points[0][i], points[1][i]), points[2][i]);
        max[i] = std::max(std::max(points[0][i], points[1][i]), points[2][i]);
    }
    Bounds3f ret(min, max);
    return ret.Apply(transform);
}

float Triangle::Area() const
{
    return glm::length(glm::cross(points[0] - points[1], points[2] - points[1])) * 0.5f;
}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = planeNormal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    planeNormal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

float TriArea(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
Normal3f Triangle::GetNormal(const Point3f &P) const
{
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], P);
    float A1 = TriArea(points[0], points[2], P);
    float A2 = TriArea(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}


//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
bool Triangle::Intersect(const Ray& r, Intersection* isect) const
{
    //1. Ray-plane intersection
    float t =  glm::dot(planeNormal, (points[0] - r.origin)) / glm::dot(planeNormal, r.direction);
    if(t < 0) return false;

    glm::vec3 P = r.origin + t * r.direction;
    //2. Barycentric test
    float S = 0.5f * glm::length(glm::cross(points[0] - points[1], points[0] - points[2]));
    float s1 = 0.5f * glm::length(glm::cross(P - points[1], P - points[2]))/S;
    float s2 = 0.5f * glm::length(glm::cross(P - points[2], P - points[0]))/S;
    float s3 = 0.5f * glm::length(glm::cross(P - points[0], P - points[1]))/S;
    float sum = s1 + s2 + s3;

    if(s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && fequal(sum, 1.0f)){
        isect->t = t;
        InitializeIntersection(isect, t, Point3f(P));
        return true;
    }
    return false;
}

void Triangle::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = pLocal;
    isect->uv = GetUVCoordinates(pLocal);
    ComputeTriangleTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent), isect->uv);
    isect->t = t;
}

void Triangle::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    //Triangle uses ComputeTriangleTBN instead of this function.
    Point3f LocalP = glm::vec3(this->transform.invT() * glm::vec4(P,1));
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], LocalP);
    float A1 = TriArea(points[0], points[2], LocalP);
    float A2 = TriArea(points[0], points[1], LocalP);
    Point2f UV = uvs[0] * A0 / A + uvs[1] * A1 / A + uvs[2] * A2 / A;
    ComputeTriangleTBN(P,nor,tan,bit,UV);
}

void Triangle::ComputeTriangleTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit, const Point2f &uv) const
{
    *nor = GetNormal(P);
    //TODO: Compute tangent and bitangent based on UV coordinates.
    *nor = this->GetNormal(P);
    Point3f v0 = this->points[0];
    Point3f v1 = this->points[1];
    Point3f v2 = this->points[2];

    Point2f uv0 = this->uvs[0];
    Point2f uv1 = this->uvs[1];
    Point2f uv2 = this->uvs[2];

    Point3f deltaPos1 = v1 - v0;
    Point3f deltaPos2 = v2 - v0;

    Point2f deltaUV1 = uv1 - uv0;
    Point2f deltaUV2 = uv2 - uv0;
    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    *tan = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
    *bit = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

    *nor = glm::vec3(this->transform.T() * glm::vec4((*nor),1));
    *tan = glm::vec3(this->transform.T() * glm::vec4((*tan),1));
    *bit = glm::vec3(this->transform.T() * glm::vec4((*bit),1));
}


Intersection Triangle::Sample(const Point2f &xi, Float *pdf) const
{
    //TODO for extra credit
    return Intersection();
}


Point2f Triangle::GetUVCoordinates(const Point3f &point) const
{
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], point);
    float A1 = TriArea(points[0], points[2], point);
    float A2 = TriArea(points[0], points[1], point);
    Point2f result = uvs[0] * A0/A + uvs[1] * A1/A + uvs[2] * A2/A;
    if (result[0] > 1)  result[0] -=1.0f;
    if (result[0] < 0)  result[0] +=1.0f;
    if (result[1] > 1)  result[1] -=1.0f;
    if (result[1] < 0)  result[1] +=1.0f;
    return result;
}

void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path, const Transform &transform)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1 = glm::vec3(transform.T() * glm::vec4(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2], 1));
                glm::vec3 p2 = glm::vec3(transform.T() * glm::vec4(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2], 1));
                glm::vec3 p3 = glm::vec3(transform.T() * glm::vec4(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2], 1));

                auto t = std::make_shared<Triangle>(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1 = transform.invTransT() * glm::vec3(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2 = transform.invTransT() * glm::vec3(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3 = transform.invTransT() * glm::vec3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
        //TODO: .mtl file loading
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

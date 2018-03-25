#ifndef _PLANE_H__
#define _PLANE_H__

#include "object.h"
#include "config.h"
#include "iostream"
class Plane : public Object
{
  public:
    glm::vec3 center;
    glm::vec3 normal;
    float gridSize;
    Plane(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, float refr,
          glm::vec3 cen, float size)
        : Object(id, amb, dif, spe, shine, refl, refr)
    {
        center = cen;
        gridSize = size;
        normal = glm::vec3{0.0, 1.0, 0.0};
        type = PLANE;
    }

    float intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit);

    glm::vec3 getNormal(glm::vec3 point);
    glm::vec3 getAmbient(glm::vec3 point);
    glm::vec3 getDiffuse(glm::vec3 point);
    glm::vec3 getSpecular(glm::vec3 point);

    glm::vec3 getLocalColor(glm::vec3 point);
};

glm::vec3 Plane::getNormal(glm::vec3 point)
{
    return normal;
}

glm::vec3 Plane::getAmbient(glm::vec3 point)
{
    return mat_ambient;
}

glm::vec3 Plane::getDiffuse(glm::vec3 point)
{
    return glm::vec3{mat_diffuse.x * getLocalColor(point).x,
                     mat_diffuse.y * getLocalColor(point).y,
                     mat_diffuse.z * getLocalColor(point).z};
}

glm::vec3 Plane::getSpecular(glm::vec3 point)
{
    return mat_specular;
}

glm::vec3 Plane::getLocalColor(glm::vec3 point)
{
    int i = abs(int(point.x - center.x)) / gridSize;
    int j = abs(int(point.z - center.z)) / gridSize;
    // std::cout << "return palne color!" << std::endl;
    if ((i + j) % 2 == 0)
    {
        // black color
        return glm::vec3{0.0, 0.0, 0.0};
    }
    else
    {
        // whie color
        return glm::vec3{1.0, 1.0, 1.0};
    }
}

float Plane::intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit)
{
    ray = glm::normalize(ray);
    // whether the ray parallel
    if (ray.y > -precision && ray.y < precision)
    {
        return -1.0;
    }

    float k = center.y / ray.y;

    if (k < 0)
    {
        return -1.0;
    }

    // set hit
    hit->x = (eye + k * ray).x;
    hit->y = (eye + k * ray).y;
    hit->z = (eye + k * ray).z;
    // std::cout << "hit the plane!" << std::endl;
    return k;
}

#endif
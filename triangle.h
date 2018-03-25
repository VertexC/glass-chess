#ifndef _TRIANGLE_H__
#define _TRIANGLE_H__

#include "object.h"
#include "config.h"

class Triangle : public Object
{
  public:
    glm::vec3 vertexes[3];
    glm::vec3 normal;

    Triangle(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, float refr,
             glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
        : Object(id, amb, dif, spe, shine, refl, refr)
    {
        vertexes[0] = p0;
        vertexes[1] = p1;
        vertexes[2] = p2;
        normal = glm::normalize(glm::cross(p1 - p0, p2 - p1));
        type = TRIANGLE;
    }

    float intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit);
};

float Triangle::intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit)
{

    // find the intersection point on triangle

    /**
     * P on plane, A = vertex[0]
     * AP * normal = 0
     * eye + k*ray = P
     * => k = [(A - eye) * n] / [ray * n]
    */

    ray = glm::normalize(ray);

    float divisor = glm::dot(ray, normal);
    float dividend = glm::dot((vertexes[0] - eye), normal);
    // whether the ray is parallel to the tiangle
    if (-precision < divisor && divisor < precision)
        return -1.0;

    float k = dividend / divisor;
    if (k < 0)
        return -1.0;

    glm::vec3 intersectPoint = k * ray + eye;

    // whether point inside the triangle

    /**
     * A->B->C as the vertex of triangle
     * P as the intersection point
     * AB cross AP should be paraller to normal, same for BC, CA
    */

    for (int i = 0; i < 3; i++)
    {
        glm::vec3 edge = vertexes[(i + 1) % 3] - vertexes[i];
        glm::vec3 vp = intersectPoint - vertexes[i];

        if (glm::dot(normal, glm::cross(edge, vp)) < 0)
        {
            return -1.0;
        }
    }
    // set the hit point
    hit->x = intersectPoint.x;
    hit->y = intersectPoint.y;
    hit->z = intersectPoint.z;

    return k;
}

#endif
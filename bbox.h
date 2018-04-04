#ifndef _BBOX_H__
#define _BBOX_H__

#include <stdint.h>
#include "config.h"
#include <iostream>
struct BBox
{
    glm::vec3 min_p, max_p, extent;
    BBox() {}
    BBox(glm::vec3 _min_p, glm::vec3 _max_p) : min_p(_min_p), max_p(_max_p)
    {
        extent = _max_p - _min_p;
        // std::cout << min_p.x << " " << min_p.y << "" << min_p.z << std::endl;
        // std::cout << max_p.x << " " << max_p.y << "" << max_p.z << std::endl;
    }
    BBox(glm::vec3 point) : min_p(point), max_p(point)
    {
        extent = point - point;
    }

    void expandToInclude(glm::vec3 p)
    {
        min_p = min_vec3(min_p, p);
        max_p = max_vec3(max_p, p);
        extent = max_p - min_p;
        // std::cout << min_p.x << " " << min_p.y << "" << min_p.z << std::endl;
        // std::cout << max_p.x << " " << max_p.y << "" << max_p.z << std::endl;
    }

    void expandToInclude(BBox box)
    {
        min_p = min_vec3(min_p, box.min_p);
        max_p = max_vec3(max_p, box.max_p);
        extent = max_p - min_p;
        // std::cout << min_p.x << " " << min_p.y << "" << min_p.z << std::endl;
        // std::cout << max_p.x << " " << max_p.y << "" << max_p.z << std::endl;
    }

    uint32_t maxDimension()
    {
        uint32_t result = 0;
        if (extent.y > extent.x && extent.y > extent.z)
            return 1;
        if (extent.z > extent.y && extent.z > extent.x)
            return 2;
        return result;
    }

    bool intersect(glm::vec3 eye, glm::vec3 ray, float &distance)
    {
        // if (min_p.x != 0.0f)
        // {
        //     std::cout << min_p.x << " " << min_p.y << " " << min_p.z << std::endl;
        //     std::cout << max_p.x << " " << max_p.y << " " << max_p.z << std::endl;
        // }

        ray = glm::normalize(ray);
        double t_xmin = (min_p.x - eye.x) / ray.x;
        double t_xmax = (max_p.x - eye.x) / ray.x;
        if (t_xmin > t_xmax)
            std::swap(t_xmin, t_xmax);

        double t_ymin = (min_p.y - eye.y) / ray.y;
        double t_ymax = (max_p.y - eye.y) / ray.y;
        if (t_ymin > t_ymax)
            std::swap(t_ymin, t_ymax);

        if (t_xmax < t_ymin || t_ymax < t_xmin)
            return false;
        double t_xymin = std::max(t_xmin, t_ymin);
        double t_xymax = std::min(t_xmax, t_ymax);

        double t_zmin = (min_p.z - eye.z) / ray.z;
        double t_zmax = (max_p.z - eye.z) / ray.z;
        if (t_zmin > t_zmax)
            std::swap(t_zmin, t_zmax);

        bool hit = t_zmax >= t_xymin && t_xymax >= t_zmin;
        if (hit)
        {
            distance = std::max(t_xymin, t_zmin);
        }
        return hit;
    }
};

#endif
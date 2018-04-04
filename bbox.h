#ifndef _BBOX_H__
#define _BBOX_H__

#include <stdint.h>
#include "config.h"
struct BBox
{
    glm::vec3 min_p, max_p, extent;
    BBox() {}
    BBox(glm::vec3 _min_p, glm::vec3 _max_p) : min_p(_min_p), max_p(_max_p)
    {
        extent = _max_p - _min_p;
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
    }

    void expandToInclude(BBox box)
    {
        min_p = min_vec3(min_p, box.min_p);
        max_p = max_vec3(max_p, box.max_p);
        extent = max_p - min_p;
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
};

#endif
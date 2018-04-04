#ifndef _INTERSECTINFO_H__
#define _INTERSECTINFO_H__

#include "object.h"
#include "config.h"

struct IntersectInfo{
    float distance;
    Object * object;
    glm::vec3 hit;
};

#endif